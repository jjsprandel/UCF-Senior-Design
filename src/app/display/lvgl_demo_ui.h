#ifndef LVGL_DEMO_UI_H
#define LVGL_DEMO_UI_H
#include "lvgl.h"

static void btn_cb(lv_event_t *e);
static void set_angle(void *obj, int32_t v);
void example_lvgl_demo_ui(lv_display_t *disp);

#endif // LVGL_DEMO_UI_H