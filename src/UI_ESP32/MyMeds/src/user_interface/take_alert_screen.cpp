#include "user_interface/take_alert_screen.h"

#include <Arduino.h>
#include <lvgl.h>

#include "actuators/dose_actuators.h"
#include "idle_manager.h"
#include "storage/medicines_storage.h"
#include "user_interface/pill_takes.h"

static lv_obj_t *alertOverlay = nullptr;
static bool previousIdleBlock = false;

static const char *findMedicineName(const char *id)
{
    for (int i = 0; i < medicine_count; i++)
    {
        if (strcmp(medicineCatalog[i].id, id) == 0)
        {
            return medicineCatalog[i].name;
        }
    }

    return "Medicamento";
}

static void closeAlert(lv_event_t *event)
{
    (void) event;

    doseActuatorsAcknowledge();
    block_idle_timeout = previousIdleBlock;
    last_touch_time = millis();
    idle_timeout_triggered = false;

    if (alertOverlay != nullptr)
    {
        lv_obj_del(alertOverlay);
        alertOverlay = nullptr;
    }
}

void show_take_alert(int takeIndex)
{
    if (takeIndex < 0 || takeIndex >= total_takes || alertOverlay != nullptr)
    {
        return;
    }

    previousIdleBlock = block_idle_timeout;
    block_idle_timeout = true;

    alertOverlay = lv_obj_create(lv_layer_top());
    lv_obj_set_size(alertOverlay, 320, 240);
    lv_obj_set_pos(alertOverlay, 0, 0);
    lv_obj_set_style_bg_color(alertOverlay, lv_color_make(48, 25, 52), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(alertOverlay, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(alertOverlay, 0, LV_PART_MAIN);
    lv_obj_clear_flag(alertOverlay, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title = lv_label_create(alertOverlay);
    lv_label_set_text(title, "Hora de la toma");
    lv_obj_set_style_text_color(title, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 8);

    String details = String("Programada: ") + takes[takeIndex].hour;

    for (int i = 0; i < takes[takeIndex].medicine_count; i++)
    {
        details += "\n";
        details += findMedicineName(takes[takeIndex].medicines[i].id);
        details += " x";
        details += (int)takes[takeIndex].medicines[i].quantity;
    }

    lv_obj_t *detailLabel = lv_label_create(alertOverlay);
    lv_label_set_text(detailLabel, details.c_str());
    lv_obj_set_width(detailLabel, 280);
    lv_obj_set_style_text_align(detailLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_color(detailLabel, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(detailLabel, &lv_font_montserrat_18, LV_PART_MAIN);
    lv_obj_align(detailLabel, LV_ALIGN_TOP_MID, 0, 50);

    lv_obj_t *button = lv_btn_create(alertOverlay);
    lv_obj_set_size(button, 220, 52);
    lv_obj_set_style_bg_color(button, lv_color_make(160, 50, 200), LV_PART_MAIN);
    lv_obj_align(button, LV_ALIGN_BOTTOM_MID, 0, -12);
    lv_obj_add_event_cb(button, closeAlert, LV_EVENT_CLICKED, nullptr);

    lv_obj_t *buttonLabel = lv_label_create(button);
    lv_label_set_text(buttonLabel, "Aceptar");
    lv_obj_set_style_text_font(buttonLabel, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_center(buttonLabel);
}

bool takeAlertVisible()
{
    return alertOverlay != nullptr;
}
