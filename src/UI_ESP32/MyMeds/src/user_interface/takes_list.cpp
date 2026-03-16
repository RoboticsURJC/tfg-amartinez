#include <lvgl.h>
#include <Arduino.h>
#include "user_interface/pill_takes.h"
#include "storage/takes_storage.h"

TakeConfig takes[MAX_TAKES];
int total_takes = 0;
static lv_obj_t *takes_list;

static lv_obj_t *popup_bg = NULL;
static lv_obj_t *popup_win = NULL;
static int delete_index = -1;

static void popup_close(lv_event_t *e)
{
    if (popup_bg)
    {
        lv_obj_del(popup_bg);
        popup_bg = NULL;
        popup_win = NULL;
    }
}

static void popup_delete(lv_event_t *e)
{
    delete_take(delete_index);

    if (popup_bg)
    {
        lv_obj_del(popup_bg);
        popup_bg = NULL;
        popup_win = NULL;
    }
}

void confirm_delete(int index)
{
    delete_index = index;

    // Fondo
    popup_bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(popup_bg, 320, 240);
    lv_obj_center(popup_bg);

    lv_obj_set_style_bg_color(popup_bg, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(popup_bg, LV_OPA_50, 0);
    lv_obj_clear_flag(popup_bg, LV_OBJ_FLAG_SCROLLABLE);

    // Ventana
    popup_win = lv_obj_create(popup_bg);
    lv_obj_set_size(popup_win, 280, 150);
    lv_obj_center(popup_win);

    lv_obj_set_style_radius(popup_win, 15, 0);
    lv_obj_set_style_bg_color(popup_win, lv_color_make(48, 25, 52), 0);
    lv_obj_clear_flag(popup_win, LV_OBJ_FLAG_SCROLLABLE);

    // Texto
    lv_obj_t *lbl = lv_label_create(popup_win);
    lv_label_set_text(lbl, "Confirme eliminar");
    lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_align(lbl, LV_ALIGN_TOP_MID, 0, 10);

    // Botón Cancelar
    lv_obj_t *btn_cancel = lv_obj_create(popup_win);
    lv_obj_set_size(btn_cancel, 130, 60);
    lv_obj_align(btn_cancel, LV_ALIGN_BOTTOM_LEFT, 5, -10);
    lv_obj_set_style_bg_color(btn_cancel, lv_color_make(209, 209, 209), 0);
    lv_obj_add_event_cb(btn_cancel, popup_close, LV_EVENT_CLICKED, NULL);
    lv_obj_t * lbl1 = lv_label_create(btn_cancel);
    lv_label_set_text(lbl1, "Cancelar");
    lv_obj_set_style_text_font(lbl1, &lv_font_montserrat_22, LV_PART_MAIN);
    lv_obj_center(lbl1);

    // Botón Borrar
    lv_obj_t *btn_del = lv_obj_create(popup_win);
    lv_obj_set_size(btn_del, 80, 60);
    lv_obj_align(btn_del, LV_ALIGN_BOTTOM_RIGHT, -5, -10);
    lv_obj_set_style_bg_color(btn_del, lv_color_make(255, 38, 0), 0);
    lv_obj_add_event_cb(btn_del, popup_delete, LV_EVENT_CLICKED, NULL);
    lv_obj_t * lbl2 = lv_label_create(btn_del);
    lv_label_set_text(lbl2, LV_SYMBOL_TRASH);
    lv_obj_set_style_text_font(lbl2, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_center(lbl2);
}

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

        lv_obj_set_user_data(btn_edit, (void*)i);

        lv_obj_add_event_cb(btn_edit, [](lv_event_t *e){
            int index = (int)lv_obj_get_user_data(lv_event_get_target(e));
            edit_takes_screen(index);
        }, LV_EVENT_CLICKED, NULL);

        lv_obj_t *btn_del = lv_btn_create(row);
        lv_obj_set_size(btn_del, 36, 28);
        lv_obj_set_style_bg_color(btn_del, lv_color_make(255, 38, 0), LV_PART_MAIN);

        lv_obj_t *lbl_del = lv_label_create(btn_del);
        lv_label_set_text(lbl_del, LV_SYMBOL_TRASH);
        lv_obj_set_style_text_font(lbl_del, &lv_font_montserrat_16, 0);
        lv_obj_center(lbl_del);

        lv_obj_set_user_data(btn_del, (void*)i);

        lv_obj_add_event_cb(btn_del, [](lv_event_t *e){
            int index = (int)lv_obj_get_user_data(lv_event_get_target(e));
            confirm_delete(index);
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

