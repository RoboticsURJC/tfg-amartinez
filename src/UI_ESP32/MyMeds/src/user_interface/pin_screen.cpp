#include <lvgl.h>
#include <Arduino.h>
#include <cstdio>

#include "user_interface/home_mod.h"

static String entered_pin = "";

static lv_obj_t *label_pin;
static lv_obj_t *label_error;

extern String DEVICE_PIN;

static void update_pin_label()
{
    String stars = "";

    for (int i = 0; i < entered_pin.length(); i++) {
        stars += "*";
    }

    if (label_pin) {
        lv_label_set_text(
            label_pin,
            stars.c_str()
        );
    }
}

void show_pin_screen()
{
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_scr_load(scr);
    lv_obj_set_style_bg_color(scr, lv_color_make(48, 25, 52), LV_PART_MAIN);

    entered_pin = "";

    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title,"Introducir PIN");
    lv_obj_set_style_text_font(title,&lv_font_montserrat_24,0);
    lv_obj_set_style_text_color(title,lv_color_white(),0);
    lv_obj_align(title,LV_ALIGN_TOP_MID,0,20);

    label_pin = lv_label_create(scr);
    lv_label_set_text(label_pin, "");
    lv_obj_set_style_text_font(label_pin,&lv_font_montserrat_32,0);
    lv_obj_set_style_text_color(label_pin,lv_color_white(),0);
    lv_obj_align(label_pin,LV_ALIGN_TOP_MID,0,80);

    label_error = lv_label_create(scr);
    if (label_error) {
        lv_label_set_text(label_error, "");
    }

    lv_obj_set_style_text_color(label_error, lv_palette_main(LV_PALETTE_RED),0);
    lv_obj_set_style_text_font(label_error, &lv_font_montserrat_24, 0);
    lv_obj_align(label_error, LV_ALIGN_TOP_MID, 0, 130);

    int num = 1;

    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            lv_obj_t *btn = lv_btn_create(scr);

            lv_obj_set_style_bg_color(btn,lv_color_make(160, 50, 200),LV_PART_MAIN);
            lv_obj_set_style_radius(btn,15,LV_PART_MAIN);
            lv_obj_set_size(btn, 70, 70);

            lv_obj_align(btn,LV_ALIGN_TOP_LEFT,35 + col * 85,160 + row * 85);
            lv_obj_t *lbl = lv_label_create(btn);

            char txt[4];
            snprintf(txt, sizeof(txt), "%d", num);

            lv_label_set_text(lbl, txt);
            lv_obj_set_style_text_color(lbl,lv_color_white(),0);

            lv_obj_center(lbl);

            int digit = num;

            lv_obj_add_event_cb(btn,
                [](lv_event_t *e)
            {
                int digit =
                    (int)(intptr_t)
                    lv_event_get_user_data(e);

                if (entered_pin.length() >= 4)
                    return;

                if (label_error) {
                    lv_label_set_text(label_error, "");
                }
                entered_pin += String(digit);

                update_pin_label();

            }, LV_EVENT_CLICKED,
               (void*)(intptr_t)digit);

            num++;
        }
    }

    // botón 0

    lv_obj_t *btn0 = lv_btn_create(scr);
    lv_obj_set_style_bg_color(btn0,lv_color_make(160, 50, 200),LV_PART_MAIN);
    lv_obj_set_style_radius(btn0,15,LV_PART_MAIN);
    lv_obj_set_size(btn0, 70, 70);
    lv_obj_align(btn0,LV_ALIGN_TOP_MID,0,415);

    lv_obj_t *lbl0 = lv_label_create(btn0);
    lv_label_set_text(lbl0, "0");
    lv_obj_set_style_text_color(lbl0,lv_color_white(),0);
    lv_obj_center(lbl0);

    lv_obj_add_event_cb(btn0,
        [](lv_event_t *e)
    {
        if (entered_pin.length() >= 4)
            return;
        
        if (label_error) {
            lv_label_set_text(label_error, "");
        }

        entered_pin += "0";

        update_pin_label();

    }, LV_EVENT_CLICKED, NULL);

    // botón OK

    lv_obj_t *btn_ok = lv_btn_create(scr);
    lv_obj_set_style_bg_color(btn_ok,lv_color_make(160, 50, 200),LV_PART_MAIN);
    lv_obj_set_style_radius(btn_ok,15,LV_PART_MAIN);

    lv_obj_set_size(btn_ok, 90, 55);
    lv_obj_set_pos(btn_ok,190,500);

    lv_obj_t *lbl_ok = lv_label_create(btn_ok);
    lv_label_set_text(lbl_ok, "OK");
    lv_obj_set_style_text_color(lbl_ok,lv_color_white(),0);
    lv_obj_center(lbl_ok);

    lv_obj_add_event_cb(btn_ok,
        [](lv_event_t *e)
    {
        if (entered_pin == DEVICE_PIN)
        {
            label_error = nullptr;
            label_pin = nullptr;

            show_home_screen();
        } else {
            entered_pin = "";

            update_pin_label();

            if (label_error) {
                lv_label_set_text(
                    label_error,
                    "PIN incorrecto"
                );
            }
        }

    }, LV_EVENT_CLICKED, NULL);

    // borrar

    lv_obj_t *btn_del = lv_btn_create(scr);
    lv_obj_set_style_bg_color(btn_del,lv_color_make(160, 50, 200),LV_PART_MAIN);
    lv_obj_set_style_radius(btn_del,15,LV_PART_MAIN);
    lv_obj_set_size(btn_del, 140, 55);

    lv_obj_set_pos(btn_del, 25, 500);
    lv_obj_t *lbl_del = lv_label_create(btn_del);
    lv_label_set_text(lbl_del, "Borrar");
    lv_obj_center(lbl_del);

    lv_obj_add_event_cb(btn_del,
        [](lv_event_t *e)
    {
        if (entered_pin.length() > 0)
        {
            entered_pin.remove(
                entered_pin.length() - 1
            );

            update_pin_label();
        }

    }, LV_EVENT_CLICKED, NULL);
}