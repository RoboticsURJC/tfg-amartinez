#include "storage/pulse_history_storage.h"
#include "lvgl_touch.h"


#include <Arduino.h>
#include <SD.h>
#include <SPI.h>


static const int SD_SCK = 18;
static const int SD_MISO = 19;
static const int SD_MOSI = 23;
static const int SD_CS = 5;
static const char *PULSE_HISTORY_FILE = "/pulse_history.csv";

static bool sd_initialized = false;
static bool sd_available = false;

static SPIClass sdSPI(HSPI);

bool pulseHistoryBegin()
{
    if (sd_initialized) {
        return sd_available;
    }

    sd_initialized = true;
    sdSPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
    sd_available = SD.begin(SD_CS, sdSPI);

    if (!sd_available) {
        Serial.println("No se pudo inicializar la microSD");
        return false;
    }

    Serial.println("microSD inicializada");
    return true;
}

int loadPulseHistory(PulseRecord records[], int maxRecords)
{
    if (!pulseHistoryBegin()) {
        return 0;
    }

    if (!SD.exists(PULSE_HISTORY_FILE)) {
        return 0;
    }

    File file = SD.open(PULSE_HISTORY_FILE, FILE_READ);

    if (!file) {
        return 0;
    }

    int count = 0;
    bool firstLine = true;

    while (file.available() && count < maxRecords) {
        String line = file.readStringUntil('\n');

        if (firstLine) {
            firstLine = false;
            continue;
        }

        line.trim();

        if (line.length() == 0) {
            continue;
        }

        int firstSep = line.indexOf(';');
        int secondSep = line.indexOf(';', firstSep + 1);

        if (firstSep < 0 || secondSep < 0) {
            continue;
        }

        records[count].date = line.substring(0, firstSep);
        records[count].time = line.substring(firstSep + 1, secondSep);
        records[count].bpm = line.substring(secondSep + 1).toInt();
        count++;
    }

    Serial.println("=== HISTORIAL LEIDO ===");

    for (int i = 0; i < count; i++)
    {
        Serial.print(i);
        Serial.print(": ");
        Serial.print(records[i].date);
        Serial.print(" ");
        Serial.print(records[i].time);
        Serial.print(" ");
        Serial.println(records[i].bpm);
    }

    Serial.println("======================");

    file.close();
    return count;
}

bool savePulseAverage(int bpm)
{
    Serial.println("Entrando en savePulseAverage");

    if (bpm <= 0) {
        Serial.println("BPM invalido");
        return false;
    }

    if (!pulseHistoryBegin()) {
        Serial.println("pulseHistoryBegin fallo");
        return false;
    }

    Serial.println("SD inicializada");

    if (!SD.exists(PULSE_HISTORY_FILE)) {
        File headerFile = SD.open(PULSE_HISTORY_FILE, FILE_WRITE);

        if (headerFile) {
            headerFile.println("Fecha,Hora,BPM");
            headerFile.close();
        }
    }

    File file = SD.open(PULSE_HISTORY_FILE, FILE_APPEND);

    if (!file) {
        Serial.println("No se pudo abrir el fichero");
        return false;
    }

    Serial.println("Fichero abierto");
    struct tm timeinfo;

    if (!getLocalTime(&timeinfo)) {
        file.printf("SIN_FECHA;SIN_HORA;%d\n", bpm);
    } else {
        file.printf(
            "%04d-%02d-%02d;%02d:%02d:%02d;%d\n",
            timeinfo.tm_year + 1900,
            timeinfo.tm_mon + 1,
            timeinfo.tm_mday,
            timeinfo.tm_hour,
            timeinfo.tm_min,
            timeinfo.tm_sec,
            bpm
        );
    }

    Serial.println("Dato escrito");

    file.close();
    Serial.println("Reinicializando touch");

    SPI.end();

    delay(50);

    SPI.begin(25,39,32);

    return true;
}
