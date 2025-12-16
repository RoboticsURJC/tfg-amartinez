#ifndef LV_CONF_H
#define LV_CONF_H

//#define LV_CONF_INCLUDE_SIMPLE 1

#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 0

#define LV_MEM_CUSTOM 0
#define LV_MEM_SIZE (48U * 1024U)

#define LV_ENABLE_GC 0

#define LV_TICK_CUSTOM 1
#define LV_TICK_CUSTOM_INCLUDE "Arduino.h"
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())

#define LV_USE_LOG 0

#define LV_USE_GPU_STM32_DNA2D 0
#define LV_USE_GPU_NXP_PXP 0
#define LV_USE_GPU_NXP_VG_LITE 0

#define LV_USE__USER_DATA 1

#define LV_USE_IMG 1
#define LV_USE_ARC 1
#define LV_USE_BAR 1 
#define LV_USE_BTN 1
#define LV_USE_LABEL 1
#define LV_USE_LINE 1
#define LV_USE_LED 1
#define LV_USE_SLIDER 1
#define LV_USE_SWITCH 1
#define LV_USE_ROLLER 1

#define LV_FONT_MONTSERRAT_32 1
#define LV_FONT_MONTSERRAT_40 1

#define LV_FONT_UNSCII_8 0

#define LV_TXT_BREAK_CHARS " ,.;:-_"
#define LV_TXT_LINE_BREAK_LONG_LEN 32

#define LV_USE_PERF_MONITOR 0
#define LV_USE_OBJ_REALIGN 1

#define LV_USE_INDEV 1

#define LV_FONT_DEFAULT &lv_font_montserrat_40

#define LV_USE_FS_IF 0
#define LV_USE_FS_POSIX 0

#define LV_USE_SNAPSHOT 0

#define LV_USE_FONT_COMPRESSED 1
#define LV_FONT_FMT_TXT_LARGE 1
#define LV_FONT_CUSTOM_DECLARE \
    LV_FONT_DECLARE(montserrat_24_regular); \
    LV_FONT_DECLARE(montserrat_30_regular); \
    LV_FONT_DECLARE(montserrat_34_regular); \
    LV_FONT_DECLARE(montserrat_46_bold); \
    LV_FONT_DECLARE(montserrat_50_bold); \
    LV_FONT_DECLARE(montserrat_100_bold);

#endif /* LV_CONF_H*/

