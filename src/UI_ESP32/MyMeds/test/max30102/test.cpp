#include <Arduino.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 particleSensor;

const byte RATE_SIZE = 5; // promedio de 5 lecturas
byte rates[RATE_SIZE];
byte rateSpot = 0;

long lastBeat = 0;
float beatsPerMinute;
float beatAvg;

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("Inicializando MAX30102...");

    Wire.begin(22, 27); // tus pines

    if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD)) {
        Serial.println("❌ Sensor no detectado");
        while (1);
    }

    Serial.println("✅ Sensor detectado");

    // 🔥 CONFIGURACIÓN AJUSTADA (IMPORTANTE)
    particleSensor.setup(
        0x3F,   // brillo (ajústalo si satura o se queda corto)
        6,      // promedio
        2,      // RED + IR
        100,    // sample rate
        411,    // pulse width
        4096    // ADC range
    );

    Serial.println("Coloca el dedo...");
}

void loop()
{
    long irValue = particleSensor.getIR();

    // 🔍 DEBUG SEÑAL
    // Serial.print("IR: ");
    // Serial.println(irValue);

    // detectar dedo
    if (irValue < 30000) {
        Serial.println("❌ No hay dedo");
        delay(200);
        return;
    }

    // ❤️ DETECCIÓN DE LATIDO
    if (checkForBeat(irValue) == true)
    {
        long delta = millis() - lastBeat;
        lastBeat = millis();

        beatsPerMinute = 60 / (delta / 1000.0);

        // filtro valores irreales
        if (beatsPerMinute > 40 && beatsPerMinute < 150)
        {
            rates[rateSpot++] = (byte)beatsPerMinute;
            rateSpot %= RATE_SIZE;

            // calcular promedio
            beatAvg = 0;
            for (byte x = 0; x < RATE_SIZE; x++)
                beatAvg += rates[x];

            beatAvg /= RATE_SIZE;

            Serial.print("❤️ BPM: ");
            Serial.print(beatAvg);
            Serial.println();
        }
    }

    delay(20);
}