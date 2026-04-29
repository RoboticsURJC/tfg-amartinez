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

void handle_save()
{
    new_ssid = server.arg("ssid");
    new_password = server.arg("password");

    Serial.print("Saved SSID: ");
    Serial.println(new_ssid);

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

void handle_get_takes()
{
    Preferences p;
    p.begin("takes", true);

    String data = p.getString("data", "{\"takes\":[]}");

    p.end();

    Serial.println("Sending stored takes:");
    Serial.println(data);

    server.send(200, "application/json", data);
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

        total_takes++;
    }

    saveTakes();

    Preferences p;
    p.begin("takes", false);
    p.putString("data", body);
    p.end();

    Serial.println("Takes saved correctly");

    server.send(200, "application/json", body);

    device_linked = true;
}

void handle_link()
{
    Serial.println("App sincronizada (link)");

    device_linked = true;

    server.send(200, "application/json", "{\"status\":\"linked\"}");
}

void wifi_portal_init()
{
    IPAddress local_ip(192,168,4,1);
    IPAddress gateway(192,168,4,1);
    IPAddress subnet(255,255,255,0);

    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP("MyMeds-Setup");

    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());

    server.on("/", handle_web_root);
    server.on("/save", HTTP_POST, handle_save);
    server.on("/takes", HTTP_GET, handle_get_takes);
    server.on("/takes", HTTP_POST, handle_takes);
    server.on("/link", HTTP_GET, handle_link);
    server.begin();

    Serial.println((uint32_t)&server);
    Serial.println("Server started");
}