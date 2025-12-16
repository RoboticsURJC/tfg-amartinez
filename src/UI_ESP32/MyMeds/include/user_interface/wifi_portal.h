#pragma once
#include <Arduino.h>
#include <Preferences.h>

extern Preferences prefs;

bool is_first_boot();
void set_first_boot(bool v);
void wifi_portal_init();
bool wifi_credentials_exists();
void wifi_load_credentials(String &ssid, String &password);