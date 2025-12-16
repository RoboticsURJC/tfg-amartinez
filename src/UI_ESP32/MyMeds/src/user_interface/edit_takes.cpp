#include <lvgl.h>
#include <Arduino.h>
#include "user_interface/pill_takes.h"
#include "storage/takes_storage.h"

static int edit_index;
static lv_obj_t *roller_hours;
static lv_obj_t *roller_minutes;
static lv_obj_t *days_repeat;
static lv_obj_t *days_warning;
static lv_obj_t *sw_recordatory;

static int right_x(int width)
{
    return 320 - width - 10;
}

void set_rollers_init(int index)
{
    int hh, mm;
    sscanf(takes[index].hour, "%d:%d", hh, mm);
}

void edit_takes_screen(int index)
{
    edit_index = index;

    lv_obj_t *scr = lv_obj_create(NULL);
    lv_scr_load(scr);
    lv_obj_set_style_bg_color(scr, lv_color_make(48, 25, 52), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

    char txt_tittle[20];
    sprintf(txt_tittle, "Editar TOMA %d", index+1);
    lv_obj_t *lbl_tittle = lv_label_create(scr);
    lv_label_set_text(lbl_tittle, txt_tittle);
    lv_obj_set_style_text_font(lbl_tittle, &montserrat_30_regular, LV_PART_MAIN);
    lv_obj_set_style_text_color(lbl_tittle, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(lbl_tittle, LV_ALIGN_TOP_MID, 0, 5);

    // ----------- HORA -------------------------------------------
    lv_obj_t *lbl_hour = lv_label_create(scr);
    lv_label_set_text(lbl_hour, "Hora");
    lv_obj_set_style_text_font(lbl_hour, &montserrat_24_regular, LV_PART_MAIN);
    lv_obj_set_style_text_color(lbl_hour, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_pos(lbl_hour, 10, 50);

    const int roller_w = 70;
    const int roller_h = 60;
    const int roller_gap = 10;
    const int group_w = roller_w*2 + roller_gap;

    lv_obj_t *hour_group = lv_obj_create(scr);
    lv_obj_set_size(hour_group, group_w, roller_h);
    lv_obj_set_pos(hour_group, right_x(group_w), 45);
    lv_obj_set_style_bg_opa(hour_group, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(hour_group, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(hour_group, 0, LV_PART_MAIN);
    lv_obj_clear_flag(hour_group, LV_OBJ_FLAG_SCROLLABLE);

    //****ROLLER HOURS****
    roller_hours = lv_roller_create(hour_group);
    lv_roller_set_options(roller_hours,
        "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n"
        "12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23",
        LV_ROLLER_MODE_NORMAL);
    lv_obj_set_size(roller_hours, roller_w, roller_h);
    lv_obj_set_pos(roller_hours, 0, 0);

    //Normal
    lv_obj_set_style_bg_opa(roller_hours, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_radius(roller_hours, 10, LV_PART_MAIN);
    lv_obj_set_style_clip_corner(roller_hours, true, LV_PART_MAIN);
    lv_obj_set_style_border_width(roller_hours, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(roller_hours, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(roller_hours, 0, LV_PART_MAIN);
    lv_obj_set_style_text_color(roller_hours, lv_color_black(), LV_PART_MAIN);

    //Selected
    lv_obj_set_style_bg_color(roller_hours, lv_color_make(238, 173, 253), LV_PART_SELECTED);
    lv_obj_set_style_bg_opa(roller_hours, LV_OPA_COVER, LV_PART_SELECTED);
    lv_obj_set_style_radius(roller_hours, 10, LV_PART_SELECTED);
    lv_obj_set_style_pad_all(roller_hours, 0, LV_PART_SELECTED);
    lv_obj_set_style_text_color(roller_hours, lv_color_black(), LV_PART_SELECTED);

    //****ROLLER MINUTES****
    roller_minutes = lv_roller_create(hour_group);
    lv_roller_set_options(roller_minutes,
        "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n"
        "10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n"
        "20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n"
        "30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n"
        "40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n"
        "50\n51\n52\n53\n54\n55\n56\n57\n58\n59",
        LV_ROLLER_MODE_NORMAL);
    lv_obj_set_size(roller_minutes, roller_w, roller_h);
    lv_obj_set_pos(roller_minutes, roller_w+roller_gap, 0);

    //Normal
    lv_obj_set_style_bg_opa(roller_minutes, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_radius(roller_minutes, 10, LV_PART_MAIN);
    lv_obj_set_style_clip_corner(roller_minutes, true, LV_PART_MAIN);
    lv_obj_set_style_border_width(roller_minutes, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(roller_minutes, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(roller_minutes, 0, LV_PART_MAIN);
    lv_obj_set_style_text_color(roller_minutes, lv_color_black(), LV_PART_MAIN);

    //Selected
    lv_obj_set_style_bg_color(roller_minutes, lv_color_make(238, 173, 253), LV_PART_SELECTED);
    lv_obj_set_style_bg_opa(roller_minutes, LV_OPA_COVER, LV_PART_SELECTED);
    lv_obj_set_style_radius(roller_minutes, 10, LV_PART_SELECTED);
    lv_obj_set_style_pad_all(roller_minutes, 0, LV_PART_SELECTED);
    lv_obj_set_style_text_color(roller_minutes, lv_color_black(), LV_PART_SELECTED);

    set_rollers_init(index);

    //----------- REPETITION -------------------------------------------
    const int rep_row_y = 120;
    const int rep_dd_w = 190;
    const int rep_dd_h = 40;
    int dd_x = right_x(rep_dd_w);

    lv_obj_t *lbl_repeat = lv_label_create(scr);
    lv_label_set_text(lbl_repeat, "Repetir");
    lv_obj_set_style_text_font(lbl_repeat, &montserrat_24_regular, LV_PART_MAIN);
    lv_obj_set_style_text_color(lbl_repeat, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_pos(lbl_repeat, 10, rep_row_y);

    days_repeat = lv_dropdown_create(scr);
    lv_dropdown_set_options(days_repeat,
        "Todos los días\n"
        "Entre semana\n"
        "Personalizar");
    lv_obj_set_size(days_repeat, rep_dd_w, rep_dd_h);
    lv_obj_set_pos(days_repeat, dd_x, rep_row_y-5);
    lv_obj_set_style_text_font(days_repeat, &montserrat_24_regular, LV_PART_MAIN);
    lv_obj_set_style_text_color(days_repeat, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_color(days_repeat, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(days_repeat, LV_OPA_COVER, LV_PART_MAIN);
    lv_dropdown_set_symbol(days_repeat, NULL);

    lv_obj_add_event_cb(days_repeat, [](lv_event_t *e)
    {
        lv_event_code_t code = lv_event_get_code(e);
        lv_obj_t *obj = lv_event_get_target(e);

        if (code = LV_EVENT_READY){
            lv_obj_t *list = lv_dropdown_get_list(obj);
            lv_obj_set_size(list, 180, 120);
            lv_obj_set_style_bg_color(list, lv_color_white(), LV_PART_MAIN);
            lv_obj_set_style_bg_opa(list, LV_OPA_COVER, LV_PART_MAIN);
            lv_obj_set_style_text_font(list, &montserrat_24_regular, LV_PART_MAIN);
            lv_obj_set_style_text_color(list, lv_color_black(), LV_PART_MAIN);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xDDDDDD), LV_PART_SELECTED);
            lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, LV_PART_SELECTED);
            lv_obj_set_style_text_color(obj, lv_color_black(), LV_PART_SELECTED);
            lv_obj_set_style_text_font(obj, &montserrat_24_regular, LV_PART_ITEMS);
            lv_obj_set_style_text_color(obj, lv_color_black(), LV_PART_ITEMS);
        }
    }, LV_EVENT_ALL, NULL);

    //****EDIT DAYS****
    lv_obj_t *btn_rep_days = lv_btn_create(scr);
    lv_obj_set_size(btn_rep_days, 190, 40);
    lv_obj_set_style_bg_color(btn_rep_days, lv_color_make(160, 50, 200), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(btn_rep_days, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(btn_rep_days, 10, LV_PART_MAIN);
    lv_obj_set_pos(btn_rep_days, dd_x+(rep_dd_w-190)/2, rep_row_y+50);

    lv_obj_t *lbl_rep_days = lv_label_create(scr);
    lv_label_set_text(lbl_rep_days, "Editar días");
    lv_obj_set_style_text_font(lbl_rep_days, &montserrat_34_regular, LV_PART_MAIN);
    lv_obj_set_style_text_color(lbl_rep_days, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(lbl_rep_days);

    lv_obj_add_event_cb(btn_rep_days, [](lv_event_t *e)
    {
        repetition_screen(edit_index);
    }, LV_EVENT_CLICKED, NULL);


    //----------- RECORDATORY -------------------------------------------
    const int rec_row_y = 230;
    const int sw_rec_w = 60;
    const int sw_rec_h = 30;

    lv_obj_t *lbl_recordatory = lv_label_create(scr);
    lv_label_set_text(lbl_recordatory, "Recordatorio");
    lv_obj_set_style_text_font(lbl_recordatory, &montserrat_24_regular, LV_PART_MAIN);
    lv_obj_set_style_text_color(lbl_recordatory, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_pos(lbl_recordatory, 10, rec_row_y);

    sw_recordatory = lv_switch_create(scr);
    lv_obj_set_size(sw_recordatory, sw_rec_w, sw_rec_h);
    lv_obj_set_pos(sw_recordatory, right_x(sw_rec_w), rec_row_y-5);
    if (takes[index].recordatory){
        lv_obj_add_state(sw_recordatory, LV_STATE_CHECKED);
    }

    //----------- EARLIER WARNING -------------------------------------------
    const int warn_row_y = 275;
    const int warn_dd_w = 110;
    const int warn_dd_h = 40;

    lv_obj_t *lbl_warning = lv_label_create(scr);
    lv_label_set_text(lbl_warning, "Avisar antes");
    lv_obj_set_style_text_font(lbl_warning, &montserrat_24_regular, LV_PART_MAIN);
    lv_obj_set_style_text_color(lbl_warning, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_pos(lbl_warning, 10, warn_row_y);

    days_warning = lv_dropdown_create(scr);
    lv_dropdown_set_options(days_warning,
        "0 min\n5 min\n10 min\n15 min\n20 min");
    lv_obj_set_size(days_warning, warn_dd_w, warn_dd_h);
    lv_obj_set_pos(days_warning, right_x(warn_dd_w), warn_row_y-5);
    lv_obj_set_style_text_font(days_warning, &montserrat_24_regular, LV_PART_MAIN);
    lv_obj_set_style_text_color(days_warning, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_color(days_warning, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(days_warning, LV_OPA_COVER, LV_PART_MAIN);
    lv_dropdown_set_symbol(days_warning, NULL);
    lv_dropdown_set_selected(days_warning, takes[index].warning_time/5);

    lv_obj_add_event_cb(days_warning, [](lv_event_t *e)
    {
        lv_obj_t *obj = lv_event_get_target(e);
        lv_obj_t *list = lv_dropdown_get_list(obj);
        lv_obj_set_size(list, warn_dd_w, 160);
        lv_obj_set_style_bg_color(list, lv_color_white(), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(list, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_text_font(list, &montserrat_24_regular, LV_PART_MAIN);
        lv_obj_set_style_text_color(list, lv_color_black(), LV_PART_MAIN);
        lv_obj_set_style_bg_color(obj, lv_color_hex(0xDDDDDD), LV_PART_SELECTED);
        lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, LV_PART_SELECTED);
        lv_obj_set_style_text_color(obj, lv_color_black(), LV_PART_SELECTED);
        lv_obj_set_style_text_font(obj, &montserrat_24_regular, LV_PART_ITEMS);
        lv_obj_set_style_text_color(obj, lv_color_black(), LV_PART_ITEMS);

    }, LV_EVENT_ALL, NULL);

    //----------- SAVE -------------------------------------------
    lv_obj_t *btn_save = lv_btn_create(scr);
    lv_obj_set_size(btn_save, 190, 45);
    lv_obj_set_style_bg_color(btn_save, lv_color_make(160, 50, 200), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(btn_save, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(btn_save, 10, LV_PART_MAIN);
    lv_obj_set_pos(btn_save, right_x(190), 320);

    lv_obj_t *lbl_save = lv_label_create(scr);
    lv_label_set_text(lbl_save, "Guardar");
    lv_obj_set_style_text_font(lbl_save, &montserrat_24_regular, LV_PART_MAIN);
    lv_obj_set_style_text_color(lbl_save, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(lbl_save);

    lv_obj_add_event_cb(btn_save, [](lv_event_t *e)
    {
        int hh = lv_roller_get_selected(roller_hours);
        int mm = lv_roller_get_selected(roller_minutes);
        sprintf(takes[edit_index].hour, "%02d:%02d", hh, mm);

        takes[edit_index].recordatory = lv_obj_has_state(sw_recordatory, LV_STATE_CHECKED);
        takes[edit_index].warning_time = lv_dropdown_get_selected(days_warning)*5;

        saveTakes();
        takes_list_screen();
    }, LV_EVENT_CLICKED, NULL);

    //----------- GO BACK -------------------------------------------
    lv_obj_t *btn_back = lv_btn_create(scr);
    lv_obj_set_size(btn_back, 190, 45);
    lv_obj_set_style_bg_color(btn_back, lv_color_make(160, 50, 200), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(btn_back, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(btn_back, 10, LV_PART_MAIN);
    lv_obj_set_pos(btn_back, 10, 320);

    lv_obj_t *lbl_back = lv_label_create(scr);
    lv_label_set_text(lbl_back, "Atrás");
    lv_obj_set_style_text_font(lbl_back, &montserrat_24_regular, LV_PART_MAIN);
    lv_obj_set_style_text_color(lbl_back, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(lbl_back);

    lv_obj_add_event_cb(btn_back, [](lv_event_t *e)
    {
        takes_list_screen();
    }, LV_EVENT_CLICKED, NULL);
}