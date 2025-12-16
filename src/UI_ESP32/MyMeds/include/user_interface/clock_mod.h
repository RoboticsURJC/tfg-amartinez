#pragma once
#include <lvgl.h>

void clock_sync();
void clock_init(lv_obj_t *parent);
void clock_update();
void show_clock_screen(lv_obj_t *parent);
static void btn_event_cb(lv_event_t *e);
void update_clock_task(lv_timer_t *timer);