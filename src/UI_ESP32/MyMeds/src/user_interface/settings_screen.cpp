#include <lvgl.h>

#include "user_interface/settings_screen.h"
#include "user_interface/change_pin_screen.h"
#include "user_interface/home_mod.h"

void show_settings_screen()
{
    lv_obj_clean(lv_scr_act());

    lv_obj_set_style_bg_color(lv_scr_act(),lv_color_make(48, 25, 52), LV_PART_MAIN);
    lv_obj_t *title = lv_label_create(lv_scr_act());
    lv_label_set_text(title,"Ajustes");
    lv_obj_set_style_text_color(title,lv_color_white(),0);
    lv_obj_set_style_text_font(title,&montserrat_30_regular,0);
    lv_obj_set_pos(title,110,20);

    // CAMBIAR PIN

    lv_obj_t *btnPin = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btnPin,260,70);
    lv_obj_set_pos(btnPin, 30,120);
    lv_obj_set_style_bg_color(btnPin,lv_color_make(160, 50, 200),LV_PART_MAIN);

    lv_obj_t *lblPin = lv_label_create(btnPin);
    lv_label_set_text(lblPin,"Cambiar PIN");
    lv_obj_set_style_text_font(lblPin,&montserrat_30_regular,0);
    lv_obj_center(lblPin);
    lv_obj_add_event_cb(btnPin,[](lv_event_t *e)
        {
            show_change_pin_screen();
        },LV_EVENT_CLICKED,NULL
    );

    // VOLVER

    lv_obj_t *btnBack = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btnBack,260,70);
    lv_obj_set_pos(btnBack,30,220);
    lv_obj_set_style_bg_color(btnBack,lv_color_make(160, 50, 200),LV_PART_MAIN);

    lv_obj_t *lblBack = lv_label_create(btnBack);
    lv_label_set_text(lblBack,"Volver");
    lv_obj_center(lblBack);
    lv_obj_add_event_cb(btnBack,[](lv_event_t *e)
        {
            show_home_screen();
        },LV_EVENT_CLICKED,NULL
    );
}