#include <Arduino.h>
#include <lvgl.h>

#include "lvgl_display.h"
#include "lvgl_touch.h"

void setup_hw()
{
    Serial.begin(115200);
    delay(300);

    // Inicializar LVGL + pantalla
    lvgl_begin();

    // Obtener el display activo para pasarlo al touch
    lv_display_t * disp = lv_display_get_default();

    // Inicializar el touchscreen (TU FUNCIÓN)
    lvgl_touch_init(disp);

    // Crear un label simple para probar que funciona
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello World!");
    lv_obj_center(label);
}

void loop_hw()
{
    lv_timer_handler();  // Ejecuta LVGL
    delay(5);
}
