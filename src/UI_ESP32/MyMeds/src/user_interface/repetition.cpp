#include <lvgl.h>
#include "user_interface/pill_takes.h"

static int repetition_index;
static lv_obj_t *btn_days[7];
static bool temp_repeat[7];

void repetition_screen(int index)
{
    repetition_index = index;

    lv_obj_t *scr = lv_obj_create(NULL);
    lv_scr_load(scr);
    lv_label_set_text(lv_label_create(scr), "Repetición semanal");

    const char* days[7] = {"L","M","X","J","V","S","D"};

    for (int i=0; i < 7; i++){
        temp_repeat[i] = takes[index].repeat[i];
    }

    for ( int i=0; i < 7; i++){
        btn_days[i] = lv_btn_create(scr);
        lv_obj_set_size(btn_days[i], 38, 38);
        lv_obj_align(btn_days[i], LV_ALIGN_TOP_LEFT, 10+i*42, 60);
        lv_label_set_text(lv_label_create(btn_days[i]), days[i]);

        if (temp_repeat[i]){
            lv_obj_add_state(btn_days[i], LV_STATE_CHECKED);
        }

        lv_obj_set_user_data(btn_days[i], (void*)i);
        lv_obj_add_event_cb(btn_days[i], [](lv_event_t *e){
            int day = (int)lv_obj_get_user_data(lv_event_get_target(e));
            temp_repeat[day] = !temp_repeat[day];

            if (temp_repeat[day]){
                lv_obj_add_state(lv_event_get_target(e), LV_STATE_CHECKED);
            } else{
                lv_obj_clear_state(lv_event_get_target(e), LV_STATE_CHECKED);
            }
        }, LV_EVENT_CLICKED, NULL);
    }

    lv_obj_t *btn_save = lv_btn_create(scr);
    lv_obj_set_size(btn_save, 120, 38);
    lv_obj_align(btn_save, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_label_set_text(lv_label_create(btn_save), "Guardar");

    lv_obj_add_event_cb(btn_save, [](lv_event_t *e){
        for (int i = 0; i < 7; i++){
            takes[repetition_index].repeat[i] = temp_repeat[i];
        }

        edit_takes_screen(repetition_index);
    }, LV_EVENT_CLICKED, NULL);
}