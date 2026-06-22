#include <Arduino.h>
#include "actuators/dose_actuators.h"
#include "logic/takes_logic.h"
#include "user_interface/take_alert_screen.h"
#include "user_interface/pill_takes.h"

static long lastTriggeredMinute[MAX_TAKES] = {
    -1, -1, -1, -1, -1, -1, -1
};

static unsigned long lastCheck = 0;

void executeTake(int index)
{
    Serial.printf("Ejecutando toma %d\n", index+1);
    show_take_alert(index);
    doseActuatorsStart();
}

void checkTakes()
{
    if (millis() - lastCheck < 500)
    {
        return;
    }

    lastCheck = millis();

    if (doseActuatorsBusy() || takeAlertVisible())
    {
        return;
    }

    struct tm timeInfo;
    if (!getLocalTime(&timeInfo)){
        return;
    }

    int currentDay = (timeInfo.tm_wday + 6) % 7;
    long minuteKey =
        ((long)timeInfo.tm_year * 366L + timeInfo.tm_yday) * 1440L +
        timeInfo.tm_hour * 60L +
        timeInfo.tm_min;

    for (int i = 0; i < total_takes; i ++){
        int hh = atoi(&takes[i].hour[0]);
        int mm = atoi(&takes[i].hour[3]);

        if (timeInfo.tm_hour == hh && timeInfo.tm_min == mm){
            if (
                is_day_active(takes[i].repeat_mask, currentDay) &&
                lastTriggeredMinute[i] != minuteKey
            ){
                lastTriggeredMinute[i] = minuteKey;
                executeTake(i);
                return;
            }
        }
    }
}
