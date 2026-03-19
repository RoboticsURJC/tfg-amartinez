#include <lvgl.h>
#include <Arduino.h>
#include "user_interface/pill_takes.h"
#include "storage/takes_storage.h"

TakeConfig takes[MAX_TAKES];
int total_takes = 0;
static lv_obj_t *takes_list;

static int delete_index = -1;

static lv_obj_t *current_screen = NULL;

static void load_screen(lv_obj_t *scr)
{
    if (current_screen)
        lv_obj_del_async(current_screen);

    current_screen = scr;

    lv_scr_load(scr);
}

void confirm_delete(int index)
{
    delete_index = index;

    lv_obj_t *scr = lv_obj_create(NULL);
    load_screen(scr);

    lv_obj_set_style_bg_color(scr, lv_color_make(48,25,52), LV_PART_MAIN);

    // TEXT

    lv_obj_t *lbl = lv_label_create(scr);

    lv_label_set_text(lbl, "Eliminar toma?");
    lv_obj_set_style_text_color(lbl, lv_color_white(), LV_PART_MAIN);

    lv_obj_align(lbl, LV_ALIGN_TOP_MID, 0, 20);

    // CANCEL

    lv_obj_t *btn_cancel = lv_btn_create(scr);

    lv_obj_set_size(btn_cancel,240,55);
    lv_obj_set_style_bg_color(btn_cancel, lv_color_make(170, 170, 170), LV_PART_MAIN);

    lv_obj_align(btn_cancel, LV_ALIGN_BOTTOM_MID, 0, -30);

    lv_obj_t *l1 = lv_label_create(btn_cancel);

    lv_label_set_text(l1,"Cancelar");

    lv_obj_center(l1);

    lv_obj_add_event_cb(btn_cancel,
    [](lv_event_t *e)
    {
        takes_list_screen();
    },
    LV_EVENT_CLICKED,
    NULL);



    // DELETE

    lv_obj_t *btn_del = lv_btn_create(scr);

    lv_obj_set_size(btn_del,240,55);
    lv_obj_set_style_bg_color(btn_del, lv_color_make(255, 38, 0), LV_PART_MAIN);

    lv_obj_align(btn_del, LV_ALIGN_TOP_MID, 0, 80);

    lv_obj_t *l2 = lv_label_create(btn_del);

    lv_label_set_text(l2,"Borrar");

    lv_obj_center(l2);

    lv_obj_add_event_cb(btn_del,
    [](lv_event_t *e)
    {
        delete_take(delete_index);
        takes_list_screen();
    },
    LV_EVENT_CLICKED,
    NULL);
}

void takes_list_screen()
{
    lv_obj_t *scr = lv_obj_create(NULL);
    load_screen(scr);
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
    //lv_obj_align(btn_add, LV_ALIGN_TOP_LEFT, 10, 50);
    lv_obj_set_pos(btn_add, 5, 50);

    lv_obj_t *lbl_btn_add = lv_label_create(btn_add);
    lv_label_set_text(lbl_btn_add, "+ Añadir toma");
    lv_obj_set_style_text_font(lbl_btn_add, &montserrat_24_regular, LV_PART_MAIN);
    lv_obj_set_style_text_color(lbl_btn_add, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(lbl_btn_add);

    lv_obj_add_event_cb(btn_add, [](lv_event_t *e){
        // if (total_takes >= MAX_TAKES) return;

        // int new_index = total_takes;
        // total_takes++;

        // strcpy(takes[new_index].hour, "00:00");

        // for (int r = 0; r < 7; r++){
        //     takes[new_index].repeat[r] = true;
        // }

        // takes[new_index].personalized = false;
        // takes[new_index].recordatory = true;
        // takes[new_index].warning_time = 0;

        //edit_takes_screen(new_index);
        edit_takes_screen(-1);
    }, LV_EVENT_CLICKED, NULL);

    takes_list = lv_obj_create(scr);
    lv_obj_set_size(takes_list, 300, 150);
    //lv_obj_align(takes_list, LV_ALIGN_TOP_MID, 0, 100);
    lv_obj_set_pos(takes_list, 5, 100);
    lv_obj_set_scroll_dir(takes_list, LV_DIR_VER);
    lv_obj_set_flex_flow(takes_list, LV_FLEX_FLOW_COLUMN);

    for (int i=0; i < total_takes; i++){
        lv_obj_t *row = lv_obj_create(takes_list);
        lv_obj_set_size(row, 280, 45);
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
        //lv_obj_set_style_pad_all(row, 5, LV_PART_MAIN);
        lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        char lbl_text[20];
        sprintf(lbl_text, "Toma %d", i+1);

        lv_obj_t *lbl_name = lv_label_create(row);
        lv_label_set_text(lbl_name, lbl_text);
        lv_obj_set_style_text_font(lbl_name, &lv_font_montserrat_24, 0);

        lv_obj_t *lbl_hour = lv_label_create(row);
        lv_label_set_text(lbl_hour, takes[i].hour);
        lv_obj_set_style_text_font(lbl_hour, &lv_font_montserrat_24, 0);

        lv_obj_t *btn_edit = lv_btn_create(row);
        lv_obj_set_size(btn_edit, 36, 28);
        lv_obj_set_style_bg_color(btn_edit, lv_color_make(0, 255, 33), LV_PART_MAIN);

        lv_obj_t *lbl_edit = lv_label_create(btn_edit);
        lv_label_set_text(lbl_edit, LV_SYMBOL_EDIT);
        lv_obj_set_style_text_font(lbl_edit, &lv_font_montserrat_16, 0);
        lv_obj_center(lbl_edit);

        lv_obj_set_user_data(btn_edit, (void*)(intptr_t)i);

        lv_obj_add_event_cb(btn_edit, [](lv_event_t *e){
            lv_obj_t *btn = lv_event_get_target(e);

            lv_obj_t *row = lv_obj_get_parent(btn);

            int index = lv_obj_get_index(row);

            edit_takes_screen(index);
        }, LV_EVENT_CLICKED, NULL);

        lv_obj_t *btn_del = lv_btn_create(row);
        lv_obj_set_size(btn_del, 36, 28);
        lv_obj_set_style_bg_color(btn_del, lv_color_make(255, 38, 0), LV_PART_MAIN);

        lv_obj_t *lbl_del = lv_label_create(btn_del);
        lv_label_set_text(lbl_del, LV_SYMBOL_TRASH);
        lv_obj_set_style_text_font(lbl_del, &lv_font_montserrat_16, 0);
        lv_obj_center(lbl_del);

        lv_obj_set_user_data(btn_del, (void*)(intptr_t)i);

        lv_obj_add_event_cb(btn_del, [](lv_event_t *e){
            
            lv_obj_t *btn = lv_event_get_target(e);

            lv_obj_t *row = lv_obj_get_parent(btn);

            int index = lv_obj_get_index(row);

            confirm_delete(index);

            // delete_take(index);

            // takes_list_screen();
        }, LV_EVENT_CLICKED, NULL);
    }

    lv_obj_t *btn_back = lv_btn_create(scr);
    lv_obj_set_size(btn_back, 300, 45);
    lv_obj_set_style_bg_color(btn_back, lv_color_make(160, 50, 200), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(btn_back, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(btn_back, 10, LV_PART_MAIN);
    lv_obj_set_pos(btn_back, 5, 260);

    lv_obj_t *lbl_back = lv_label_create(btn_back);
    lv_label_set_text(lbl_back, "Atrás");
    lv_obj_set_style_text_font(lbl_back, &montserrat_24_regular, LV_PART_MAIN);
    lv_obj_set_style_text_color(lbl_back, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(lbl_back);

    lv_obj_add_event_cb(btn_back, [](lv_event_t *e)
    {
        show_home_screen();
    }, LV_EVENT_CLICKED, NULL);
}

