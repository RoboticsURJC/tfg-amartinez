#include "user_interface/wifi_portal.h"
#include "storage/takes_storage.h"
#include "user_interface/pill_takes.h"
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>

Preferences prefs;
extern WebServer server;
extern bool device_linked;

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
            <label>NOMBRE_WIFI:</label><br>
            <input name="ssid" /><br><br>
            <label>CONTRASEÑA:</label><br>
            <input name="password" type="password"/><br><br>
            <input type="submit" value="Guardar"/>
        </form>
        </body><html>
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

    server.send(200, "text/html",
        "<html><body><h2>Guardado correctamente. Reiniciando...</h2></body></html>");

    delay(2000);   
    server.stop();
    delay(200);

    p.begin("sys", false);
    p.putBool("skip_logo", true);
    p.end();

    ESP.restart();
}

// ---------------- GET TAKES ----------------

void handle_get_takes()
{
    DynamicJsonDocument doc(4096);

    JsonArray arr = doc.createNestedArray("takes");

    for (int i = 0; i < total_takes; i++) {

        JsonObject t = arr.createNestedObject();

        t["id"] = takes[i].id;
        t["time"] = takes[i].hour;

        JsonArray days = t.createNestedArray("days");

        if (takes[i].repeat[0]) days.add("MONDAY");
        if (takes[i].repeat[1]) days.add("TUESDAY");
        if (takes[i].repeat[2]) days.add("WEDNESDAY");
        if (takes[i].repeat[3]) days.add("THURSDAY");
        if (takes[i].repeat[4]) days.add("FRIDAY");
        if (takes[i].repeat[5]) days.add("SATURDAY");
        if (takes[i].repeat[6]) days.add("SUNDAY");

        t["reminderEnabled"] = takes[i].recordatory;
        t["advanceWarningMinutes"] = takes[i].warning_time;
    }

    String response;
    serializeJson(doc, response);

    Serial.println("Sending takes:");
    Serial.println(response);

    server.send(200, "application/json", response);
}

// ---------------- ADD TAKE ----------------

void handle_add_take()
{
    String body = server.arg("plain");

    if (body.length() == 0) {
        server.send(400, "application/json", "{\"error\":\"empty body\"}");
        return;
    }

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
        server.send(400, "application/json", "{\"error\":\"invalid json\"}");
        return;
    }

    if (total_takes >= MAX_TAKES) {
        server.send(400, "application/json", "{\"error\":\"max takes reached\"}");
        return;
    }

    const char* id = doc["id"] | "";

    if (strlen(id) == 0) {
        server.send(400, "application/json", "{\"error\":\"missing id\"}");
        return;
    }

    // evitar duplicados
    for (int i = 0; i < total_takes; i++) {
        if (strcmp(takes[i].id, id) == 0) {
            server.send(409, "application/json", "{\"error\":\"duplicate id\"}");
            return;
        }
    }

    Serial.println("ADD TAKE VALID:");
    Serial.println(body);

    TakeConfig &newTake = takes[total_takes];

    strncpy(newTake.id, id, sizeof(newTake.id));
    newTake.id[sizeof(newTake.id) - 1] = '\0';

    String time = doc["time"] | "00:00";
    strncpy(newTake.hour, time.c_str(), sizeof(newTake.hour));
    newTake.hour[sizeof(newTake.hour) - 1] = '\0';

    for (int i = 0; i < 7; i++) newTake.repeat[i] = false;

    JsonArray days = doc["days"];
    for (String d : days) {
        if (d == "MONDAY") newTake.repeat[0] = true;
        if (d == "TUESDAY") newTake.repeat[1] = true;
        if (d == "WEDNESDAY") newTake.repeat[2] = true;
        if (d == "THURSDAY") newTake.repeat[3] = true;
        if (d == "FRIDAY") newTake.repeat[4] = true;
        if (d == "SATURDAY") newTake.repeat[5] = true;
        if (d == "SUNDAY") newTake.repeat[6] = true;
    }

    newTake.recordatory = doc["reminderEnabled"] | false;
    newTake.warning_time = doc["advanceWarningMinutes"] | 0;

    total_takes++;

    saveTakes();

    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

// ---------------- UPDATE TAKE ----------------

void handle_update_take()
{
    String body = server.arg("plain");

    if (body.length() == 0) {
        server.send(400, "application/json", "{\"error\":\"empty body\"}");
        return;
    }

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
        server.send(400, "application/json", "{\"error\":\"invalid json\"}");
        return;
    }

    const char* id = doc["id"] | "";

    if (strlen(id) == 0) {
        server.send(400, "application/json", "{\"error\":\"missing id\"}");
        return;
    }

    // BUSCAR TAKE
    int index = -1;

    for (int i = 0; i < total_takes; i++) {
        if (strcmp(takes[i].id, id) == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        Serial.println("Update failed: ID not found");
        server.send(404, "application/json", "{\"error\":\"not found\"}");
        return;
    }

    // LOG SOLO SI EXISTE
    Serial.println("UPDATE TAKE:");
    Serial.println(body);

    TakeConfig &t = takes[index];

    // --- HORA ---
    if (doc.containsKey("time")) {
        String time = doc["time"];
        strncpy(t.hour, time.c_str(), sizeof(t.hour));
        t.hour[sizeof(t.hour) - 1] = '\0';
    }

    // --- DÍAS ---
    if (doc.containsKey("days")) {

        // reset
        for (int i = 0; i < 7; i++) t.repeat[i] = false;

        JsonArray days = doc["days"];
        for (String d : days) {
            if (d == "MONDAY") t.repeat[0] = true;
            if (d == "TUESDAY") t.repeat[1] = true;
            if (d == "WEDNESDAY") t.repeat[2] = true;
            if (d == "THURSDAY") t.repeat[3] = true;
            if (d == "FRIDAY") t.repeat[4] = true;
            if (d == "SATURDAY") t.repeat[5] = true;
            if (d == "SUNDAY") t.repeat[6] = true;
        }
    }

    // --- RECORDATORIO ---
    if (doc.containsKey("reminderEnabled")) {
        t.recordatory = doc["reminderEnabled"];
    }

    // --- WARNING ---
    if (doc.containsKey("advanceWarningMinutes")) {
        t.warning_time = doc["advanceWarningMinutes"];
    }

    saveTakes();

    server.send(200, "application/json", "{\"status\":\"updated\"}");
}

// ---------------- DELETE TAKE ----------------

void handle_delete_take()
{
    if (!server.hasArg("id")) {
        server.send(400, "application/json", "{\"error\":\"missing id\"}");
        return;
    }

    String idStr = server.arg("id");

    if (idStr.length() == 0) {
        server.send(400, "application/json", "{\"error\":\"empty id\"}");
        return;
    }

    Serial.print("DELETE TAKE: ");
    Serial.println(idStr);

    int index = -1;

    // 🔍 Buscar take
    for (int i = 0; i < total_takes; i++) {
        if (strcmp(takes[i].id, idStr.c_str()) == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        Serial.println("Delete failed: ID not found");
        server.send(404, "application/json", "{\"error\":\"not found\"}");
        return;
    }

    // 🔥 Compactar array (clave)
    for (int i = index; i < total_takes - 1; i++) {
        takes[i] = takes[i + 1];
    }

    total_takes--;

    saveTakes();

    Serial.println("Take eliminada correctamente");

    server.send(200, "application/json", "{\"status\":\"deleted\"}");
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

    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
        Serial.println("Error parsing JSON");
        server.send(400, "application/json", "{\"error\":\"invalid json\"}");
        return;
    }

    JsonArray takesJson = doc["takes"];

    total_takes = 0;

    for (JsonObject t : takesJson) {

        if (total_takes >= MAX_TAKES) break;

        String time = t["time"] | "00:00";
        strncpy(takes[total_takes].hour, time.c_str(), 6);

        for (int i = 0; i < 7; i++) {
            takes[total_takes].repeat[i] = false;
        }

        JsonArray days = t["days"];
        for (String d : days) {

            if (d == "MONDAY") takes[total_takes].repeat[0] = true;
            if (d == "TUESDAY") takes[total_takes].repeat[1] = true;
            if (d == "WEDNESDAY") takes[total_takes].repeat[2] = true;
            if (d == "THURSDAY") takes[total_takes].repeat[3] = true;
            if (d == "FRIDAY") takes[total_takes].repeat[4] = true;
            if (d == "SATURDAY") takes[total_takes].repeat[5] = true;
            if (d == "SUNDAY") takes[total_takes].repeat[6] = true;
        }

        takes[total_takes].recordatory = t["reminderEnabled"] | false;
        takes[total_takes].warning_time = t["advanceWarningMinutes"] | 0;

        // 🔥 ID (importante)
        if (strlen(takes[total_takes].id) == 0) {
            sprintf(takes[total_takes].id, "take_%d", total_takes);
        }

        total_takes++;
    }

    saveTakes();

    Serial.println("Takes saved correctly");

    server.send(200, "application/json", body);
}

// ---------------- LINK ----------------

void handle_link()
{
    Serial.println("App sincronizada (link)");

    device_linked = true;

    Preferences p;
    p.begin("sys", false);
    p.putBool("linked", true);  // GUARDAR ESTADO
    p.end();

    server.send(200, "application/json", "{\"status\":\"linked\"}");
}

// ---------------- INIT ----------------

void wifi_portal_init()
{
    IPAddress local_ip(192,168,4,1);
    IPAddress gateway(192,168,4,1);
    IPAddress subnet(255,255,255,0);

    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP("MyMeds-Setup");

    Serial.println(WiFi.softAPIP());

    server.on("/", handle_web_root);
    server.on("/save", HTTP_POST, handle_save);

    server.on("/takes", HTTP_GET, handle_get_takes);
    server.on("/take", HTTP_POST, handle_add_take);

    server.on("/link", HTTP_GET, handle_link);

    server.begin();

    Serial.println("Server started");
}