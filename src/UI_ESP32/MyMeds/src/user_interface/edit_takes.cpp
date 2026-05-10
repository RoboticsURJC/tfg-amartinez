#include <lvgl.h>
#include <Arduino.h>
#include "user_interface/pill_takes.h"
#include "storage/takes_storage.h"
#include "storage/medicines_storage.h"

static lv_obj_t *roller_hours;
static lv_obj_t *roller_minutes;
static lv_obj_t *days_repeat;
static lv_obj_t *days_warning;
static lv_obj_t *sw_recordatory;
static lv_obj_t *btn_rep_days;
static lv_obj_t *btn_days[7];
static lv_obj_t *current_screen = NULL;

static lv_obj_t *lbl_recordatory;
static lv_obj_t *lbl_warning;
static lv_obj_t *btn_save;
static lv_obj_t *btn_back;

static lv_obj_t *lbl_med;
static lv_obj_t *dd_medicine;
static lv_obj_t *meds_container;
static const char* med_names[MAX_CATALOG_MEDICINES + 1];

TakeConfig temp_take;
TakeConfig *current_cfg = NULL;

bool editing_existing = false;
int current_index = -1;

static void load_screen(lv_obj_t *scr)
{
    if (current_screen)
        lv_obj_del_async(current_screen);

    current_screen = scr;

    lv_scr_load(scr);
}

static int right_x(int width)
{
    return 320 - width - 10;
}

static void update_layout_positions()
{
    int sel = lv_dropdown_get_selected(days_repeat);

    int base_y = 180;

    if (sel == 2)
        base_y = 260;
    else
        base_y = 180;

    lv_obj_set_pos(lbl_med, 10, base_y);
    lv_obj_set_pos(dd_medicine, right_x(170), base_y - 7);

    int recordatory_y = base_y + 45;

    lv_obj_set_pos(lbl_recordatory, 10, recordatory_y);
    lv_obj_set_pos(sw_recordatory, right_x(60), recordatory_y - 5);

    int warn_y = recordatory_y + 45;

    lv_obj_set_pos(lbl_warning, 10, warn_y);
    lv_obj_set_pos(days_warning, right_x(110), warn_y - 5);

    int btn_y = warn_y + 45;

    lv_obj_set_pos(btn_save, right_x(150), btn_y);
    lv_obj_set_pos(btn_back, 5, btn_y);
}

void set_rollers_init(const char *hour_str)
{
    int hh = 0, mm = 0;

    if (hour_str) {
        sscanf(hour_str, "%d:%d", &hh, &mm);
    }

    if (hh < 0 || hh > 23) {
        hh = 0;
    }

    if (mm < 0 || mm > 59) {
        mm = 0;
    }

    lv_roller_set_selected(roller_hours, hh, LV_ANIM_OFF);
    lv_roller_set_selected(roller_minutes, mm, LV_ANIM_OFF);
}

static void update_days_visibility()
{
    if (!days_repeat) return;

    int sel = lv_dropdown_get_selected(days_repeat);

    bool show = (sel == 2);

    for (int i = 0; i < 7; i++)
    {
        if (!btn_days[i]) continue;

        if (show)
            lv_obj_clear_flag(btn_days[i], LV_OBJ_FLAG_HIDDEN);
        else
            lv_obj_add_flag(btn_days[i], LV_OBJ_FLAG_HIDDEN);
    }
}

static void refresh_days_buttons()
{
    if (!current_cfg) return;

    for (int i = 0; i < 7; i++)
    {
        if (!btn_days[i]) continue;

        if (is_day_active(current_cfg->repeat_mask, i))
            lv_obj_add_state(btn_days[i], LV_STATE_CHECKED);
        else
            lv_obj_clear_state(btn_days[i], LV_STATE_CHECKED);
    }
}

static void refresh_medicines_list()
{
    if (!meds_container || !current_cfg)
        return;

    lv_obj_clean(meds_container);

    for (int i = 0; i < current_cfg->medicine_count; i++) {

        lv_obj_t *row = lv_obj_create(meds_container);

        lv_obj_set_size(row, 260, 35);

        lv_obj_set_style_pad_all(row, 4, 0);

        lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

        // -------- NOMBRE --------

        const char* medName = "Desconocido";

        for (int k = 0; k < medicine_count; k++) {

            if (strcmp(
                medicineCatalog[k].id,
                current_cfg->medicines[i].id
            ) == 0) {

                medName = medicineCatalog[k].name;
                break;
            }
        }

        char txt[64];

        sprintf(
            txt,
            "%s x%d",
            medName,
            current_cfg->medicines[i].quantity
        );

        lv_obj_t *lbl = lv_label_create(row);

        lv_label_set_text(lbl, txt);

        lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 5, 0);

        // -------- BOTÓN DELETE --------

        lv_obj_t *btn_del = lv_btn_create(row);

        lv_obj_set_size(btn_del, 28, 28);

        lv_obj_align(btn_del, LV_ALIGN_RIGHT_MID, 0, 0);

        lv_obj_set_user_data(btn_del, (void*)(intptr_t)i);

        lv_obj_t *lbl_x = lv_label_create(btn_del);

        lv_label_set_text(lbl_x, "X");

        lv_obj_center(lbl_x);

        lv_obj_add_event_cb(btn_del, [](lv_event_t *e){

            int idx = (int)(intptr_t)
                lv_obj_get_user_data(
                    lv_event_get_target(e)
                );

            if (!current_cfg)
                return;

            if (idx >= current_cfg->medicine_count)
                return;

            // reducir cantidad

            if (current_cfg->medicines[idx].quantity > 1) {

                current_cfg->medicines[idx].quantity--;

            } else {

                // eliminar medicamento

                for (int j = idx;
                     j < current_cfg->medicine_count - 1;
                     j++) {

                    current_cfg->medicines[j] =
                        current_cfg->medicines[j + 1];
                }

                current_cfg->medicine_count--;
            }

            refresh_medicines_list();

        }, LV_EVENT_CLICKED, NULL);
    }
}

void edit_takes_screen(int index)
{
    for (int i = 0; i < 7; i++) {
        btn_days[i] = NULL;
    }

    days_repeat = NULL;
    days_warning = NULL;
    sw_recordatory = NULL;

    if (index == -1)
    {
        editing_existing = false;
        memset(&temp_take, 0, sizeof(TakeConfig));
        strcpy(temp_take.hour, "00:00");

        for (int i = 0; i < 7; i++) {
            set_day(temp_take.repeat_mask, i, true);
        }
        
        current_cfg = &temp_take;
        current_index = total_takes;
    } else {
        editing_existing = true;
        current_cfg = &takes[index];
        current_index = index;
    }

    lv_obj_t *scr = lv_obj_create(NULL);
    load_screen(scr);
    lv_obj_set_style_bg_color(scr, lv_color_make(48, 25, 52), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

    char txt_tittle[32];

    if (editing_existing) {
        sprintf(txt_tittle, "Editar Toma %d", current_index + 1);
    } else {
        sprintf(txt_tittle, "Nueva toma");
    }
    
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

    set_rollers_init(current_cfg->hour);

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

    bool all = true;
    bool week = true;

    for (int i=0;i<7;i++)
        if (!is_day_active(current_cfg->repeat_mask, i))
            all = false;

    for (int i=0;i<5;i++)
        if (!is_day_active(current_cfg->repeat_mask, i))
            week = false;

    if (is_day_active(current_cfg->repeat_mask, 5)) week = false;
    if (is_day_active(current_cfg->repeat_mask, 6)) week = false;

    if (all)
        lv_dropdown_set_selected(days_repeat,0);
    else if (week)
        lv_dropdown_set_selected(days_repeat,1);
    else
        lv_dropdown_set_selected(days_repeat,2);

    int sel = lv_dropdown_get_selected(days_repeat);

    if (sel == 0) {
        for (int i = 0; i < 7; i++) {
            set_day(current_cfg->repeat_mask, i, true);
        }
    } else if (sel == 1) {
        for (int i = 0; i < 5; i++) {
            set_day(current_cfg->repeat_mask, i, true);
        }

        set_day(current_cfg->repeat_mask, 5, false);
        set_day(current_cfg->repeat_mask, 6, false);
    }

    lv_obj_add_event_cb(days_repeat, [](lv_event_t *e)
    {
        lv_event_code_t code = lv_event_get_code(e);
        lv_obj_t *obj = lv_event_get_target(e);

        if (code == LV_EVENT_READY)
        {
            lv_obj_t *list = lv_dropdown_get_list(obj);

            lv_obj_set_size(list, 180, 120);

            lv_obj_set_style_bg_color(list,
                                    lv_color_white(),
                                    LV_PART_MAIN);

            lv_obj_set_style_text_color(list,
                                        lv_color_black(),
                                        LV_PART_MAIN);

            lv_obj_set_style_text_font(list, &montserrat_24_regular, LV_PART_MAIN);

            lv_obj_set_style_text_font(obj, &montserrat_24_regular, LV_PART_ITEMS);
        }

        if (code == LV_EVENT_VALUE_CHANGED)
        {
            if (!current_cfg) return;

            int sel = lv_dropdown_get_selected(obj);

            if (sel == 0)
            {
                for (int i = 0; i < 7; i++)
                    set_day(current_cfg->repeat_mask, i, true);

            } else if (sel == 1) {
                for (int i = 0; i < 5; i++)
                    set_day(current_cfg->repeat_mask, i, true);

                set_day(current_cfg->repeat_mask, 5, false);
                set_day(current_cfg->repeat_mask, 6, false);
 
            } else if (sel == 2) {
                bool all = true;
                bool week = true;

                for (int i = 0; i < 7; i++)
                {
                    if (!is_day_active(current_cfg->repeat_mask, i))
                        all = false;
                }

                for (int i = 0; i < 5; i++)
                {
                    if (!is_day_active(current_cfg->repeat_mask, i))
                        week = false;
                }

                if (is_day_active(current_cfg->repeat_mask, 5)) week = false;
                if (is_day_active(current_cfg->repeat_mask, 6)) week = false;

                // Si venimos de TODOS o SEMANA → limpiar
                if (all || week)
                {
                    for (int i = 0; i < 7; i++)
                        set_day(current_cfg->repeat_mask, i, false);
                }
            }

            refresh_days_buttons();
            update_days_visibility();
            update_layout_positions();
        }

    }, LV_EVENT_ALL, NULL);

    //----------- DAYS REPEAT BUTTONS------------------------------------
    const char* days[7] = {"L","M","X","J","V","S","D"};
    for (int i=0; i < 7; i++){
        int x, y;

        if (i < 5) {
            x = 40 + i*45;
            y = rep_row_y + 45;
        } else {
            x = 110 + (i-5)*45;
            y = rep_row_y + 90;
        }

        btn_days[i] = lv_btn_create(scr);
        lv_obj_set_size(btn_days[i], 36, 36);
        lv_obj_align(btn_days[i], LV_ALIGN_TOP_LEFT, x, y);
        lv_obj_set_style_bg_color(btn_days[i], lv_color_make(160, 50, 200), LV_PART_MAIN);
        lv_obj_t * lbl_btn = lv_label_create(btn_days[i]);
        lv_label_set_text(lbl_btn, days[i]);
        lv_obj_center(lbl_btn);

        if (is_day_active(current_cfg->repeat_mask, i)) {
            lv_obj_add_state(btn_days[i], LV_STATE_CHECKED);
        }

        lv_obj_set_user_data(btn_days[i], (void*)(intptr_t)i);
        lv_obj_add_event_cb(btn_days[i], [](lv_event_t *e){
            lv_obj_t *obj = lv_event_get_target(e);
            int day = (int)(intptr_t)lv_obj_get_user_data(obj);
            bool active = is_day_active(current_cfg->repeat_mask, day);
            set_day(current_cfg->repeat_mask, day, !active);

            if (is_day_active(current_cfg->repeat_mask, day)){
                lv_obj_add_state(lv_event_get_target(e), LV_STATE_CHECKED);
            } else{
                lv_obj_clear_state(lv_event_get_target(e), LV_STATE_CHECKED);
            }
        }, LV_EVENT_CLICKED, NULL);
    }

    update_days_visibility();

    // ---------- MEDICAMENTO ----------

    lbl_med = lv_label_create(scr);
    lv_label_set_text(lbl_med, "Medicina");
    lv_obj_set_style_text_font(lbl_med, &lv_font_montserrat_22, LV_PART_MAIN);
    lv_obj_set_style_text_color(lbl_med, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_pos(lbl_med, 10, 245);

    dd_medicine = lv_dropdown_create(scr);
    lv_obj_set_pos(dd_medicine, 120, 250);
    lv_obj_set_size(dd_medicine, 170, 40);

    // estilo dropdown
    lv_obj_set_style_text_font(
        dd_medicine,
        &lv_font_montserrat_22,
        LV_PART_MAIN
    );

    lv_obj_set_style_text_color(
        dd_medicine,
        lv_color_black(),
        LV_PART_MAIN
    );

    lv_obj_set_style_bg_color(
        dd_medicine,
        lv_color_white(),
        LV_PART_MAIN
    );

    lv_obj_set_style_bg_opa(
        dd_medicine,
        LV_OPA_COVER,
        LV_PART_MAIN
    );

    lv_dropdown_set_symbol(dd_medicine, NULL);

    // construir opciones
    static char med_options[256];
    med_options[0] = '\0';

    for (int i = 0; i < medicine_count; i++) {

        strcat(med_options, medicineCatalog[i].name);

        if (i < medicine_count - 1) {
            strcat(med_options, "\n");
        }
    }

    lv_dropdown_set_options(dd_medicine, med_options);

    // ---------- CONTENEDOR VISUAL ----------

    meds_container = lv_obj_create(scr);

    lv_obj_set_size(meds_container, 280, 90);
    lv_obj_set_pos(meds_container, 20, 300);

    lv_obj_set_style_pad_all(meds_container, 5, 0);
    lv_obj_set_style_pad_row(meds_container, 5, 0);

    lv_obj_set_scroll_dir(
        meds_container,
        LV_DIR_VER
    );

    lv_obj_set_flex_flow(
        meds_container,
        LV_FLEX_FLOW_COLUMN
    );

    // PRESELECCIÓN
    if (editing_existing && current_cfg->medicine_count > 0) {

        for (int i = 0; i < medicine_count; i++) {

            if (strcmp(
                medicineCatalog[i].id,
                current_cfg->medicines[0].id
            ) == 0) {

                lv_dropdown_set_selected(dd_medicine, i);
                break;
            }
        }
    }

    // refrescar lista visual inicial
    refresh_medicines_list();

    // EVENTOS
    lv_obj_add_event_cb(dd_medicine, [](lv_event_t *e){

        lv_event_code_t code = lv_event_get_code(e);
        lv_obj_t *obj = lv_event_get_target(e);

        // estilo lista desplegable
        if (code == LV_EVENT_READY)
        {
            lv_obj_t *list = lv_dropdown_get_list(obj);

            lv_obj_set_size(list, 180, 120);

            lv_obj_set_style_bg_color(
                list,
                lv_color_white(),
                LV_PART_MAIN
            );

            lv_obj_set_style_text_color(
                list,
                lv_color_black(),
                LV_PART_MAIN
            );

            lv_obj_set_style_text_font(
                list,
                &lv_font_montserrat_18,
                LV_PART_MAIN
            );

            lv_obj_set_style_text_font(
                obj,
                &lv_font_montserrat_18,
                LV_PART_ITEMS
            );
        }

        // selección medicamento
        if (code == LV_EVENT_VALUE_CHANGED)
        {
            int sel = lv_dropdown_get_selected(obj);

            if (sel >= medicine_count)
                return;

            const char* selectedId =
                medicineCatalog[sel].id;

            // buscar si ya existe
            for (int i = 0;
                i < current_cfg->medicine_count;
                i++)
            {
                if (strcmp(
                    current_cfg->medicines[i].id,
                    selectedId
                ) == 0)
                {
                    current_cfg->medicines[i].quantity++;

                    refresh_medicines_list();

                    Serial.print("Cantidad aumentada: ");
                    Serial.println(
                        medicineCatalog[sel].name
                    );

                    return;
                }
            }

            // evitar overflow
            if (current_cfg->medicine_count >=
                MAX_MEDICINES_PER_TAKE)
                return;

            int idx = current_cfg->medicine_count;

            strcpy(
                current_cfg->medicines[idx].id,
                selectedId
            );

            current_cfg->medicines[idx].quantity = 1;

            current_cfg->medicine_count++;

            refresh_medicines_list();

            Serial.print("Medicamento añadido: ");
            Serial.println(
                medicineCatalog[sel].name
            );
        }

    }, LV_EVENT_ALL, NULL);
    //----------- RECORDATORY -------------------------------------------
    const int rec_row_y = 305;
    const int sw_rec_w = 60;
    const int sw_rec_h = 30;

    lbl_recordatory = lv_label_create(scr);
    lv_label_set_text(lbl_recordatory, "Recordatorio");
    lv_obj_set_style_text_font(lbl_recordatory, &montserrat_24_regular, LV_PART_MAIN);
    lv_obj_set_style_text_color(lbl_recordatory, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_pos(lbl_recordatory, 10, rec_row_y);

    sw_recordatory = lv_switch_create(scr);
    lv_obj_set_size(sw_recordatory, sw_rec_w, sw_rec_h);
    lv_obj_set_pos(sw_recordatory, right_x(sw_rec_w), rec_row_y-5);
    if (current_cfg->recordatory){
        lv_obj_add_state(sw_recordatory, LV_STATE_CHECKED);
    }

    //----------- EARLIER WARNING -------------------------------------------
    const int warn_row_y = 350;
    const int warn_dd_w = 110;
    const int warn_dd_h = 40;

    lbl_warning = lv_label_create(scr);
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
    lv_dropdown_set_selected(days_warning, current_cfg->warning_time/5);

    lv_obj_add_event_cb(days_warning, [](lv_event_t *e)
    {
        lv_event_code_t code = lv_event_get_code(e);
        lv_obj_t *obj = lv_event_get_target(e);

        if (code == LV_EVENT_READY) {
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
        }
        
        if (code == LV_EVENT_VALUE_CHANGED) {
            if (!current_cfg) {
                return;
            }

            uint16_t sel = lv_dropdown_get_selected(obj);
            current_cfg->warning_time = sel*5;
        }

    }, LV_EVENT_ALL, NULL);

    //----------- SAVE -------------------------------------------
    btn_save = lv_btn_create(scr);
    lv_obj_set_size(btn_save, 150, 45);
    lv_obj_set_style_bg_color(btn_save, lv_color_make(160, 50, 200), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(btn_save, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(btn_save, 10, LV_PART_MAIN);
    lv_obj_set_pos(btn_save, right_x(150), 395);

    lv_obj_t *lbl_save = lv_label_create(btn_save);
    lv_label_set_text(lbl_save, "Guardar");
    lv_obj_set_style_text_font(lbl_save, &montserrat_24_regular, LV_PART_MAIN);
    lv_obj_set_style_text_color(lbl_save, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(lbl_save);

    lv_obj_add_event_cb(btn_save, [](lv_event_t *e)
    {
        int hh = lv_roller_get_selected(roller_hours);
        int mm = lv_roller_get_selected(roller_minutes);

        sprintf(current_cfg->hour, "%02d:%02d", hh, mm);

        current_cfg->recordatory = lv_obj_has_state(sw_recordatory, LV_STATE_CHECKED);
        current_cfg->warning_time = lv_dropdown_get_selected(days_warning)*5;

        if (!editing_existing) {
            if (total_takes < MAX_TAKES) {
                takes[total_takes] = temp_take;
                total_takes++;
            } else {
                Serial.println("MAX_TAKES reached");
            }
        }
        
        saveTakes();
        takes_list_screen();
    }, LV_EVENT_CLICKED, NULL);

    //----------- GO BACK -------------------------------------------
    btn_back = lv_btn_create(scr);
    lv_obj_set_size(btn_back, 150, 45);
    lv_obj_set_style_bg_color(btn_back, lv_color_make(160, 50, 200), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(btn_back, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(btn_back, 10, LV_PART_MAIN);
    lv_obj_set_pos(btn_back, 5, 395);

    lv_obj_t *lbl_back = lv_label_create(btn_back);
    lv_label_set_text(lbl_back, "Atrás");
    lv_obj_set_style_text_font(lbl_back, &montserrat_24_regular, LV_PART_MAIN);
    lv_obj_set_style_text_color(lbl_back, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(lbl_back);

    lv_obj_add_event_cb(btn_back, [](lv_event_t *e)
    {
        takes_list_screen();
    }, LV_EVENT_CLICKED, NULL);

    update_days_visibility();
    update_layout_positions();
}