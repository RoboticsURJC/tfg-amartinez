#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

#include "sensors/pulse_sensor.h"

MAX30105 particleSensor;

static bool initialized = false;
static bool sensorRunning = false;

static bool fingerDetected = false;
static bool measurementReady = false;

static int currentBpm = 0;

const byte RATE_SIZE = 10;

byte rates[RATE_SIZE];
byte rateSpot = 0;
byte validSamples = 0;

long lastBeat = 0;

float beatsPerMinute = 0;
float beatAvg = 0;

float filteredBpm = 0;
bool firstMeasure = true;

void pulseSensorInit()
{
    Serial.println("Inicializando MAX30102...");

    Wire.begin(22, 27);

    if (!particleSensor.begin(
            Wire,
            I2C_SPEED_STANDARD))
    {
        Serial.println(
            "❌ Sensor no detectado"
        );

        initialized = false;

        return;
    }

    particleSensor.setup(
        0x3F,
        6,
        2,
        100,
        411,
        4096
    );

    particleSensor.shutDown();

    initialized = true;

    Serial.println(
        "✅ Sensor detectado"
    );
}

void pulseSensorUpdate()
{
    if (!initialized)
        return;

    if (!sensorRunning)
        return;

    long irValue =
        particleSensor.getIR();

    // ----------------------
    // DEDO AUSENTE
    // ----------------------

    if (irValue < 30000)
    {
        fingerDetected = false;

        measurementReady = false;

        validSamples = 0;
        rateSpot = 0;

        firstMeasure = true;

        filteredBpm = 0;

        return;
    }

    // ----------------------
    // DEDO PRESENTE
    // ----------------------

    fingerDetected = true;

    // ----------------------
    // LATIDO
    // ----------------------

    if (checkForBeat(irValue))
    {
        long delta =
            millis() - lastBeat;

        lastBeat = millis();

        beatsPerMinute =
            60.0f /
            (delta / 1000.0f);

        if (
            beatsPerMinute > 40 &&
            beatsPerMinute < 150
        )
        {
            rates[rateSpot++] =
                (byte)beatsPerMinute;

            if (
                validSamples <
                RATE_SIZE
            )
            {
                validSamples++;
            }

            rateSpot %= RATE_SIZE;

            // Esperar buffer lleno
            if (
                validSamples <
                RATE_SIZE
            )
            {
                return;
            }

            beatAvg = 0;

            for (
                byte i = 0;
                i < RATE_SIZE;
                i++
            )
            {
                beatAvg += rates[i];
            }

            beatAvg /= RATE_SIZE;

            if (firstMeasure)
            {
                filteredBpm =
                    beatAvg;

                firstMeasure = false;
            }
            else
            {
                filteredBpm =
                    0.9f *
                    filteredBpm
                    +
                    0.1f *
                    beatAvg;
            }

            currentBpm =
                (int)
                round(
                    filteredBpm
                );

            measurementReady = true;
        }
    }
}

bool pulseFingerDetected()
{
    return fingerDetected;
}

bool pulseMeasurementReady()
{
    return measurementReady;
}

int pulseGetBpm()
{
    return currentBpm;
}

int pulseGetSamples()
{
    return validSamples;
}

int pulseGetRequiredSamples()
{
    return RATE_SIZE;
}

void pulseSensorStart()
{
    if (!initialized)
        return;

    particleSensor.wakeUp();

    sensorRunning = true;

    validSamples = 0;
    rateSpot = 0;

    firstMeasure = true;

    filteredBpm = 0;

    measurementReady = false;

    fingerDetected = false;

    lastBeat = 0;
}

void pulseSensorStop()
{
    if (!initialized)
        return;

    particleSensor.shutDown();

    sensorRunning = false;
}