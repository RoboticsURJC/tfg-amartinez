#include "user_interface/wifi_portal.h"
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

Preferences prefs;
WebServer server(80);
static String new_ssid = "";
static String new_password = "";

bool is_first_boot()
{
    Preferences p;
    p.begin("sys", true);
    bool fb = p.getBool("first_boot", true);
    p.end();
    
    return fb;
}

void set_first_boot(bool v)
{
    Preferences p;
    p.begin("sys", false);
    bool fb = p.getBool("fisrt_bool", true);
    p.end();
}

bool wifi_credentials_exists()
{
    Preferences p;
    p.begin("wifi", true);
    bool ok = p.isKey("ssid") && p.isKey("password");
    p.end();

    return ok;
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

    Preferences p;
    p.begin("wifi", false);
    p.putString("ssid", new_ssid);
    p.putString("password", new_password);
    p.end();

    set_first_boot(false);
    server.send(200, "text/html", "<html><body><h2>Guardado. Reiniciando...</h2></body></html>");

    delay(1000);
    ESP.restart();
}

void wifi_portal_init()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP("MyMeds-Setup");

    server.on("/", handle_web_root);
    server.on("/save", HTTP_POST, handle_save);
    server.begin();
}