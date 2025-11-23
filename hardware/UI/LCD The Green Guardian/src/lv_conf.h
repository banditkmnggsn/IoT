#ifndef LV_CONF_H
#define LV_CONF_H

/* Basic LVGL Config */
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 0
#define LV_DPI_DEF 100

/* Display buffer size */
#define LV_USE_DRAW_SW 1
#define LV_DRAW_SW_COMPLETE_DRAW 1

/* Log (optional) */
#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN

/* Fonts */
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_20 1

/* Filesystem (optional) */
#define LV_USE_FS_STDIO 0

/* Input (touchscreen) */
#define LV_USE_INDEV 1

/* Enable LVGL examples (optional) */
#define LV_USE_DEMO_WIDGETS 1
// #define LV_USE_DEMO_BENCHMARK 1

#endif
