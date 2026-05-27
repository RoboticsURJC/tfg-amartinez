#include <Arduino.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 particleSensor;

// ==============================
// CONFIGURACIÓN
// ==============================

const byte RATE_SIZE = 10;

byte rates[RATE_SIZE];
byte rateSpot = 0;
byte validSamples = 0;

long lastBeat = 0;

float beatsPerMinute = 0;
float beatAvg = 0;

float filteredBpm = 0;
bool firstMeasure = true;

bool fingerDetected = false;

// ==============================
// SETUP
// ==============================

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("Inicializando MAX30102...");

    Wire.begin(22, 27);

    if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD))
    {
        Serial.println("❌ Sensor no detectado");

        while (1);
    }

    Serial.println("✅ Sensor detectado");
    Serial.println("Coloca el dedo...");

    particleSensor.setup(
        0x3F,   // brillo LED
        6,      // promedio interno
        2,      // RED + IR
        100,    // sample rate
        411,    // pulse width
        4096    // ADC range
    );
}

// ==============================
// LOOP
// ==============================

void loop()
{
    long irValue = particleSensor.getIR();

    // Detección de dedo
    if (irValue < 30000)
    {
        if (fingerDetected)
        {
            Serial.println("❌ No hay dedo");
        }

        fingerDetected = false;

        validSamples = 0;
        rateSpot = 0;

        firstMeasure = true;
        filteredBpm = 0;

        delay(100);
        return;
    }

    if (!fingerDetected)
    {
        Serial.println("✅ Dedo detectado");

        fingerDetected = true;
    }

    // Detección de latido
    if (checkForBeat(irValue))
    {
        long delta = millis() - lastBeat;
        lastBeat = millis();

        beatsPerMinute =
            60.0 / (delta / 1000.0);

        // Filtrar lecturas imposibles
        if (beatsPerMinute > 40 &&
            beatsPerMinute < 150)
        {
            rates[rateSpot++] =
                (byte)beatsPerMinute;

            if (validSamples < RATE_SIZE)
            {
                validSamples++;
            }

            rateSpot %= RATE_SIZE;

            // Esperar a llenar buffer
            if (validSamples < RATE_SIZE)
            {
                return;
            }

            // Calcular promedio
            beatAvg = 0;

            for (byte i = 0;
                 i < RATE_SIZE;
                 i++)
            {
                beatAvg += rates[i];
            }

            beatAvg /= RATE_SIZE;

            // Filtro exponencial
            if (firstMeasure)
            {
                filteredBpm = beatAvg;
                firstMeasure = false;
            }
            else
            {
                filteredBpm =
                    0.9f * filteredBpm +
                    0.1f * beatAvg;
            }

            Serial.print("❤️ BPM: ");
            Serial.println(filteredBpm, 0);
        }
    }

    delay(20);
}

// #include <Arduino.h>
// #include <Wire.h>

// void setup()
// {
//     Serial.begin(115200);
//     delay(1000);

//     Serial.println("Escaneando bus I2C...");

//     Wire.begin(22, 27);

//     byte count = 0;

//     for (byte address = 1; address < 127; address++)
//     {
//         Wire.beginTransmission(address);

//         byte error = Wire.endTransmission();

//         if (error == 0)
//         {
//             Serial.print("Dispositivo encontrado en 0x");

//             if (address < 16)
//                 Serial.print("0");

//             Serial.println(address, HEX);

//             count++;
//         }
//     }

//     if (count == 0)
//     {
//         Serial.println("No se encontraron dispositivos I2C");
//     }
//     else
//     {
//         Serial.println("Escaneo terminado");
//     }
// }

// void loop()
// {
// }