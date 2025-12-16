#include <lvgl.h>
#include <stdlib.h>
#include "qrcodegen.h"
#include "user_interface/wifi_mod.h"

static lv_obj_t *qr_img = nullptr;
static uint8_t qr_data[qrcodegen_BUFFER_LEN_MAX];
static uint8_t temp[qrcodegen_BUFFER_LEN_MAX];

static void generate_wifi_qr(lv_obj_t *parent)
{
    const char *text = "http://192.168.4.1";

    qrcodegen_encodeText(
        text,
        temp,
        qr_data,
        qrcodegen_Ecc_LOW,
        qrcodegen_VERSION_MIN,
        qrcodegen_VERSION_MAX,
        qrcodegen_Mask_AUTO,
        true
    );

    int size = qrcodegen_getSize(qr_data);
    int scale = 4;
    int out_size = size*scale;

    static lv_color_t *buf = nullptr;
    if (!buf){
        buf = (lv_color_t *) malloc(out_size*out_size*sizeof(lv_color_t));
    }

    if (!buf){
        return;
    }

    for (int y=0; y < size; y++){
        for (int x=0; x < size; x++){
            bool black = qrcodegen_getModule(qr_data, x, y);
            lv_color_t c;

            if (black){
                c = lv_color_black();
            } else{
                c = lv_color_white();
            }

            for (int dy = 0; dy < scale; dy++){
                for (int dx = 0; dx < scale; dx++){
                    buf[(y*scale*dy)*out_size+(x*scale*dx)] = c;
                }
            }
        }
    }

    static lv_img_dsc_t img_desc = {
        .header = {
            .cf = LV_IMG_CF_TRUE_COLOR,
            .always_zero = 0,
            .w = 0,
            .h = 0
        },
        .data_size = 0,
        .data = NULL
    };

    img_desc.header.w = out_size;
    img_desc.header.h = out_size;
    img_desc.data_size = out_size*out_size*sizeof(lv_color_t);
    img_desc.data = (const uint8_t *) buf;

    if (!qr_img){
        qr_img = lv_img_create(parent);
    }

    lv_img_set_src(qr_img, &img_desc);
    lv_obj_align(qr_img, LV_ALIGN_CENTER, 0, 10);
}

void show_wifi_screen(lv_obj_t *parent)
{
    lv_obj_clean(parent);

    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, "Escanear QR");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_32, LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);

    generate_wifi_qr(parent);
}