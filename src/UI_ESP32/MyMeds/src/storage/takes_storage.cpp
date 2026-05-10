#include <Preferences.h>
#include "user_interface/pill_takes.h"
#include "Arduino.h"
#include "user_interface/wifi_portal.h"

void initNVS()
{
    prefs.begin("takes", false);
}

void saveTakes()
{
    prefs.begin("takes", false);

    prefs.clear();

    prefs.putInt("total", total_takes);

    for (int i = 0; i < total_takes; i++) {

        char key[32];

        // ---------- ID ----------
        sprintf(key, "id%d", i);
        prefs.putString(key, takes[i].id);

        // ---------- HOUR ----------
        sprintf(key, "hour%d", i);
        prefs.putString(key, takes[i].hour);

        // ---------- REPEAT ----------
        sprintf(key, "rep%d", i);
        prefs.putUChar(key, takes[i].repeat_mask);

        // ---------- REMINDER ----------
        sprintf(key, "rec%d", i);
        prefs.putBool(key, takes[i].recordatory);

        // ---------- WARNING ----------
        sprintf(key, "warn%d", i);
        prefs.putUChar(key, takes[i].warning_time);

        // ---------- MED COUNT ----------
        sprintf(key, "mcount%d", i);
        prefs.putUChar(key, takes[i].medicine_count);

        // ---------- MEDICINES ----------
        for (int m = 0; m < takes[i].medicine_count; m++) {

            sprintf(key, "mid%d_%d", i, m);
            prefs.putString(key, takes[i].medicines[m].id);

            sprintf(key, "mqty%d_%d", i, m);
            prefs.putUChar(key, takes[i].medicines[m].quantity);
        }
    }

    prefs.end();

    Serial.println("Takes guardadas");
}

void uploadTakes()
{
    prefs.begin("takes", true);

    total_takes = prefs.getInt("total", 0);

    if (total_takes > MAX_TAKES)
        total_takes = MAX_TAKES;

    for (int i = 0; i < total_takes; i++) {

        char key[32];

        // ---------- ID ----------
        sprintf(key, "id%d", i);

        String id = prefs.getString(key, "");

        strncpy(
            takes[i].id,
            id.c_str(),
            sizeof(takes[i].id)
        );

        // ---------- HOUR ----------
        sprintf(key, "hour%d", i);

        String hour = prefs.getString(key, "00:00");

        strncpy(
            takes[i].hour,
            hour.c_str(),
            sizeof(takes[i].hour)
        );

        // ---------- REPEAT ----------
        sprintf(key, "rep%d", i);

        takes[i].repeat_mask =
            prefs.getUChar(key, 0x7F);

        // ---------- REMINDER ----------
        sprintf(key, "rec%d", i);

        takes[i].recordatory =
            prefs.getBool(key, false);

        // ---------- WARNING ----------
        sprintf(key, "warn%d", i);

        takes[i].warning_time =
            prefs.getUChar(key, 0);

        // ---------- MED COUNT ----------
        sprintf(key, "mcount%d", i);

        takes[i].medicine_count =
            prefs.getUChar(key, 0);

        if (takes[i].medicine_count > MAX_MEDICINES_PER_TAKE)
            takes[i].medicine_count = MAX_MEDICINES_PER_TAKE;

        // ---------- MEDICINES ----------
        for (int m = 0; m < takes[i].medicine_count; m++) {

            sprintf(key, "mid%d_%d", i, m);

            String medId = prefs.getString(key, "");

            strncpy(
                takes[i].medicines[m].id,
                medId.c_str(),
                sizeof(takes[i].medicines[m].id)
            );

            takes[i].medicines[m]
                .id[
                    sizeof(takes[i].medicines[m].id) - 1
                ] = '\0';

            sprintf(key, "mqty%d_%d", i, m);

            takes[i].medicines[m].quantity =
                prefs.getUChar(key, 1);
        }

        Serial.println("------ TAKE LOADED ------");

        Serial.print("ID: ");
        Serial.println(takes[i].id);

        Serial.print("Hour: ");
        Serial.println(takes[i].hour);

        Serial.print("Medicine count: ");
        Serial.println(takes[i].medicine_count);

        for (int m = 0; m < takes[i].medicine_count; m++) {

            Serial.print("Medicine ID: ");
            Serial.println(takes[i].medicines[m].id);

            Serial.print("Quantity: ");
            Serial.println(takes[i].medicines[m].quantity);
        }
    }

    prefs.end();

    Serial.println("Takes cargadas");
}

void delete_take(int index)
{
    if (index < 0 || index >= total_takes)
        return;

    for (int i = index; i < total_takes - 1; i++)
    {
        takes[i] = takes[i + 1];
    }

    total_takes--;

    saveTakes();
}