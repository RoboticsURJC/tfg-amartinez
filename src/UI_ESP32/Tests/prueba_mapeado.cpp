#include <Arduino.h>
#include <lvgl.h>
#include "lvgl_display.h"
#include "lvgl_touch.h"

// Label global para mostrar coordenadas
lv_obj_t * label_coord;

void setup_map() {
    Serial.begin(115200);

    // Inicializar LVGL y tu display (definido en lvgl_display.cpp)
    lvgl_begin();

    // Crear un label en pantalla para mostrar coordenadas
    label_coord = lv_label_create(lv_scr_act());
    lv_label_set_text(label_coord, "0,0");
    lv_obj_align(label_coord, LV_ALIGN_BOTTOM_MID, 0, -10);

    Serial.println("Prueba de calibración: toca esquinas y centro");
}

void loop_map() {
    lv_timer_handler();
    delay(5);

    // Lectura directa del touch para calibración
    if (ts.touched()) {
        TS_Point p = ts.getPoint();

        // Mapeo de RAW a coordenadas de pantalla
        int32_t x = fmap(p.x, RAW_X_MIN, RAW_X_MAX, 0, SCREEN_W - 1);
        int32_t y = fmap(p.y, RAW_Y_MIN, RAW_Y_MAX, 0, SCREEN_H - 1);

        // Mostrar en Serial RAW y MAP
        Serial.printf("RAW X=%d RAW Y=%d Z=%d -> MAP X=%d Y=%d\n",
                      p.x, p.y, p.z, x, y);

        // Actualizar el label en pantalla
        char buf[32];
        sprintf(buf, "%d,%d", x, y);
        lv_label_set_text(label_coord, buf);
    }
}
