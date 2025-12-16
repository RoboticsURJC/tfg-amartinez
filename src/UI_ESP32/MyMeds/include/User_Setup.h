#pragma once

#define USER_SETUP

#define ILI9341_DRIVER

#define TFT_WIDTH 240
#define TDT_HEIGHT 320

#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST   4

#define TFT_BL  21
#define TFT_BLACKLIGHT_ON HIGH

#define TFT_RGB_ORFER_TFT_BGR
#define TFT_OFFSET_ROTATION 2

#define TFT_OFFSET_X 0
#define TFT_OFFSET_Y 80

#define SPI_FREQUENCY 40000000
#define SPI_READ_FREQUENCY 20000000
#define SPI_TOUCH_FREQUENCY 2500000

#define TOUCH_CS 33
#define TOUCH_IRQ 36

