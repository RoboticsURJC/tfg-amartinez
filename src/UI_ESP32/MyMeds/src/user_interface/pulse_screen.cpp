#include "user_interface/pulse_screen.h"
#include "user_interface/home_mod.h"
#include "sensors/pulse_sensor.h"
#include "storage/pulse_history_storage.h"
#include "idle_manager.h"
#include <lvgl.h>
#include <Arduino.h>

static lv_obj_t *bpm_label = nullptr;
static lv_obj_t *status_label = nullptr;
static lv_timer_t *pulse_timer = nullptr;

LV_IMG_DECLARE(heart);

static void pulse_update_cb(lv_timer_t *timer)
{
    if (
        bpm_label == nullptr ||
        status_label == nullptr
    )
    {
        return;
    }

    if (!pulseFingerDetected())
    {
        lv_label_set_text(status_label,"Coloque el dedo");
        lv_label_set_text(bpm_label,"-- BPM");

        return;
    }

    if (!pulseMeasurementReady())
    {
        char txt[32];

        snprintf(
            txt,
            sizeof(txt),
            "Midiendo %d:%d",
            pulseGetSamples(),
            pulseGetRequiredSamples()
        );

        lv_label_set_text(
            status_label,
            txt
        );

        lv_label_set_text(
            bpm_label,
            "-- BPM"
        );

        return;
    }

    char bpmText[16];

    snprintf(bpmText,sizeof(bpmText),"%d BPM",pulseGetBpm());
    lv_label_set_text(bpm_label,bpmText);

    if (
        pulseGetSamples() <
        pulseGetRequiredSamples()
    )
    {
        char txt[32];

        snprintf(
            txt,
            sizeof(txt),
            "Midiendo %d:%d",
            pulseGetSamples(),
            pulseGetRequiredSamples()
        );

        lv_label_set_text(
            status_label,
            txt
        );
    }
    else
    {
        lv_label_set_text(status_label,"");
    }
}

static void back_btn_event_cb(lv_event_t *e)
{
    block_idle_timeout = false;

    last_touch_time = millis();
    idle_timeout_triggered = false;

    if (pulse_timer)
    {
        lv_timer_del(pulse_timer);
        pulse_timer = nullptr;
    }

    bpm_label = nullptr;
    status_label = nullptr;

    if (pulseFullMeasurementReady()) {
        int bpm = pulseGetFullBufferBpm();

        Serial.print("Guardando media de pulso: ");
        Serial.println(bpm);

        bool ok = savePulseAverage(bpm);

        Serial.print("Guardado SD: ");
        Serial.println(ok ? "OK" : "ERROR");
    }

    pulseSensorStop();
    show_home_screen();
    Serial.println("Home cargado");
}

void show_pulse_screen(lv_obj_t *parent)
{
    pulseSensorStart();
    block_idle_timeout = true;

    lv_obj_clean(parent);
    lv_obj_set_style_bg_color(parent,lv_color_make(48,25,52),LV_PART_MAIN);

    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text(title,"Pulsómetro");
    lv_obj_set_style_text_color(title,lv_color_white(),LV_PART_MAIN);
    lv_obj_set_style_text_font(title,&montserrat_34_regular,LV_PART_MAIN);
    lv_obj_align(title,LV_ALIGN_TOP_MID,0,20);

    //---------------------------------

    lv_obj_t *heart_img = lv_img_create(parent);
    lv_img_set_src(heart_img, &heart);
    lv_obj_align(heart_img,LV_ALIGN_TOP_MID,0,125);

    //---------------------------------

    bpm_label = lv_label_create(parent);
    lv_label_set_text(bpm_label,"-- BPM");
    lv_obj_set_style_text_color(bpm_label,lv_color_white(),LV_PART_MAIN);
    lv_obj_set_style_text_font(bpm_label,&lv_font_montserrat_34,LV_PART_MAIN);
    lv_obj_align(bpm_label,LV_ALIGN_CENTER,0,-20);

    //---------------------------------

    status_label = lv_label_create(parent);
    lv_label_set_text(status_label,"Coloque el dedo");
    lv_obj_set_style_text_color(status_label,lv_color_white(),LV_PART_MAIN);
    lv_obj_set_style_text_font(status_label,&montserrat_34_regular,LV_PART_MAIN);
    lv_obj_align(status_label, LV_ALIGN_TOP_MID, 0, 190);

    //---------------------------------

    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, 180, 60);
    lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 250);
    lv_obj_set_style_bg_color(btn,lv_color_make(160, 50, 200),LV_PART_MAIN);
    lv_obj_add_event_cb(btn, back_btn_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *lbl = lv_label_create(btn);
    lv_label_set_text(lbl,"Volver");
    lv_obj_center(lbl);

    pulse_timer = lv_timer_create(pulse_update_cb, 500, NULL);
}
