#include <WiFi.h>
#include <time.h>
#include "user_interface/home_mod.h"
#include "user_interface/clock_mod.h"

#include "fonts/montserrat_100_bold.h"
#include "fonts/montserrat_46_bold.h"

lv_obj_t *lbl_time = nullptr;
lv_obj_t *lbl_day = nullptr;
static lv_obj_t *msg_lbl = nullptr;

const char *days[] = {
    "Domingo", "Lunes", "Martes", "Miércoles",
    "Jueves", "Viernes", "Sábado"
};

static void btn_event_cb(lv_event_t *e)
{
    show_home_screen();
}

void show_clock_screen(lv_obj_t *parent)
{
    lv_obj_clean(parent);
    lv_obj_set_style_bg_color(parent, lv_color_make(48, 25, 52), LV_PART_MAIN);

    clock_init(parent);

    lv_obj_t *btn_init = lv_btn_create(parent);
    lv_obj_set_size(btn_init, 300, 90);
    lv_obj_set_style_bg_color(btn_init, lv_color_make(160, 50, 200), LV_PART_MAIN);
    lv_obj_set_style_radius(btn_init, 20, LV_PART_MAIN);
    lv_obj_align(btn_init, LV_ALIGN_BOTTOM_MID, 0, -10);

    lv_obj_t *lbl_init = lv_label_create(parent);
    lv_label_set_text(lbl_init, "INICIO");
    lv_obj_set_style_text_font(lbl_init, &lv_font_montserrat_40, LV_PART_MAIN);
    lv_obj_set_style_text_color(lbl_init, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(lbl_init);

    lv_obj_add_event_cb(btn_init, btn_event_cb, LV_EVENT_CLICKED, NULL);
}

void clock_sync()
{
    configTzTime("CET-1CEST,M3.5.0/02:00,M10.5.0/03:00", "pool.ntp.org");
    struct tm info;
    const unsigned long start = millis();
    const unsigned long timeout = 10000;

    while (!getLocalTime(&info)){
        if ((millis()-start) > timeout){
            Serial.println("[WARN] NTP timeout - hora no sincronizada");
            return;
        }

        Serial.print(".");
        delay(500);
    }

    Serial.println("[OK] Hora sincronizada");
}

void clock_init(lv_obj_t *parent){
    //------ HOUR ---------------
    lbl_time = lv_label_create(parent);
    lv_obj_set_style_text_font(lbl_time, &montserrat_100_bold, LV_PART_MAIN);
    lv_obj_set_style_text_color(lbl_time, lv_color_make(188, 135, 197), LV_PART_MAIN);
    lv_obj_align(lbl_time, LV_ALIGN_TOP_MID, 0, -10);

    //------ DAY ---------------
    lbl_day = lv_label_create(parent);
    lv_obj_set_style_text_font(lbl_day, &montserrat_46_bold, LV_PART_MAIN);
    lv_obj_set_style_text_color(lbl_day, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(lbl_day, LV_ALIGN_TOP_MID, 5, 85);
}

void clock_update()
{
    if (!lbl_time || !lbl_day){
        return;
    }

    struct tm info;
    if (!getLocalTime(&info)){
        return;
    }

    char hour_buf[16];
    snprintf(hour_buf, sizeof(hour_buf), "%02d:%02d", info.tm_hour, info.tm_min);
    lv_label_set_text(lbl_time, hour_buf);

    char day_buf[16];
    snprintf(day_buf, sizeof(day_buf), "%s", days[info.tm_wday]);
    lv_label_set_text(lbl_day, day_buf);
}

void update_clock_task(lv_timer_t *timer)
{
    (void) timer;
    clock_update();
}