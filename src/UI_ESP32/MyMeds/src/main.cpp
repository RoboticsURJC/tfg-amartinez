#include <Arduino.h>
#include <WiFi.h>
#include <lvgl.h>

#include <TFT_eSPI.h>
extern TFT_eSPI tft;

#include <WebServer.h>
WebServer server(80);
extern String DEVICE_TOKEN;
String DEVICE_TOKEN = "";

String DEVICE_PIN = "1234";

#include <WiFiUdp.h>
WiFiUDP udp;

#include "lvgl_display.h"
#include "lvgl_touch.h"

#include "user_interface/wifi_mod.h"
#include "user_interface/wifi_portal.h"
#include "user_interface/clock_mod.h"
#include "user_interface/pill_takes.h"

#include "storage/takes_storage.h" 
#include "storage/medicines_storage.h" 

#include "logo_mymeds.h"

#include <Preferences.h>

static const int LOGO_TIME_MS = 4000;

static bool wifi_connected = false;
static bool portal_running = false;

bool device_linked = false;
static bool clock_started = false;

static lv_timer_t *clock_timer = nullptr;

String generateToken()
{
    const char charset[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    String token = "";

    for (int i = 0; i < 16; i++) {

        token += charset[
            random(sizeof(charset) - 1)
        ];
    }

    return token;
}

void handle_not_found() {
    Serial.println("----- NOT FOUND -----");
    Serial.print("URI: ");
    Serial.println(server.uri());

    Serial.print("Method: ");
    Serial.println(server.method());

    Serial.print("Args: ");
    Serial.println(server.args());

    for (uint8_t i = 0; i < server.args(); i++) {
        Serial.print(" - ");
        Serial.print(server.argName(i));
        Serial.print(": ");
        Serial.println(server.arg(i));
    }

    Serial.println("---------------------");

    server.send(404, "text/plain", "Not found");
}

bool isAuthorized()
{
    Serial.println("=== HEADERS ===");

    for (int i = 0; i < server.headers(); i++)
    {
        Serial.print(server.headerName(i));
        Serial.print(" = ");
        Serial.println(server.header(i));
    }

    Serial.println("===============");
    String token = server.header("X-DEVICE-TOKEN");

    Serial.print("TOKEN RECIBIDO [");
    Serial.print(token.length());
    Serial.println("]");

    Serial.println(token);

    Serial.print("TOKEN ESP [");
    Serial.print(DEVICE_TOKEN.length());
    Serial.println("]");

    Serial.println(DEVICE_TOKEN);

    if (token != DEVICE_TOKEN) {

        Serial.println("Unauthorized request");
        Serial.println(server.uri());

        server.send(401, "text/plain", "Unauthorized");

        return false;
    }

    return true;
}

void setup()
{
    Serial.begin(115200);
    delay(200);

    lvgl_begin();

    loadMedicines();
    uploadTakes();
    
    Preferences p;
    p.begin("sys", false);
    device_linked = p.getBool("linked", false);
    String savedToken = p.getString("token", "");

    DEVICE_PIN = p.getString("pin", "1234");

    if (savedToken.isEmpty()) {
        DEVICE_TOKEN = generateToken();
        p.putString("token", DEVICE_TOKEN);
        Serial.println("Nuevo token generado:");

    } else {
        DEVICE_TOKEN = savedToken;
        Serial.println("Token cargado:");
    }

    Serial.println(DEVICE_TOKEN);
    Serial.println("PIN cargado:");
    Serial.println(DEVICE_PIN);
    p.end();

    // --- Logo ---
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

    if (wifi_credentials_exists()){
        String ssid, password;
        wifi_load_credentials(ssid, password);
        WiFi.begin(ssid.c_str(), password.c_str());

        unsigned long start = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - start < 10000){
            delay(100);
            Serial.print(".");
        }

        if (WiFi.status() == WL_CONNECTED){

            wifi_connected = true;

            Serial.println("\nConectado al WiFi!");
            Serial.println(WiFi.localIP());

            Serial.println("---- TAKES PARSED (WIFI CONNECT) ----");
            Serial.print("Total takes: ");
            Serial.println(total_takes);

            udp.begin(8888);
            Serial.println("UDP listo en puerto 8888");

            server.on("/", handle_web_root);

            server.on("/save", HTTP_POST, handle_save);

            server.on("/ping", HTTP_GET, [](){

                server.send(
                    200,
                    "text/plain",
                    "MYMEDS_ESP_OK"
                );
            });

            server.on("/pin", HTTP_POST, [](){

                if (!isAuthorized())
                    return;

                handle_set_pin();
            });

            server.on("/takes", HTTP_GET, [](){

                if (!isAuthorized())
                    return;

                handle_get_takes();
            });

            server.on("/takes", HTTP_POST, [](){

                if (!isAuthorized())
                    return;

                handle_takes();
            });

            server.on("/take", HTTP_POST, [](){

                if (!isAuthorized())
                    return;

                handle_add_take();
            });

            server.on("/take", HTTP_PUT, [](){

                if (!isAuthorized())
                    return;

                handle_update_take();
            });

            server.on("/take", HTTP_DELETE, [](){

                if (!isAuthorized())
                    return;

                handle_delete_take();
            });

            server.on("/medicines", HTTP_POST, [](){

                if (!isAuthorized())
                    return;

                handle_set_medicines();
            });

            server.on("/medicines", HTTP_GET, [](){

                if (!isAuthorized())
                    return;

                handle_get_medicines();
            });

            server.on("/link", HTTP_GET, handle_link);

            server.onNotFound(handle_not_found);

            const char* headerKeys[] = {
                "X-DEVICE-TOKEN"
            };

            server.collectHeaders(
                headerKeys,
                1
            );

            server.begin();

            if (device_linked) {

                Serial.println("Ya vinculado → cargando reloj");

                lv_obj_clean(lv_scr_act());
                show_clock_screen(lv_scr_act());

                lv_timer_create(update_clock_task, 1000, NULL);
                clock_sync();

                clock_started = true;

            } else {

                Serial.println("Esperando conexión de la app...");

                lv_obj_clean(lv_scr_act());
                lv_obj_t *label = lv_label_create(lv_scr_act());
                lv_label_set_text(label, "Conectando\ncon la app...");
                lv_obj_center(label);
            }

            return;
        }
    }

    // --- Modo AP ---
    lv_obj_clean(lv_scr_act());
    show_wifi_screen(
        lv_scr_act(),
        "Configurar WiFi",
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

    // --- DISCOVERY UDP ---
    int packetSize = udp.parsePacket();

    if (packetSize) {

        char incoming[255];
        int len = udp.read(incoming, 255);

        if (len > 0) incoming[len] = 0;

        String message = String(incoming);

        Serial.print("UDP recibido: ");
        Serial.println(message);

        if (message == "DISCOVER_ESP") {

            Serial.println("Respondiendo a la app...");

            udp.beginPacket(udp.remoteIP(), udp.remotePort());
            udp.write((const uint8_t*)"ESP_HERE", 8);
            udp.endPacket();
        }
    }

    // --- Conexión WiFi desde AP ---
    if (!wifi_connected && WiFi.status() == WL_CONNECTED){

        wifi_connected = true;

        if (portal_running) {
            server.stop();
            delay(200);
            server.close();
            delay(200);
        }

        Serial.println("\nConectado al WiFi!");
        Serial.println(WiFi.localIP());

        Serial.println("---- TAKES PARSED (AP → WIFI) ----");
        Serial.print("Total takes: ");
        Serial.println(total_takes);

        udp.begin(8888);
        Serial.println("UDP listo en puerto 8888");

        server.on("/", handle_web_root);

        server.on("/save", HTTP_POST, handle_save);

        server.on("/ping", HTTP_GET, [](){

            server.send(
                200,
                "text/plain",
                "MYMEDS_ESP_OK"
            );
        });

        server.on("/pin", HTTP_POST, [](){

            if (!isAuthorized())
                return;

            handle_set_pin();
        });

        server.on("/takes", HTTP_GET, [](){

            if (!isAuthorized())
                return;

            handle_get_takes();
        });

        server.on("/takes", HTTP_POST, [](){

            if (!isAuthorized())
                return;

            handle_takes();
        });

        server.on("/take", HTTP_POST, [](){

            if (!isAuthorized())
                return;

            handle_add_take();
        });

        server.on("/take", HTTP_PUT, [](){

            if (!isAuthorized())
                return;

            handle_update_take();
        });

        server.on("/take", HTTP_DELETE, [](){

            if (!isAuthorized())
                return;

            handle_delete_take();
        });

        server.on("/medicines", HTTP_POST, [](){

            if (!isAuthorized())
                return;

            handle_set_medicines();
        });

        server.on("/medicines", HTTP_GET, [](){

            if (!isAuthorized())
                return;

            handle_get_medicines();
        });

        server.on("/link", HTTP_GET, handle_link);

        server.onNotFound(handle_not_found);

        const char* headerKeys[] = {
            "X-DEVICE-TOKEN"
        };

        server.collectHeaders(
            headerKeys,
            1
        );

        server.begin();

        Serial.println("Servidor HTTP listo");

        if (device_linked) {

            Serial.println("Ya vinculado → cargando reloj");

            lv_obj_clean(lv_scr_act());
            show_clock_screen(lv_scr_act());

            lv_timer_create(update_clock_task, 1000, NULL);
            clock_sync();

            clock_started = true;

        } else {

            lv_obj_clean(lv_scr_act());
            lv_obj_t *label = lv_label_create(lv_scr_act());
            lv_label_set_text(label, "Conectando\ncon la app...");
            lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
            lv_obj_center(label);
        }
    }

    // --- SINCRONIZACIÓN CON APP ---
    if (device_linked && wifi_connected && !clock_started) {

        clock_started = true;

        Serial.println("App sincronizada → mostrando reloj");

        lv_obj_clean(lv_scr_act());
        show_clock_screen(lv_scr_act());

        lv_timer_create(update_clock_task, 1000, NULL);
        clock_sync();
    }
}