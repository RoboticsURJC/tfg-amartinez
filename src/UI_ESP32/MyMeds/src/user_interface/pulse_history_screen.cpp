#include "user_interface/pulse_history_screen.h"

#include <Arduino.h>
#include <lvgl.h>

#include "storage/pulse_history_storage.h"
#include "user_interface/home_mod.h"

static void back_btn_event_cb(lv_event_t *e)
{
    show_home_screen();
}

void show_pulse_history_screen(lv_obj_t *parent)
{
    lv_obj_clean(parent);
    lv_obj_set_style_bg_color(parent, lv_color_make(48, 25, 52), LV_PART_MAIN);

    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text(title, "Historial");
    lv_obj_set_style_text_color(title, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(title, &montserrat_34_regular, LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    PulseRecord records[PULSE_HISTORY_MAX];
    int count = loadPulseHistory( records, PULSE_HISTORY_MAX);

    if (count == 0) {
        lv_obj_t *empty = lv_label_create(parent);
        lv_label_set_text(empty, "Sin mediciones");
        lv_obj_set_style_text_color(empty, lv_color_white(), LV_PART_MAIN);
        lv_obj_set_style_text_font(empty, &lv_font_montserrat_24, LV_PART_MAIN);
        lv_obj_align(empty, LV_ALIGN_TOP_MID, 0, 110);
    } else {
        for (int i = 0; i < count; i++) {
            char text[96];

            const PulseRecord &record = records[count - 1 - i];

            snprintf(
                text,
                sizeof(text),
                "%s %s\n%d BPM",
                record.date.c_str(),
                record.time.c_str(),
                record.bpm
            );

            lv_obj_t *row = lv_label_create(parent);
            lv_label_set_text(row, text);
            lv_obj_set_style_text_color(row, lv_color_white(), LV_PART_MAIN);
            lv_obj_set_style_text_font(row, &lv_font_montserrat_24, LV_PART_MAIN);
            lv_obj_align(row, LV_ALIGN_TOP_LEFT, 30, 80 + i * 70);
        }
    }

    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, 180, 60);
    lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 520);
    lv_obj_set_style_bg_color(btn, lv_color_make(160, 50, 200), LV_PART_MAIN);
    lv_obj_add_event_cb(btn, back_btn_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *lbl = lv_label_create(btn);
    lv_label_set_text(lbl, "Volver");
    lv_obj_center(lbl);
}

