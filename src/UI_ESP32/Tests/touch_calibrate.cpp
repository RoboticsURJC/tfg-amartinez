#include <Arduino.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include "lvgl_touch.h"

#define T_CS   33
#define T_IRQ  36
#define T_CLK  25
#define T_MISO 39
#define T_MOSI 32

void setup_cal() {
    Serial.begin(115200);
    SPI.begin(T_CLK, T_MISO, T_MOSI);

    ts.begin();

    Serial.println("Toca esquinas y mira valores RAW…");
}

void loop_cal() {
    if (ts.touched()) {
        TS_Point p = ts.getPoint();
        
        delay(150);
    }
}
