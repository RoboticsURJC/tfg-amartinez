#include "storage/medicines_storage.h"
#include <Preferences.h>
#include <ArduinoJson.h>

CatalogMedicine medicineCatalog[MAX_CATALOG_MEDICINES];
int medicine_count = 0;

Preferences prefs;

// ---------------- SAVE ----------------

void saveMedicines() {

    prefs.begin("meds", false);

    DynamicJsonDocument doc(512);
    JsonArray arr = doc.createNestedArray("medicines");

    for (int i = 0; i < medicine_count; i++) {

        JsonObject obj = arr.createNestedObject();

        obj["id"] = medicineCatalog[i].id;
        obj["name"] = medicineCatalog[i].name;
    }

    String json;
    serializeJson(doc, json);

    prefs.putString("catalog", json);
    prefs.end();

    Serial.println("Medicines guardados en ESP");
}

// ---------------- LOAD ----------------

void loadMedicines() {

    prefs.begin("meds", true);

    String json = prefs.getString("catalog", "");
    prefs.end();

    if (json.isEmpty()) return;

    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, json);

    if (error) {
        Serial.println("Error parsing medicines JSON");
        return;
    }

    medicine_count = 0;

    JsonArray arr = doc["medicines"].as<JsonArray>();

    for (JsonObject obj : arr) {

        if (medicine_count >= MAX_CATALOG_MEDICINES) break;

        strncpy(
            medicineCatalog[medicine_count].id,
            obj["id"] | "",
            sizeof(medicineCatalog[medicine_count].id)
        );

        strncpy(
            medicineCatalog[medicine_count].name,
            obj["name"] | "",
            sizeof(medicineCatalog[medicine_count].name)
        );

        medicine_count++;
    }

    Serial.println("Medicines cargados:");
    for (int i = 0; i < medicine_count; i++) {
        Serial.println(medicineCatalog[i].name);
    }
}