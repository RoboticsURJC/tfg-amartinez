#include <lvgl.h>
#include <time.h>
#include <WiFi.h>
#include "user_interface/pill_takes.h"
#include "user_interface/home_mod.h"
#include <cstdio>

static lv_obj_t *current_screen = NULL;

static void load_screen(lv_obj_t *scr)
{
    if (current_screen)
        lv_obj_del_async(current_screen);

    current_screen = scr;

    lv_scr_load(scr);
}

int
get_today_index()
{
    struct tm info;
    if (!getLocalTime(&info)){
       return -1; 
    }

    int w = info.tm_wday;

    if (w == 0) {
        return 6;
    }
        
    return w - 1;
}

bool
is_take_today(int index)
{
    int today = get_today_index();

    if (today < 0) {
       return false; 
    }

    if (index < 0 || index >= total_takes) {
        return false;
    }

    return takes[index].repeat[today];
}

void days_text_repeat(bool rep[7], char *buf)
{
    const char *d[7] = {"L","M","X","J","V","S","D"};

    buf[0] = 0;

    for (int i = 0; i < 7; i++)
    {
        if (rep[i])
        {
            strcat(buf, d[i]);
            strcat(buf, " ");
        }
    }
}

void
today_takes_screen()
{
    lv_obj_t *scr = lv_obj_create(NULL);
    load_screen(scr);

    lv_obj_set_style_bg_color(scr, lv_color_make(48,25,52), LV_PART_MAIN);

    lv_obj_t *title = lv_label_create(scr);

    lv_label_set_text(title, "Tomas de hoy");
    lv_obj_set_style_text_color(title, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);


    lv_obj_t *list = lv_obj_create(scr);
    lv_obj_set_size(list, 300, 150);
    lv_obj_set_pos(list, 10, 60);
    lv_obj_set_scroll_dir(list, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(list, 0, 0);
    lv_obj_set_style_pad_all(list, 4, 0);

    lv_obj_clear_flag(list, LV_OBJ_FLAG_SCROLL_ELASTIC);
    lv_obj_clear_flag(list, LV_OBJ_FLAG_SCROLL_MOMENTUM);
    // lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);

    for (int i = 0; i < total_takes; i++)
    {
        if (!is_take_today(i))
            continue;

        lv_obj_t *row = lv_obj_create(list);
        lv_obj_set_width(row, 280);
        lv_obj_set_height(row, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_COLUMN);

        lv_obj_t *btn = lv_btn_create(row);
        lv_obj_set_size(btn, 250, 40);
        lv_obj_set_style_bg_color(btn, lv_color_make(160, 50, 200), LV_PART_MAIN);
        
        char txt[32];
        sprintf(txt, "Toma %d  %s", i + 1, takes[i].hour);

        lv_obj_t *lbl = lv_label_create(btn);
        lv_label_set_text(lbl, txt);
        lv_obj_set_style_text_font(lbl, &montserrat_24_regular, LV_PART_MAIN);
        lv_obj_center(lbl);

        lv_obj_t *details = lv_obj_create(row);
        lv_obj_set_width(details, 250);
        lv_obj_set_height(details, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(details, LV_FLEX_FLOW_COLUMN);

        lv_obj_set_style_pad_all(details, 5, 0);
        lv_obj_set_style_pad_row(details, 4, 0);
        lv_obj_add_flag(details, LV_OBJ_FLAG_HIDDEN);

        char rec_txt[32];
        sprintf(rec_txt, "Recordatorio: %s", takes[i].recordatory ? "Si" : "No");
        lv_obj_t *l1 = lv_label_create(details);
        lv_obj_set_style_text_font(l1, &montserrat_24_regular, LV_PART_MAIN);
        lv_label_set_text(l1, rec_txt);

        char warn_txt[32];
        sprintf(warn_txt, "Aviso: %d min", takes[i].warning_time);
        lv_obj_t *l2 = lv_label_create(details);
        lv_obj_set_style_text_font(l2, &montserrat_24_regular, LV_PART_MAIN);
        lv_label_set_text(l2, warn_txt);

        char rep_txt[64] = "Días: ";
        char tmp[32];
        days_text_repeat(takes[i].repeat, tmp);
        strcat(rep_txt, tmp);
        lv_obj_t *l3 = lv_label_create(details);
        lv_obj_set_style_text_font(l3, &montserrat_24_regular, LV_PART_MAIN);
        lv_label_set_text(l3, rep_txt);

        lv_obj_set_user_data(btn, details);

        lv_obj_add_event_cb(btn, [](lv_event_t *e)
        {
            lv_obj_t *btn = lv_event_get_target(e);
            lv_obj_t *details = (lv_obj_t *)lv_obj_get_user_data(btn);

            if (!details) {
                return;
            }

            if (lv_obj_has_flag(details, LV_OBJ_FLAG_HIDDEN))
            {
                lv_obj_clear_flag(details, LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_add_flag(details, LV_OBJ_FLAG_HIDDEN);
            }

            lv_obj_mark_layout_as_dirty(
                lv_obj_get_parent(details)
            );
        }, LV_EVENT_CLICKED, NULL);
    }

    lv_obj_t *btn_back = lv_btn_create(scr);
    lv_obj_set_size(btn_back, 300, 45);
    lv_obj_set_style_bg_color(btn_back, lv_color_make(160, 50, 200), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(btn_back, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(btn_back, 10, LV_PART_MAIN);
    lv_obj_set_pos(btn_back, 10, 255);

    lv_obj_t *lbl = lv_label_create(btn_back);
    lv_obj_set_style_text_font(lbl, &montserrat_24_regular, LV_PART_MAIN);
    lv_obj_set_style_text_color(lbl, lv_color_white(), LV_PART_MAIN);
    lv_label_set_text(lbl, "Volver");
    
    lv_obj_center(lbl);

    lv_obj_add_event_cb(btn_back, [](lv_event_t *e)
    {
        show_home_screen();
    }, LV_EVENT_CLICKED, NULL);
}
