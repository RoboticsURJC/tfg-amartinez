#include <lvgl.h>
#include <Arduino.h>
#include "user_interface/pill_takes.h"
#include "storage/takes_storage.h"

TakeConfig takes[MAX_TAKES];
int total_takes = 0;
static lv_obj_t *takes_list;

void takes_list_screen()
{
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_scr_load(scr);
    lv_obj_set_style_bg_color(scr, lv_color_make(48, 25, 52), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "Configuración de tomas");
    lv_obj_set_style_text_font(title, &montserrat_24_regular, LV_PART_MAIN);
    lv_obj_set_style_text_color(title, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t *btn_add = lv_btn_create(scr);
    lv_obj_set_size(btn_add, 300, 40);
    lv_obj_set_style_bg_color(btn_add, lv_color_make(160, 50, 200), LV_PART_MAIN);
    lv_obj_align(btn_add, LV_ALIGN_TOP_LEFT, 10, 50);

    lv_obj_t *lbl_btn_add = lv_label_create(scr);
    lv_label_set_text(lbl_btn_add, "+ Añadir toma");
    lv_obj_set_style_text_font(lbl_btn_add, &montserrat_24_regular, LV_PART_MAIN);
    lv_obj_set_style_text_color(lbl_btn_add, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(lbl_btn_add);

    lv_obj_add_event_cb(btn_add, [](lv_event_t *e){
        if (total_takes < MAX_TAKES){
            strcpy(takes[total_takes].hour, "00:00");
            for (int r=0; r < 7; r++){
                takes[total_takes].repeat[r] = true;
                takes[total_takes]. personalized = false;
                takes[total_takes].recordatory = true;
                takes[total_takes].warning_time = 0;
            }

            edit_takes_screen(total_takes);
            total_takes++;
        }
    }, LV_EVENT_CLICKED, NULL);

    takes_list = lv_obj_create(scr);
    lv_obj_set_size(takes_list, 300, 150);
    lv_obj_align(takes_list, LV_ALIGN_TOP_MID, 0, 100);
    lv_obj_set_scroll_dir(takes_list, LV_DIR_VER);
    lv_obj_set_flex_flow(takes_list, LV_FLEX_FLOW_COLUMN);

    for (int i=0; i < total_takes; i++){
        lv_obj_t *row = lv_obj_create(NULL);
        lv_obj_set_size(row, 280, 40);
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);

        char lbl_text[20];
        sprintf(lbl_text, "Toma %d", i+1);
        lv_label_set_text(lv_label_create(row), lbl_text);
        lv_label_set_text(lv_label_create(row), takes[i].hour);

        lv_obj_t *btn_edit = lv_btn_create(row);
        lv_obj_set_size(btn_edit, 70, 30);
        lv_label_set_text(lv_label_create(btn_edit), "Editar");
        lv_obj_set_user_data(btn_edit, (void*)i);

        lv_obj_add_event_cb(btn_edit, [](lv_event_t *e){
            int index = (int)lv_obj_get_user_data(lv_event_get_target(e));
            edit_takes_screen(index);
        }, LV_EVENT_CLICKED, NULL);
    }
}

