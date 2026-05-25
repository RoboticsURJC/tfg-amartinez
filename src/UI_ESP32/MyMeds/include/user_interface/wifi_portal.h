#pragma once
#include <Arduino.h>
#include <Preferences.h>

extern Preferences prefs;
extern String DEVICE_PIN;

void handle_web_root();
void handle_save();
void handle_get_takes();
void handle_add_take();
void handle_update_take();
void handle_delete_take();
void handle_set_medicines();
void handle_get_medicines();
void handle_set_pin();
void handle_get_pin();
void handle_takes();
void handle_link();
void wifi_portal_init();
bool wifi_credentials_exists();
void wifi_load_credentials(String &ssid, String &password);