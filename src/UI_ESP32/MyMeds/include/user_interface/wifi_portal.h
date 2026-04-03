#pragma once
#include <Arduino.h>
#include <Preferences.h>

extern Preferences prefs;

void handle_web_root();
void handle_save();
void handle_takes();
void wifi_portal_init();
bool wifi_credentials_exists();
void wifi_load_credentials(String &ssid, String &password);