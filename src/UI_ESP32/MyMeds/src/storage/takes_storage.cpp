#include <Preferences.h>
#include "user_interface/pill_takes.h"
#include "Arduino.h"
#include "user_interface/wifi_portal.h"

void initNVS()
{
    prefs.begin("takes", total_takes);
}

void saveTakes()
{
    prefs.putInt("total", total_takes);

    for (int i = 0; i < total_takes; i++){
        char key[20];
        sprintf(key, "hour%d", i);
        prefs.putString(key, takes[i].hour);

        sprintf(key, "rep%d", i);
        uint8_t bits = 0;
        for (int day = 0; day < 7; day++){
            if (takes[i].repeat[day]){
                bits |= (1 << day);
            }
        }
        prefs.putUChar(key, bits);

        sprintf(key, "rec%d", i);
        prefs.putBool(key, takes[i].recordatory);

        sprintf(key, "warning%d", i);
        prefs.putInt(key, takes[i].warning_time);
    }
}

void uploadTakes()
{
    total_takes = prefs.getInt("total", 0);
    if (total_takes <= 0){
        return;
    }

    for ( int i = 0; i < total_takes; i++){
        char key[20];
        sprintf(key, "hour%d", i);
        String h = prefs.getString(key, "00:00");
        strcpy(takes[i].hour, h.c_str());

        sprintf(key, "rep%d", i);
        uint8_t bits = prefs.getUChar(key, 0X7F);
        for (int day = 0; day < 7; day++){
            takes[i].repeat[day] = bits & (1 << day);
        }

        sprintf(key, "rec%d", i);
        takes[i].recordatory = prefs.getBool(key, true);

        sprintf(key, "warning%d", i);
        takes[i].warning_time = prefs.getInt(key, 0);
    }
}