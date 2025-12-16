#include <Arduino.h>
#include "logic/takes_logic.h"
#include "user_interface/pill_takes.h"

void executeTake(int index)
{
    Serial.printf("Ejecutando toma %d\n", index+1);
}

void checkTakes()
{
    struct tm timeInfo;
    if (!getLocalTime(&timeInfo)){
        return;
    }

    for (int i = 0; i < total_takes; i ++){
        int hh = atoi(&takes[i].hour[0]);
        int mm = atoi(&takes[i].hour[3]);

        if (timeInfo.tm_hour == hh && timeInfo.tm_min == mm){
            if (takes[i].repeat[timeInfo.tm_wday]){
                executeTake(i);
            }
        }
    }
}