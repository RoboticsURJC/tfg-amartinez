#include "user_interface/wifi_portal.h"
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

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

void handle_takes()
{
    String body = server.arg("plain");  // JSON recibido

    Serial.println("Takes received:");
    Serial.println(body);

    // Guardar en memoria (temporal por ahora)
    Preferences p;
    p.begin("takes", false);
    p.putString("data", body);
    p.end();

    server.send(200, "application/json", "{\"status\":\"ok\"}");
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
    server.on("/takes", HTTP_POST, handle_takes);
    server.on("/link", HTTP_GET, handle_link);
    server.begin();

    Serial.println((uint32_t)&server);
    Serial.println("Server started");
}