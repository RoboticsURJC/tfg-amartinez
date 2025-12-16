// src/main.cpp
#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h>

#include "lvgl_display.h"   // tu inicialización de pantalla/TFT_eSPI (si la tienes)
#include "lvgl_touch.h"     // crea el indev y set_read_cb en ese .cpp

// Si no tienes lvgl_begin()/lvgl_touch_init(), ver nota más abajo.
extern TFT_eSPI tft;           // si lo declaras en lvgl_display.cpp
lv_obj_t * label_coord = nullptr;

void create_ui() {
  // Pantalla actual
  lv_obj_t * scr = lv_scr_act();
  lv_obj_set_style_bg_color(scr, lv_color_white(), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

  // Label para coordenadas (parte inferior)
  label_coord = lv_label_create(scr);
  lv_label_set_text(label_coord, "0,0");
  lv_obj_align(label_coord, LV_ALIGN_BOTTOM_MID, 0, -8);

  // Botón grande en el centro
  lv_obj_t * btn = lv_btn_create(scr);
  lv_obj_set_size(btn, 180, 180);
  lv_obj_center(btn);
  lv_obj_set_style_radius(btn, LV_RADIUS_CIRCLE, LV_PART_MAIN);
  lv_obj_set_style_bg_color(btn, lv_color_make(160, 50, 200), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_PART_MAIN);

  // Label del botón
  lv_obj_t * lbl = lv_label_create(btn);
  lv_label_set_text(lbl, "INICIO");
  lv_obj_set_style_text_color(lbl, lv_color_white(), LV_PART_MAIN);
  lv_obj_center(lbl);

  // Evento CLICKED
  lv_obj_add_event_cb(btn, [](lv_event_t * e){
    Serial.println(">>> BOTON PULSADO");
  }, LV_EVENT_CLICKED, NULL);
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("== Inicio main LVGL test ==");

  // Inicializa display + LVGL (usa tu implementación existente)
  // Debe: iniciar TFT_eSPI, lv_init(), registrar display, draw buffer, etc.
  lvgl_begin();               // si ya lo tienes implementado

  // Espera un pelín y crea la UI
  delay(200);
  create_ui();

  Serial.println("[OK] UI creada");
  Serial.println("[OK] Touch LVGL inicializado (si lvgl_touch_init devolvió OK)");
}

void loop() {
    lv_timer_handler();

    // Solución al bug LVGL 9.4.0 en ESP32
    lvgl_touch_force_read();

    delay(5);
}
