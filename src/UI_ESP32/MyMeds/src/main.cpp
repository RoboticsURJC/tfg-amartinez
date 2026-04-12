#include <Arduino.h>
#include <WiFi.h>
#include <lvgl.h>

#include <TFT_eSPI.h>
extern TFT_eSPI tft;

#include <WebServer.h>
WebServer server(80);

#include "lvgl_display.h"
#include "lvgl_touch.h"

#include "user_interface/wifi_mod.h"
#include "user_interface/wifi_portal.h"
#include "user_interface/clock_mod.h"

#include "logo_mymeds.h"

static const int LOGO_TIME_MS = 4000;

static bool wifi_connected = false;
static bool portal_running = false;

bool device_linked = false;
static bool clock_started = false;   // 🔥 control de pantalla

static lv_timer_t *clock_timer = nullptr;

void setup()
{
    Serial.begin(115200);
    delay(200);

    lvgl_begin();

    // --- Control logo ---
    Preferences p;
    p.begin("sys", false);
    bool skipLogo = p.getBool("skip_logo", false);
    p.putBool("skip_logo", false);
    p.end();

    if (!skipLogo){
        lv_obj_t *logo_screen = lv_obj_create(NULL);
        lv_scr_load(logo_screen);

        lv_obj_t *img = lv_img_create(logo_screen);
        lv_img_set_src(img, &logo_mymeds_blanco);
        lv_img_set_zoom(img, 250);
        lv_obj_center(img);

        unsigned long start = millis();
        while (millis() - start < LOGO_TIME_MS) {
            lv_timer_handler();
            delay(5);
        }

        Serial.println("LOGO END");
    }

    // --- WiFi ---
    WiFi.mode(WIFI_AP_STA);
    WiFi.disconnect(true);
    delay(100);

    // --- Intentar conexión guardada ---
    if (wifi_credentials_exists()){
        String ssid, password;
        wifi_load_credentials(ssid, password);
        WiFi.begin(ssid.c_str(), password.c_str());

        unsigned long start = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - start < 10000){
            delay(100);
            Serial.print(".");
        }

        Serial.println(WiFi.status());

        if (WiFi.status() == WL_CONNECTED){

            wifi_connected = true;

            Serial.println("\nConectado al WiFi!");
            Serial.println(WiFi.localIP());

            // --- Servidor ---
            server.on("/", handle_web_root);
            server.on("/takes", HTTP_POST, handle_takes);
            server.on("/link", HTTP_GET, handle_link);
            server.begin();

            // --- Mostrar QR con IP ---
            String ip = WiFi.localIP().toString();
            String url = "http://" + ip;

            Serial.print("Device URL: ");
            Serial.println(url);

            lv_obj_clean(lv_scr_act());
            show_wifi_screen(
                lv_scr_act(),
                "Escanea para vincular con la app",
                url.c_str()
            );

            return;
        }
    }

    // --- Modo AP ---
    lv_obj_clean(lv_scr_act());
    show_wifi_screen(
        lv_scr_act(),
        "Escanea para configurar WiFi",
        "http://192.168.4.1"
    );

    wifi_portal_init();
    portal_running = true;
}

void loop()
{
    lv_timer_handler();
    delay(5);

    server.handleClient();

    // --- Conexión WiFi desde AP ---
    if (!wifi_connected && WiFi.status() == WL_CONNECTED){

        wifi_connected = true;

        if (portal_running) {
            server.stop();
        }

        Serial.println("\nConectado al WiFi!");
        Serial.println(WiFi.localIP());

        // --- Servidor ---
        server.on("/", handle_web_root);
        server.on("/takes", HTTP_POST, handle_takes);
        server.on("/link", HTTP_GET, handle_link);
        server.begin();

        // --- Mostrar QR con IP ---
        String ip = WiFi.localIP().toString();
        String url = "http://" + ip;

        Serial.print("Device URL: ");
        Serial.println(url);

        lv_obj_clean(lv_scr_act());
        show_wifi_screen(
            lv_scr_act(),
            "Escanea para vincular con la app",
            url.c_str()
        );
    }

    // --- SINCRONIZACIÓN CON APP ---
    if (device_linked && !clock_started) {

        device_linked = false;
        clock_started = true;

        Serial.println("App sincronizada → mostrando reloj");

        lv_obj_clean(lv_scr_act());
        show_clock_screen(lv_scr_act());

        lv_timer_create(update_clock_task, 1000, NULL);
        clock_sync();
    }
}