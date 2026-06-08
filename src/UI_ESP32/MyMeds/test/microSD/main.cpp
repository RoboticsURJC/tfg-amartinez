#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

void setup()
{
    Serial.begin(115200);
    delay(2000);

    Serial.println("Probando SD...");

    if (!SD.begin(5))
    {
        Serial.println("SD ERROR");
        return;
    }

    Serial.println("SD OK");

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);

    Serial.print("Capacidad: ");
    Serial.print(cardSize);
    Serial.println(" MB");

    File file = SD.open("/test.txt", FILE_WRITE);

    if (!file)
    {
        Serial.println("No se pudo crear archivo");
        return;
    }

    file.println("Hola SD");
    file.close();

    Serial.println("Archivo creado correctamente");
}

void loop()
{
}