#include <Arduino.h>
#include <WiFi.h>
#include <lvgl.h>

#include <TFT_eSPI.h>
extern TFT_eSPI tft;

#include <WebServer.h>
extern WebServer server;

#include "lvgl_display.h"
#include "lvgl_touch.h"

#include "user_interface/wifi_mod.h"
#include "user_interface/wifi_portal.h"
#include "user_interface/clock_mod.h"

#include "logo_mymeds_240_swapped_from_original.h"

static const int LOGO_TIME_MS = 4000;
static bool wifi_connected = false;

static lv_timer_t *clock_timer = nullptr;

void draw_logo()    //Show logo with TFT library
{
    tft.startWrite();
    tft.pushImage(
        40,
        0,
        240,
        240,
        logo_mymeds_240
    );
    tft.endWrite();
}

void setup()
{
    Serial.begin(115200);
    delay(200);

    // TFT
    if (is_first_boot()){   //Show logo only on first boot
        tft.begin();
        tft.setRotation(3);
        tft.fillScreen(TFT_WHITE);
        draw_logo();
        delay(LOGO_TIME_MS);
    }

    // LVGL
    lvgl_begin();

    if (wifi_credentials_exists()){
        String ssid, password;
        wifi_load_credentials(ssid, password);
        WiFi.begin(ssid.c_str(), password.c_str());

        unsigned long start = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - start < 5000){
            delay(100);
        }

        if (WiFi.status() == WL_CONNECTED){
            wifi_connected = true;

            lv_obj_clean(lv_scr_act());
            show_clock_screen(lv_scr_act());
            lv_timer_create(update_clock_task, 1000, NULL);
            clock_sync();
            
            return;
        }
    }

    lv_obj_clean(lv_scr_act());
    show_wifi_screen(lv_scr_act());
    wifi_portal_init();
}

void loop(){
    lv_timer_handler();
    server.handleClient();
    delay(5);

    if (!wifi_connected && WiFi.status() == WL_CONNECTED){
        wifi_connected = true;

        lv_obj_clean(lv_scr_act());
        show_clock_screen(lv_scr_act());
        lv_timer_create(update_clock_task, 1000, NULL);
        clock_sync();
    }
}
