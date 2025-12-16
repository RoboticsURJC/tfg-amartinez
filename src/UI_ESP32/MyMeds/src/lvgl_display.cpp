#include <lvgl.h>
#include <TFT_eSPI.h>
#include "lvgl_touch.h"

TFT_eSPI tft;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[320*20];

void my_disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(drv);
}

void lvgl_begin()
{
    lv_init();

    tft.begin();
    tft.setRotation(3);

    #ifdef TFT_BL
        pinMode(TFT_BL, OUTPUT);
        digitalWrite(TFT_BL, HIGH);
    #endif

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, 320*20);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 320;
    disp_drv.ver_res = 240;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.full_refresh = 1;
    lv_disp_drv_register(&disp_drv);

    lvgl_touch_init();
}

