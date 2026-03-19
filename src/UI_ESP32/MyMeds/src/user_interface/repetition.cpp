#include <lvgl.h>
#include "user_interface/pill_takes.h"

static int repetition_index;
static lv_obj_t *btn_days[7];
static bool temp_repeat[7];

extern TakeConfig *current_cfg;
extern int current_index;
extern bool editing_existing;
extern TakeConfig temp_take;
extern TakeConfig takes[];

void repetition_screen(int index)
{
    current_index = index;

    if (editing_existing)
        current_cfg = &takes[index];
    else
        current_cfg = &temp_take;
        
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_scr_load(scr);
    lv_obj_set_style_bg_color(scr, lv_color_make(48, 25, 52), 0);
    lv_obj_t * tittle = lv_label_create(scr);
    lv_label_set_text(tittle, "Repetir tomas");
    lv_obj_set_style_text_color(tittle, lv_color_white(), 0);
    lv_obj_align(tittle, LV_ALIGN_TOP_MID, 0, 10);

    const char* days[7] = {"L","M","X","J","V","S","D"};

    for (int i=0; i < 7; i++){
        int x, y;

        if (i < 5) {
            x = 45 + i*50;
            y = 70;
        } else {
            x = 115 + (i-5)*50;
            y = 120;
        }

        btn_days[i] = lv_btn_create(scr);
        lv_obj_set_size(btn_days[i], 40, 40);
        lv_obj_align(btn_days[i], LV_ALIGN_TOP_LEFT, 45+i*50, 70);
        lv_obj_set_style_bg_color(btn_days[i], lv_color_make(160, 50, 200), LV_PART_MAIN);
        lv_obj_t * lbl_btn = lv_label_create(btn_days[i]);
        lv_label_set_text(lbl_btn, days[i]);
        lv_obj_center(lbl_btn);

        if (current_cfg->repeat[i]) {
            lv_obj_add_state(btn_days[i], LV_STATE_CHECKED);
        }

        lv_obj_set_user_data(btn_days[i], (void*)i);
        lv_obj_add_event_cb(btn_days[i], [](lv_event_t *e){
            lv_obj_t *obj = lv_event_get_target(e);
            int day = (int)lv_obj_get_user_data(lv_event_get_target(e));
            current_cfg->repeat[day] = !current_cfg->repeat[day];

            if (current_cfg->repeat[day]){
                lv_obj_add_state(lv_event_get_target(e), LV_STATE_CHECKED);
            } else{
                lv_obj_clear_state(lv_event_get_target(e), LV_STATE_CHECKED);
            }
        }, LV_EVENT_CLICKED, NULL);
    }

    lv_obj_t *btn_save = lv_btn_create(scr);
    lv_obj_set_size(btn_save, 140, 45);
    lv_obj_set_style_bg_color(btn_save, lv_color_make(160, 50, 200), LV_PART_MAIN);
    lv_obj_align(btn_save, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_t * lbl_save = lv_label_create(btn_save);
    lv_label_set_text(lbl_save, "Guardar");
    lv_obj_set_style_text_font(lbl_save, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_center(lbl_save);

    lv_obj_add_event_cb(btn_save, [](lv_event_t *e){
        int idx = current_index;
        lv_async_call([](void *p){
            edit_takes_screen((int)p);
        }, (void*)idx);
    }, LV_EVENT_CLICKED, NULL);
}