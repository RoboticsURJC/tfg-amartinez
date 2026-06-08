#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

#include "sensors/pulse_sensor.h"

MAX30105 particleSensor;

static bool initialized = false;
static bool sensorRunning = false;

static bool fingerDetected = false;
static bool measurementReady = false;
static bool fullMeasurementReady = false;

static int currentBpm = 0;
static int fullBufferBpm = 0;

const byte RATE_SIZE = 10;
const byte MIN_VALID_SAMPLES_TO_SHOW = 5;

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

            // Esperar al menos 5 valores validos antes de mostrar pulso.
            // El buffer sigue creciendo hasta 10 para estabilizar la media.
            if (
                validSamples <
                MIN_VALID_SAMPLES_TO_SHOW
            )
            {
                return;
            }

            beatAvg = 0;

            for (
                byte i = 0;
                i < validSamples;
                i++
            )
            {
                beatAvg += rates[i];
            }

            beatAvg /= validSamples;

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

            if (validSamples >= RATE_SIZE)
            {
                fullBufferBpm = currentBpm;
                fullMeasurementReady = true;
            }
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

bool pulseFullMeasurementReady()
{
    return fullMeasurementReady;
}

int pulseGetFullBufferBpm()
{
    return fullBufferBpm;
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
    currentBpm = 0;
    fullBufferBpm = 0;
    fullMeasurementReady = false;

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
