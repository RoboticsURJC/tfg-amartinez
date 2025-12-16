#include <lvgl.h>
#include <Arduino.h>
#include "user_interface/pill_takes.h"
#include "user_interface/home_mod.h"

static void btn_event_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    const char *txt = (const char*)lv_event_get_user_data(e);

    if (!strcmp(txt, "Configuración de tomas")){
        takes_list_screen();
        return;
    }
}

void show_home_screen()
{
    lv_obj_t * scr = lv_obj_create(NULL);
    lv_scr_load(scr);
    lv_obj_set_style_bg_color(scr, lv_color_make(48, 25, 52), LV_PART_MAIN);

    lv_obj_t *btn1 = lv_btn_create(scr);
    lv_obj_set_size(btn1, 265, 70);
    lv_obj_set_style_bg_color(btn1, lv_color_make(160, 50, 200), LV_PART_MAIN);
    lv_obj_align(btn1, LV_ALIGN_TOP_MID, 0, 40);
    lv_obj_add_event_cb(btn1, btn_event_cb, LV_EVENT_CLICKED, (void *)"Tomas hoy");

    lv_obj_t *lbl1 = lv_label_create(scr);
    lv_label_set_text(lbl1, "Tomas hoy");
    lv_obj_set_style_text_font(lbl1, &montserrat_34_regular, LV_PART_MAIN);
    lv_obj_center(lbl1);

    // ---------------------------------------------------------------

    lv_obj_t *btn2 = lv_btn_create(scr);
    lv_obj_set_size(btn2, 265, 70);
    lv_obj_set_style_bg_color(btn2, lv_color_make(160, 50, 200), LV_PART_MAIN);
    lv_obj_align(btn2, LV_ALIGN_TOP_MID, 0, 130);
    lv_obj_add_event_cb(btn2, btn_event_cb, LV_EVENT_CLICKED, (void *)"Historial");

    lv_obj_t *lbl2 = lv_label_create(scr);
    lv_label_set_text(lbl2, "Historial");
    lv_obj_set_style_text_font(lbl2, &montserrat_34_regular, LV_PART_MAIN);
    lv_obj_center(lbl2);

    // ---------------------------------------------------------------

    lv_obj_t *btn3 = lv_btn_create(scr);
    lv_obj_set_size(btn3, 265, 70);
    lv_obj_set_style_bg_color(btn3, lv_color_make(160, 50, 200), LV_PART_MAIN);
    lv_obj_align(btn3, LV_ALIGN_TOP_MID, 0, 220);
    lv_obj_add_event_cb(btn3, btn_event_cb, LV_EVENT_CLICKED, (void *)"Pulsómetro");

    lv_obj_t *lbl3 = lv_label_create(scr);
    lv_label_set_text(lbl3, "Pulsómetro");
    lv_obj_set_style_text_font(lbl3, &montserrat_34_regular, LV_PART_MAIN);
    lv_obj_center(lbl3);

    // ---------------------------------------------------------------

    lv_obj_t *btn4 = lv_btn_create(scr);
    lv_obj_set_size(btn4, 265, 150);
    lv_obj_set_style_bg_color(btn4, lv_color_make(160, 50, 200), LV_PART_MAIN);
    lv_obj_align(btn4, LV_ALIGN_TOP_MID, 0, 310);
    lv_obj_add_event_cb(btn4, btn_event_cb, LV_EVENT_CLICKED, (void *)"Configuración de tomas");

    lv_obj_t *lbl4 = lv_label_create(scr);
    lv_label_set_text(lbl4, "Configuración\n           de\n        tomas");
    lv_obj_set_style_text_font(lbl4, &montserrat_34_regular, LV_PART_MAIN);
    lv_obj_center(lbl4);
}
