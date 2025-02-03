#ifndef DISPLAY_CONFIG_H
#define DISPLAY_CONFIG_H

#include "lvgl.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_io_interface.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"

#define EXAMPLE_LCD_HOST SPI2_HOST
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ (20 * 1000 * 1000)
#define TEST_LCD_BIT_PER_PIXEL 16
#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL 1
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL
#define EXAMPLE_PIN_NUM_SCLK 10
#define EXAMPLE_PIN_NUM_MOSI 7
#define EXAMPLE_PIN_NUM_MISO 2
#define EXAMPLE_PIN_NUM_LCD_DC 18
#define EXAMPLE_PIN_NUM_LCD_RST 11 // Will need to change this to a different pin
#define EXAMPLE_PIN_NUM_LCD_CS 15
#define EXAMPLE_PIN_NUM_BK_LIGHT 2
#define EXAMPLE_PIN_NUM_TOUCH_CS 15

#define EXAMPLE_LCD_H_RES 240
#define EXAMPLE_LCD_V_RES 240
// Bit number used to represent command and parameter
#define EXAMPLE_LCD_CMD_BITS 8
#define EXAMPLE_LCD_PARAM_BITS 8
#define TEST_DELAY_TIME_MS (3000)

#define EXAMPLE_LVGL_DRAW_BUF_LINES 20 // number of display lines in each draw buffer
#define EXAMPLE_LVGL_TICK_PERIOD_MS 2
#define EXAMPLE_LVGL_TASK_MAX_DELAY_MS 500
#define EXAMPLE_LVGL_TASK_MIN_DELAY_MS 1
#define EXAMPLE_LVGL_TASK_STACK_SIZE (4 * 1024)
#define EXAMPLE_LVGL_TASK_PRIORITY 2

#define CONFIG_EXAMPLE_LCD_CONTROLLER_GC9A01 1

/*
// 'SCAN logo', 128x128px
// const unsigned char epd_bitmap_SCAN_logo[] = {
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x5f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xea, 0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x57, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x22, 0x22, 0x23, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf5, 0x55, 0x55, 0x55, 0x7f, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe8, 0xaf, 0xff, 0x8a, 0xbf, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x55, 0x7f, 0xff, 0xf5, 0x57, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x23, 0xff, 0xff, 0xfe, 0x2f, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0x57, 0xff, 0xff, 0xff, 0x55, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0xbf, 0xff, 0xff, 0xff, 0xeb, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf5, 0x7f, 0xff, 0xff, 0xff, 0xd5, 0x7f, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe2, 0xff, 0xff, 0xff, 0xff, 0xfa, 0x7f, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd5, 0xff, 0xff, 0xff, 0xff, 0xfd, 0x5f, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xab, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xbf, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x57, 0xff, 0xff, 0xff, 0xff, 0xff, 0x57, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x2f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xaf, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0x5f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd7, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xeb, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd5, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xfa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xf5, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf5, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xfa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfa, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xf5, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0x7f, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0x7f, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xbf, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x5f, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x5f, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xaf, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf5, 0xff, 0x57, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xaf, 0xff, 0xff,
//     0xff, 0xff, 0xdf, 0xff, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xff, 0xd7, 0xff, 0xff,
//     0xff, 0xff, 0xef, 0xfe, 0xaf, 0xfe, 0xaf, 0xff, 0xfb, 0xff, 0xff, 0xfe, 0xff, 0xef, 0xff, 0xff,
//     0xff, 0xfd, 0x55, 0xfd, 0x55, 0xfd, 0x57, 0xf5, 0xf5, 0xff, 0xf7, 0x7f, 0x7f, 0xd7, 0xff, 0xff,
//     0xff, 0xff, 0xbb, 0xfb, 0xbb, 0xff, 0xbf, 0xfb, 0xfb, 0xff, 0xff, 0xbf, 0xbf, 0xe3, 0xff, 0xff,
//     0xff, 0xfd, 0x55, 0xf5, 0x55, 0xfd, 0x57, 0xf5, 0xf5, 0xff, 0xff, 0x5f, 0x7f, 0xd7, 0xff, 0xff,
//     0xff, 0xfa, 0xee, 0xfe, 0xee, 0xfe, 0xaf, 0xee, 0xff, 0xff, 0xff, 0xbf, 0xbf, 0xeb, 0xff, 0xff,
//     0xff, 0xf5, 0x75, 0xf5, 0x75, 0x7d, 0x57, 0xf5, 0x75, 0xff, 0xf7, 0xd7, 0x5f, 0xf7, 0xff, 0xff,
//     0xff, 0xfb, 0xfb, 0xfb, 0xfb, 0xff, 0xbf, 0xfb, 0xfb, 0xff, 0xfb, 0xef, 0xbf, 0xf3, 0xff, 0xff,
//     0xff, 0xf5, 0x75, 0xf5, 0xf5, 0x7d, 0x57, 0xf5, 0x75, 0xff, 0xf5, 0xd7, 0xdf, 0xf5, 0xff, 0xff,
//     0xff, 0xfa, 0xff, 0xee, 0xfe, 0xfe, 0xef, 0xea, 0xfb, 0xff, 0xfb, 0xef, 0xbf, 0xfb, 0xff, 0xff,
//     0xff, 0xf5, 0x7f, 0xf5, 0x75, 0x7d, 0x57, 0xf5, 0x75, 0xff, 0xfd, 0xf7, 0xdf, 0xf5, 0xff, 0xff,
//     0xff, 0xfb, 0xbf, 0xfb, 0xfb, 0xfb, 0xfb, 0xfb, 0xbb, 0xff, 0xfe, 0xfb, 0xbf, 0xf3, 0xff, 0xff,
//     0xff, 0xfd, 0x5f, 0xf5, 0xff, 0xfd, 0x57, 0xf5, 0x55, 0xff, 0xfd, 0x77, 0xdf, 0xf5, 0xff, 0xff,
//     0xff, 0xfe, 0xff, 0xea, 0xff, 0xfa, 0xef, 0xee, 0xbf, 0xff, 0xfe, 0xfb, 0xef, 0xfb, 0xff, 0xff,
//     0xff, 0xfd, 0x57, 0xf5, 0xff, 0xf5, 0x57, 0xf5, 0x55, 0xff, 0xff, 0x77, 0xd7, 0xf5, 0xff, 0xff,
//     0xff, 0xff, 0xbf, 0xfb, 0xff, 0xfb, 0xfb, 0xfb, 0xbb, 0xff, 0xfe, 0xfb, 0xef, 0xfb, 0xff, 0xff,
//     0xff, 0xff, 0x55, 0xf5, 0xff, 0xf5, 0x55, 0xf5, 0x55, 0xff, 0xff, 0x75, 0xd7, 0xf5, 0xff, 0xff,
//     0xff, 0xff, 0xab, 0xea, 0xff, 0xfe, 0xef, 0xeb, 0xeb, 0xff, 0xfe, 0xfb, 0xef, 0xfb, 0xff, 0xff,
//     0xff, 0xff, 0x55, 0xf5, 0x7f, 0xf5, 0x75, 0xf5, 0x55, 0xff, 0xff, 0x77, 0xd7, 0xf5, 0xff, 0xff,
//     0xff, 0xff, 0xfb, 0xfb, 0xff, 0xfb, 0xfb, 0xfb, 0xbb, 0xff, 0xfe, 0xfb, 0xef, 0xfb, 0xff, 0xff,
//     0xff, 0xff, 0xd5, 0xf5, 0xf5, 0xf5, 0xd5, 0xf5, 0xd5, 0xff, 0xfd, 0x75, 0xdf, 0xf5, 0xff, 0xff,
//     0xff, 0xff, 0xea, 0xee, 0xfa, 0xfe, 0xeb, 0xeb, 0xef, 0xff, 0xfe, 0xfb, 0xef, 0xfb, 0xff, 0xff,
//     0xff, 0xf5, 0x75, 0x75, 0x75, 0x75, 0x55, 0xf5, 0xd5, 0xff, 0xfd, 0x77, 0xd7, 0xf5, 0xff, 0xff,
//     0xff, 0xfb, 0xfb, 0xfb, 0xfb, 0xfb, 0xbb, 0xfb, 0xfb, 0xff, 0xfe, 0xf3, 0xbf, 0xfb, 0xff, 0xff,
//     0xff, 0xf5, 0x75, 0x75, 0xf5, 0x75, 0x55, 0xf5, 0xd5, 0xff, 0xfd, 0xf7, 0xdf, 0xf5, 0xff, 0xff,
//     0xff, 0xfe, 0xfa, 0xea, 0xfe, 0xea, 0xff, 0xeb, 0xeb, 0xff, 0xfb, 0xef, 0xbf, 0xfb, 0xff, 0xff,
//     0xff, 0xf5, 0x75, 0x75, 0x75, 0x75, 0xf5, 0xf5, 0xf5, 0xff, 0xf5, 0xd7, 0x5f, 0xf5, 0xff, 0xff,
//     0xff, 0xfb, 0xbb, 0xfb, 0xbb, 0xfb, 0xfb, 0xfb, 0xfb, 0xff, 0xfb, 0xef, 0xbf, 0xf3, 0xff, 0xff,
//     0xff, 0xfd, 0x55, 0xf5, 0x55, 0xd5, 0xf5, 0xf5, 0xf5, 0xff, 0xff, 0xdf, 0x5f, 0xf5, 0xff, 0xff,
//     0xff, 0xfe, 0xab, 0xfa, 0xab, 0xef, 0xfe, 0xeb, 0xff, 0xff, 0xff, 0xbf, 0xbf, 0xeb, 0xff, 0xff,
//     0xff, 0xfd, 0x55, 0xfd, 0x57, 0xd5, 0xf5, 0x75, 0xf5, 0xff, 0xff, 0x5f, 0x7f, 0xf7, 0xff, 0xff,
//     0xff, 0xff, 0xbf, 0xff, 0xbf, 0xfb, 0xfb, 0xfb, 0xfb, 0xff, 0xff, 0xbe, 0xff, 0xe3, 0xff, 0xff,
//     0xff, 0xff, 0xdf, 0xff, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0x7f, 0xd7, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xef, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf5, 0x7f, 0xd7, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xe7, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf5, 0xff, 0xd7, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x8f, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x57, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xaf, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x5f, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x5f, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0x7f, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xf5, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0x7f, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfa, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xf5, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf5, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xfa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xeb, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd5, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x5f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd7, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xaf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xaf, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x57, 0xff, 0xff, 0xff, 0xff, 0xf7, 0x5f, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xa3, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd5, 0xff, 0xff, 0xff, 0xff, 0xfd, 0x7f, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xea, 0xff, 0xff, 0xff, 0xff, 0xfa, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf5, 0x7f, 0xff, 0xff, 0xff, 0xd5, 0x7f, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfa, 0x3f, 0xff, 0xff, 0xff, 0xa3, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0x57, 0xff, 0xff, 0xff, 0x55, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xab, 0xff, 0xff, 0xfe, 0xaf, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x55, 0x7f, 0xff, 0xf5, 0x5f, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe2, 0x3f, 0xff, 0xaa, 0x3f, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf5, 0x55, 0x55, 0x55, 0x7f, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xaa, 0xaa, 0x8b, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x57, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfa, 0x23, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
*/
bool example_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx);
void example_lvgl_port_update_callback(lv_display_t *disp);
void example_lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
void example_increase_lvgl_tick(void *arg);
void example_lvgl_port_task(void *arg);
lv_display_t *gc9a01_init();
void display_test(void *pvParameters);
void display_test_1(void *pvParameters);

#endif