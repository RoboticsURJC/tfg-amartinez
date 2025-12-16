#include "lvgl_touch.h"
#include <SPI.h>
#include <XPT2046_Touchscreen.h>

#define T_CS 33
#define T_CLK 25
#define T_MISO 39
#define T_MOSI 32

XPT2046_Touchscreen ts(T_CS);
static lv_indev_t *indev_touch = nullptr;

static void touch_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    if (!ts.touched()){
        data->state = LV_INDEV_STATE_REL;
        return;
    }

    TS_Point p = ts.getPoint();

    uint16_t x = map(p.x, 3800, 300, 0, 320);
    uint16_t y = map(p.x, 3685, 148, 0, 240);

    data->state = LV_INDEV_STATE_PR;
    data->point.x = x;
    data->point.y = y;
}

void lvgl_touch_init()
{
    SPI.begin(T_CLK, T_MISO, T_MOSI);
    ts.begin();
    ts.setRotation(1);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touch_read_cb;

    indev_touch = lv_indev_drv_register(&indev_drv);
}