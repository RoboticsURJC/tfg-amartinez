#include <lvgl.h>
#include <Arduino.h>
#include <Preferences.h>

#include "user_interface/change_pin_screen.h"
#include "user_interface/settings_screen.h"

extern String DEVICE_PIN;

static lv_obj_t *label_pin;
static lv_obj_t *label_error;
static lv_obj_t *title_label;

static int pin_step = 0;

static String entered_pin = "";
static String new_pin = "";

static void update_pin_label()
{
    String stars = "";

    for (int i = 0; i < entered_pin.length(); i++) {
        stars += "*";
    }

    lv_label_set_text(
        label_pin,
        stars.c_str()
    );
}

static void update_title()
{
    if (pin_step == 0)
        lv_label_set_text(title_label, "PIN actual");

    else if (pin_step == 1)
        lv_label_set_text(title_label, "Nuevo PIN");

    else
        lv_label_set_text(title_label, "Confirmar PIN");
}

void show_change_pin_screen(){
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_scr_load(scr);
    lv_obj_set_style_bg_color(scr, lv_color_make(48, 25, 52), LV_PART_MAIN);

    entered_pin = "";

    title_label = lv_label_create(scr);
    lv_label_set_text(title_label,"Introducir PIN");
    lv_obj_set_style_text_font(title_label,&lv_font_montserrat_24,0);
    lv_obj_set_style_text_color(title_label,lv_color_white(),0);
    lv_obj_align(title_label,LV_ALIGN_TOP_MID,0,20);

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

    lv_obj_add_event_cb(btn_ok,[](lv_event_t *e)
        {
            if (pin_step == 0)
            {
                if (entered_pin == DEVICE_PIN)
                {
                    Serial.println("PIN actual validado");
                    pin_step = 1;
                    entered_pin = "";

                    update_pin_label();
                    update_title();

                    lv_label_set_text(label_error,"");
                }
                else
                {
                    entered_pin = "";
                    update_pin_label();
                    lv_label_set_text(label_error,"PIN incorrecto");
                }
            }
            else if (pin_step == 1)
            {
                if (entered_pin.length() != 4)
                {
                    lv_label_set_text(label_error,"Debe tener 4 digitos");
                    return;
                }

                new_pin = entered_pin;
                entered_pin = "";
                pin_step = 2;

                update_pin_label();
                update_title();
                lv_label_set_text(label_error,"");
            }
            else if (pin_step == 2)
            {
                if (entered_pin == new_pin)
                {
                    Preferences p;
                    p.begin("sys",false);
                    p.putString("pin",new_pin);
                    p.end();

                    DEVICE_PIN = new_pin;

                    Serial.println("PIN cambiado:");
                    Serial.println(new_pin);

                    lv_obj_clean(lv_scr_act());
                    lv_obj_set_style_bg_color(lv_scr_act(),lv_color_make(48, 25, 52),LV_PART_MAIN);

                    entered_pin = "";
                    new_pin = "";
                    pin_step = 0;

                    // Mensaje

                    lv_obj_t *msg = lv_label_create(lv_scr_act());
                    lv_label_set_text(msg,"PIN actualizado");
                    lv_obj_set_style_text_font(msg,&lv_font_montserrat_32,0);
                    lv_obj_set_style_text_color(msg,lv_color_white(), 0);

                    lv_obj_center(msg);

                    // Volver a ajustes en 3 segundos

                    lv_timer_create(
                        [](lv_timer_t *t)
                        {
                            show_settings_screen();
                            lv_timer_del(t);

                        },3000,NULL
                    );
                }
                else
                {
                    entered_pin = "";
                    update_pin_label();
                    lv_label_set_text(label_error,"No coincide");
                }
            }

        },LV_EVENT_CLICKED, NULL
    );

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

    // volver

    lv_obj_t *btn_back = lv_btn_create(lv_scr_act());
    lv_obj_set_style_bg_color(btn_back,lv_color_make(160, 50, 200),LV_PART_MAIN);
    lv_obj_set_style_radius(btn_back,15,LV_PART_MAIN);
    lv_obj_set_size(btn_back,255,55);
    lv_obj_set_pos(btn_back,25,565);

    lv_obj_t *lbl_back = lv_label_create(btn_back);
    lv_label_set_text(lbl_back,"Volver");
    lv_obj_set_style_text_color(lbl_back,lv_color_white(),0);
    lv_obj_center(lbl_back);
    lv_obj_add_event_cb(btn_back,[](lv_event_t *e)
        {
            entered_pin = "";
            new_pin = "";
            pin_step = 0;

            show_settings_screen();
        },LV_EVENT_CLICKED,NULL
    );
}