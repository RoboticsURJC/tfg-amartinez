#include <lvgl.h>
#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>

#include "user_interface/settings_screen.h"
#include "user_interface/change_pin_screen.h"
#include "user_interface/home_mod.h"
#include "user_interface/clock_mod.h"

static lv_obj_t *message_label = nullptr;
static bool reset_pending = false;

static void show_settings_message(const char *text)
{
    if (!message_label) {
        return;
    }

    lv_label_set_text(message_label, text);
    lv_obj_clear_flag(message_label, LV_OBJ_FLAG_HIDDEN);
}

static void create_section_label(lv_obj_t *parent, const char *text, int y)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_font(label, &montserrat_24_regular, 0);
    lv_obj_set_pos(label, 28, y);
}

static lv_obj_t *create_action_button(lv_obj_t *parent, const char *text, int y)
{
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, 285, 50);
    lv_obj_set_pos(btn, 18, y);
    lv_obj_set_style_bg_color(btn, lv_color_make(160, 50, 200), LV_PART_MAIN);
    lv_obj_set_style_radius(btn, 12, LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_font(label, &montserrat_24_regular, 0);
    lv_obj_center(label);

    return btn;
}

static void restart_device_timer_cb(lv_timer_t *timer)
{
    lv_timer_del(timer);
    ESP.restart();
}

static void reset_factory_timer_cb(lv_timer_t *timer)
{
    lv_timer_del(timer);

    Preferences prefs;

    prefs.begin("sys", false);
    prefs.clear();
    prefs.end();

    prefs.begin("takes", false);
    prefs.clear();
    prefs.end();

    prefs.begin("meds", false);
    prefs.clear();
    prefs.end();

    ESP.restart();
}

void show_settings_screen()
{
    lv_obj_clean(lv_scr_act());
    reset_pending = false;

    lv_obj_set_style_bg_color(lv_scr_act(),lv_color_make(48, 25, 52), LV_PART_MAIN);
    lv_obj_t *title = lv_label_create(lv_scr_act());
    lv_label_set_text(title,"Ajustes");
    lv_obj_set_style_text_color(title,lv_color_white(),0);
    lv_obj_set_style_text_font(title,&lv_font_montserrat_24,0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 14);

    lv_obj_t *status_box = lv_obj_create(lv_scr_act());
    lv_obj_set_size(status_box, 265, 118);
    lv_obj_set_pos(status_box, 28, 54);
    lv_obj_set_style_bg_color(status_box, lv_color_make(67, 37, 73), LV_PART_MAIN);
    lv_obj_set_style_border_width(status_box, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(status_box, 12, LV_PART_MAIN);
    lv_obj_clear_flag(status_box, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *status_title = lv_label_create(status_box);
    lv_label_set_text(status_title, "Estado del dispensador");
    lv_obj_set_style_text_color(status_title, lv_color_white(), 0);
    lv_obj_set_style_text_font(status_title, &lv_font_montserrat_18, 0);
    lv_obj_align(status_title, LV_ALIGN_TOP_MID, 0, 6);

    lv_obj_t *status_value = lv_label_create(status_box);
    lv_label_set_text(status_value, WiFi.status() == WL_CONNECTED ? "Conectado" : "Desconectado");
    lv_obj_set_style_text_color(
        status_value,
        WiFi.status() == WL_CONNECTED ? lv_palette_main(LV_PALETTE_GREEN) : lv_palette_main(LV_PALETTE_RED),
        0
    );
    lv_obj_set_style_text_font(status_value, &montserrat_24_regular, 0);
    lv_obj_align(status_value, LV_ALIGN_TOP_MID, 0, 36);

    lv_obj_t *status_detail = lv_label_create(status_box);
    String detail = "IP: ";
    detail += WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString() : String("--");
    lv_label_set_text(status_detail, detail.c_str());
    lv_obj_set_style_text_color(status_detail, lv_color_make(230, 220, 235), 0);
    lv_obj_set_style_text_font(status_detail, &lv_font_montserrat_24, 0);
    lv_obj_align(status_detail, LV_ALIGN_TOP_MID, 0, 68);

    create_section_label(lv_scr_act(), "Seguridad", 188);
    lv_obj_t *btnPin = create_action_button(lv_scr_act(), "Cambiar PIN", 222);
    lv_obj_add_event_cb(btnPin,[](lv_event_t *e)
        {
            reset_pending = false;
            show_change_pin_screen();
        },LV_EVENT_CLICKED,NULL
    );

    create_section_label(lv_scr_act(), "Sincronizacion", 280);
    lv_obj_t *btnSync = create_action_button(lv_scr_act(), "Sincronizar\nconfiguracion", 314);
    lv_obj_set_size(btnSync, 285, 68);
    lv_obj_add_event_cb(btnSync, [](lv_event_t *e)
        {
            reset_pending = false;
            clock_sync();
            show_settings_message("Configuracion sincronizada");
        }, LV_EVENT_CLICKED, NULL
    );

    lv_obj_t *btnStatus = create_action_button(lv_scr_act(), "Actualizar estado", 394);
    lv_obj_add_event_cb(btnStatus, [](lv_event_t *e)
        {
            show_settings_screen();
        }, LV_EVENT_CLICKED, NULL
    );

    create_section_label(lv_scr_act(), "Sistema", 456);
    lv_obj_t *btnRestart = create_action_button(lv_scr_act(), "Reiniciar dispositivo", 490);
    lv_obj_add_event_cb(btnRestart, [](lv_event_t *e)
        {
            reset_pending = false;
            show_settings_message("Reiniciando dispositivo...");
            lv_timer_create(restart_device_timer_cb, 1200, NULL);
        }, LV_EVENT_CLICKED, NULL
    );

    lv_obj_t *btnReset = create_action_button(lv_scr_act(), "Restablecer fabrica", 548);
    lv_obj_set_style_bg_color(btnReset, lv_color_make(185, 55, 95), LV_PART_MAIN);
    lv_obj_add_event_cb(btnReset, [](lv_event_t *e)
        {
            if (!reset_pending) {
                reset_pending = true;
                show_settings_message("Pulsa otra vez para restablecer");
                return;
            }

            show_settings_message("Restableciendo fabrica...");
            lv_timer_create(reset_factory_timer_cb, 1400, NULL);
        }, LV_EVENT_CLICKED, NULL
    );

    message_label = lv_label_create(lv_scr_act());
    lv_label_set_text(message_label, "");
    lv_obj_set_style_text_color(message_label, lv_color_make(230, 220, 235), 0);
    lv_obj_set_style_text_font(message_label, &lv_font_montserrat_14, 0);
    lv_obj_align(message_label, LV_ALIGN_TOP_MID, 0, 598);
    lv_obj_add_flag(message_label, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *btnBack = create_action_button(lv_scr_act(), "Volver", 624);
    lv_obj_add_event_cb(btnBack,[](lv_event_t *e)
        {
            reset_pending = false;
            show_home_screen();
        },LV_EVENT_CLICKED,NULL
    );
}
