#include "user_interface/wifi_portal.h"
#include "storage/takes_storage.h"
#include "storage/medicines_storage.h"
#include "user_interface/pill_takes.h"
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>

// ---------------- DAYS ----------------

#define DAY_MONDAY    0
#define DAY_TUESDAY   1
#define DAY_WEDNESDAY 2
#define DAY_THURSDAY  3
#define DAY_FRIDAY    4
#define DAY_SATURDAY  5
#define DAY_SUNDAY    6

extern WebServer server;
extern bool device_linked;

extern String DEVICE_TOKEN;

static String new_ssid = "";
static String new_password = "";

// ---------------- WIFI ----------------

bool wifi_credentials_exists()
{
    Preferences p;
    p.begin("wifi", true);

    String ssid = p.getString("ssid", "");
    String password = p.getString("password", "");

    p.end();

    return (ssid.length() > 0 && password.length() > 0);
}

void wifi_load_credentials(String &ssid, String &password)
{
    Preferences p;
    p.begin("wifi", true);
    ssid = p.getString("ssid", "");
    password = p.getString("password", "");
    p.end();
}

// ---------------- WEB ROOT ----------------

void handle_web_root()
{
    String page = R"(
        <html><body style="font-family:Arial;">
        <h2>Configuración WiFi - MyMeds</h2>
        <form action="/save" method="POST">
            <input name="ssid" placeholder="SSID"/><br><br>
            <input name="password" type="password" placeholder="Password"/><br><br>
            <input type="submit" value="Guardar"/>
        </form>
        </body></html>
    )";

    server.send(200, "text/html", page);
}

// ---------------- SAVE WIFI ----------------

void handle_save()
{
    new_ssid = server.arg("ssid");
    new_password = server.arg("password");

    Preferences p;
    p.begin("wifi", false);
    p.putString("ssid", new_ssid);
    p.putString("password", new_password);
    p.end();

    server.send(200, "text/html", "Guardado. Reiniciando...");

    delay(2000);
    server.stop();

    p.begin("sys", false);
    p.putBool("skip_logo", true);
    p.end();

    ESP.restart();
}

// ---------------- GET TAKES ----------------

void handle_get_takes()
{
    DynamicJsonDocument doc(2048);

    JsonArray arr = doc.createNestedArray("takes");

    for (int i = 0; i < total_takes; i++) {

        JsonObject t = arr.createNestedObject();

        t["id"] = takes[i].id;
        t["time"] = takes[i].hour;

        // ---------------- DAYS ----------------

        JsonArray days = t.createNestedArray("days");

        if (takes[i].repeat_mask & (1 << 0)) days.add("MONDAY");
        if (takes[i].repeat_mask & (1 << 1)) days.add("TUESDAY");
        if (takes[i].repeat_mask & (1 << 2)) days.add("WEDNESDAY");
        if (takes[i].repeat_mask & (1 << 3)) days.add("THURSDAY");
        if (takes[i].repeat_mask & (1 << 4)) days.add("FRIDAY");
        if (takes[i].repeat_mask & (1 << 5)) days.add("SATURDAY");
        if (takes[i].repeat_mask & (1 << 6)) days.add("SUNDAY");

        // ---------------- MEDICINES ----------------

        JsonArray meds = t.createNestedArray("medicines");

        Serial.println("===== ENVIANDO TAKE =====");

        Serial.print("Take: ");
        Serial.println(i);

        Serial.print("Medicine count: ");
        Serial.println(takes[i].medicine_count);

        for (int j = 0; j < takes[i].medicine_count; j++) {

            JsonObject med = meds.createNestedObject();

            med["id"] =
                takes[i].medicines[j].id;

            const char* medName = "Desconocido";

            for (int k = 0; k < medicine_count; k++) {

                if (strcmp(
                    medicineCatalog[k].id,
                    takes[i].medicines[j].id
                ) == 0) {

                    medName =
                        medicineCatalog[k].name;

                    break;
                }
            }

            med["name"] = medName;

            med["quantity"] =
                takes[i].medicines[j].quantity;

            Serial.print("  ID: ");
            Serial.println(
                takes[i].medicines[j].id
            );

            Serial.print("  Name: ");
            Serial.println(medName);

            Serial.print("  Quantity: ");
            Serial.println(
                takes[i].medicines[j].quantity
            );
        }

        // ---------------- REMINDER ----------------

        t["reminderEnabled"] =
            takes[i].recordatory;

        t["advanceWarningMinutes"] =
            takes[i].warning_time;
    }

    String response;
    serializeJson(doc, response);

    Serial.println("===== JSON ENVIADO =====");
    Serial.println(response);

    server.send(200, "application/json", response);
}
// ---------------- ADD TAKE ----------------

void handle_add_take()
{
    String body = server.arg("plain");

    DynamicJsonDocument doc(512);
    if (deserializeJson(doc, body)) {
        server.send(400, "application/json", "{\"error\":\"invalid json\"}");
        return;
    }

    if (total_takes >= MAX_TAKES) {
        server.send(400, "application/json", "{\"error\":\"max takes\"}");
        return;
    }

    const char* id = doc["id"] | "";

    for (int i = 0; i < total_takes; i++) {
        if (strcmp(takes[i].id, id) == 0) {
            server.send(409, "application/json", "{\"error\":\"duplicate\"}");
            return;
        }
    }

    TakeConfig &t = takes[total_takes];

    strncpy(t.id, id, sizeof(t.id));
    t.id[sizeof(t.id)-1] = '\0';

    String time = doc["time"] | "00:00";
    strncpy(t.hour, time.c_str(), sizeof(t.hour));
    t.hour[sizeof(t.hour)-1] = '\0';

    t.repeat_mask = 0;

    for (JsonVariant v : doc["days"].as<JsonArray>()) {
        String d = v.as<String>();
        if (d == "MONDAY")    t.repeat_mask |= (1 << 0);
        if (d == "TUESDAY")   t.repeat_mask |= (1 << 1);
        if (d == "WEDNESDAY") t.repeat_mask |= (1 << 2);
        if (d == "THURSDAY")  t.repeat_mask |= (1 << 3);
        if (d == "FRIDAY")    t.repeat_mask |= (1 << 4);
        if (d == "SATURDAY")  t.repeat_mask |= (1 << 5);
        if (d == "SUNDAY")    t.repeat_mask |= (1 << 6);
    }

    t.recordatory = doc["reminderEnabled"] | false;
    t.warning_time = doc["advanceWarningMinutes"] | 0;

    total_takes++;
    saveTakes();

    server.send(200, "application/json", "{\"ok\":true}");
}

// ---------------- UPDATE TAKE ----------------

void handle_update_take()
{
    String body = server.arg("plain");

    DynamicJsonDocument doc(512);
    if (deserializeJson(doc, body)) {
        server.send(400, "application/json", "{\"error\":\"invalid json\"}");
        return;
    }

    const char* id = doc["id"] | "";

    int index = -1;
    for (int i = 0; i < total_takes; i++) {
        if (strcmp(takes[i].id, id) == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        server.send(404, "application/json", "{\"error\":\"not found\"}");
        return;
    }

    TakeConfig &t = takes[index];

    if (doc["time"].is<String>()) {
        String time = doc["time"];
        strncpy(t.hour, time.c_str(), sizeof(t.hour));
        t.hour[sizeof(t.hour)-1] = '\0';
    }

    if (doc["days"].is<JsonArray>()) {

        t.repeat_mask = 0;

        JsonArray days = doc["days"].as<JsonArray>();

        for (int i = 0; i < days.size(); i++) {

            String d = days[i].as<String>();

            if (d == "MONDAY")    t.repeat_mask |= (1 << 0);
            if (d == "TUESDAY")   t.repeat_mask |= (1 << 1);
            if (d == "WEDNESDAY") t.repeat_mask |= (1 << 2);
            if (d == "THURSDAY")  t.repeat_mask |= (1 << 3);
            if (d == "FRIDAY")    t.repeat_mask |= (1 << 4);
            if (d == "SATURDAY")  t.repeat_mask |= (1 << 5);
            if (d == "SUNDAY")    t.repeat_mask |= (1 << 6);
        }
    }

    if (doc["reminderEnabled"].is<bool>())
        t.recordatory = doc["reminderEnabled"];

    if (doc["advanceWarningMinutes"].is<int>())
        t.warning_time = doc["advanceWarningMinutes"];

    saveTakes();
    server.send(200, "application/json", "{\"updated\":true}");
}

// ---------------- DELETE TAKE ----------------

void handle_delete_take()
{
    String id = server.arg("id");

    int index = -1;

    for (int i = 0; i < total_takes; i++) {
        if (strcmp(takes[i].id, id.c_str()) == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        server.send(404, "application/json", "{\"error\":\"not found\"}");
        return;
    }

    for (int i = index; i < total_takes - 1; i++) {
        takes[i] = takes[i + 1];
    }

    total_takes--;
    saveTakes();

    server.send(200, "application/json", "{\"deleted\":true}");
}

// ---------------- MEDICINES ----------------

void handle_set_medicines()
{
    DynamicJsonDocument doc(512);
    deserializeJson(doc, server.arg("plain"));

    medicine_count = 0;

    for (JsonObject obj : doc["medicines"].as<JsonArray>()) {

        if (medicine_count >= MAX_CATALOG_MEDICINES) break;

        strncpy(medicineCatalog[medicine_count].id, obj["id"] | "", sizeof(medicineCatalog[0].id));
        strncpy(medicineCatalog[medicine_count].name, obj["name"] | "", sizeof(medicineCatalog[0].name));

        medicine_count++;
    }

    saveMedicines();
    server.send(200, "text/plain", "OK");
}

void handle_get_medicines()
{
    DynamicJsonDocument doc(512);
    JsonArray arr = doc.createNestedArray("medicines");

    for (int i = 0; i < medicine_count; i++) {
        JsonObject obj = arr.createNestedObject();
        obj["id"] = medicineCatalog[i].id;
        obj["name"] = medicineCatalog[i].name;
    }

    String json;
    serializeJson(doc, json);

    Serial.println("===== MEDICINES ENVIADOS =====");
    Serial.println(json);

    server.send(200, "application/json", json);
}

void handle_link()
{
    Serial.println("App sincronizada (link)");

    device_linked = true;

    Preferences p;

    p.begin("sys", false);
    p.putBool("linked", true);
    p.end();

    DynamicJsonDocument doc(128);

    doc["status"] = "linked";
    doc["token"] = DEVICE_TOKEN;

    String response;

    serializeJson(doc, response);

    server.send(
        200,
        "application/json",
        response
    );
}

void handle_takes()
{
    String body = server.arg("plain");

    Serial.println("Takes received:");
    Serial.println(body);

    if (body.length() == 0) {
        server.send(400, "application/json", "{\"error\":\"empty body\"}");
        return;
    }

    DynamicJsonDocument doc(2048);

    if (deserializeJson(doc, body)) {
        server.send(400, "application/json", "{\"error\":\"invalid json\"}");
        return;
    }

    JsonArray takesJson = doc["takes"];

    total_takes = 0;

    for (JsonObject t : takesJson) {

        if (total_takes >= MAX_TAKES) break;

        // ID
        const char* id = t["id"] | "";
        strncpy(takes[total_takes].id, id, sizeof(takes[0].id));
        takes[total_takes].id[sizeof(takes[0].id)-1] = '\0';

        // TIME
        String time = t["time"] | "00:00";
        strncpy(takes[total_takes].hour, time.c_str(), sizeof(takes[0].hour));
        takes[total_takes].hour[sizeof(takes[0].hour)-1] = '\0';

        // DAYS
        takes[total_takes].repeat_mask = 0;

        JsonArray days = t["days"].as<JsonArray>();

        for (int i = 0; i < days.size(); i++) {

            String d = days[i].as<String>();

            if (d == "MONDAY")    takes[total_takes].repeat_mask |= (1 << 0);
            if (d == "TUESDAY")   takes[total_takes].repeat_mask |= (1 << 1);
            if (d == "WEDNESDAY") takes[total_takes].repeat_mask |= (1 << 2);
            if (d == "THURSDAY")  takes[total_takes].repeat_mask |= (1 << 3);
            if (d == "FRIDAY")    takes[total_takes].repeat_mask |= (1 << 4);
            if (d == "SATURDAY")  takes[total_takes].repeat_mask |= (1 << 5);
            if (d == "SUNDAY")    takes[total_takes].repeat_mask |= (1 << 6);
        }

        takes[total_takes].medicine_count = 0;

        JsonArray meds = t["medicines"].as<JsonArray>();

        for (JsonObject med : meds) {

            if (takes[total_takes].medicine_count >= MAX_MEDICINES_PER_TAKE)
                break;

            int idx = takes[total_takes].medicine_count;

            // ID
            strncpy(
                takes[total_takes].medicines[idx].id,
                med["id"] | "",
                sizeof(takes[total_takes].medicines[idx].id)
            );

            takes[total_takes]
                .medicines[idx]
                .id[
                    sizeof(
                        takes[total_takes]
                            .medicines[idx]
                            .id
                    ) - 1
                ] = '\0';

            takes[total_takes].medicines[idx].quantity = med["quantity"] | 1;

            takes[total_takes].medicine_count++;

            // DEBUG
            Serial.println("---- MEDICAMENTO RECIBIDO ----");

            Serial.print("Take: ");
            Serial.println(total_takes);

            Serial.print("Medicine ID: ");
            Serial.println(
                takes[total_takes]
                    .medicines[idx]
                    .id
            );

            Serial.print("Quantity: ");
            Serial.println(
                takes[total_takes]
                    .medicines[idx]
                    .quantity
            );

            Serial.println("------------------------------");
        }

        // REMINDER
        takes[total_takes].recordatory = t["reminderEnabled"] | false;

        // WARNING
        takes[total_takes].warning_time = t["advanceWarningMinutes"] | 0;

        total_takes++;
    }

    Serial.println("===== TAKES EN RAM =====");

    for (int i = 0; i < total_takes; i++) {

        Serial.print("Take ");
        Serial.println(i);

        Serial.print("Hour: ");
        Serial.println(takes[i].hour);

        Serial.print("Medicine count: ");
        Serial.println(takes[i].medicine_count);

        for (int j = 0; j < takes[i].medicine_count; j++) {

            Serial.print("  ID: ");
            Serial.println(takes[i].medicines[j].id);

            Serial.print("  Quantity: ");
            Serial.println(takes[i].medicines[j].quantity);
        }

        Serial.println("----------------");
    }

    saveTakes();

    Serial.println("Takes saved correctly");

    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

// ---------------- INIT ----------------

void wifi_portal_init()
{
    WiFi.softAP("MyMeds-Setup");

    server.on("/", handle_web_root);
    server.on("/save", HTTP_POST, handle_save);

    server.on("/takes", HTTP_GET, handle_get_takes);
    server.on("/take", HTTP_POST, handle_add_take);
    server.on("/take", HTTP_PUT, handle_update_take);
    server.on("/take", HTTP_DELETE, handle_delete_take);

    server.on("/medicines", HTTP_POST, handle_set_medicines);
    server.on("/medicines", HTTP_GET, handle_get_medicines);

    server.begin();
}