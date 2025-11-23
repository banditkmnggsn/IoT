#include <Arduino.h>
#include "lvgl.h"
#include "ui.h"
#include "lgfx_4827s043r.h"

LGFX lcd;

static const int LCD_WIDTH  = 480;
static const int LCD_HEIGHT = 272;
lv_display_t *disp;

void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *color_p)
{
    lcd.pushImage(
        area->x1,
        area->y1,
        (area->x2 - area->x1 + 1),
        (area->y2 - area->y1 + 1),
        (lgfx::rgb565_t*)color_p
    );

    lv_display_flush_ready(disp);
}

void setup()
{
    Serial.begin(115200);
    delay(300);

    // ====== Init LGFX ======
    lcd.init();
    lcd.setRotation(1);
    lcd.setBrightness(255);
    lcd.fillScreen(TFT_BLACK);

    // Simple display self-test: color bands, crosshair and pixels
    auto display_self_test = [&]() {
        Serial.println("Display self-test: start");
        const uint16_t colors[] = {TFT_RED, TFT_GREEN, TFT_BLUE, TFT_WHITE, TFT_YELLOW, TFT_CYAN, TFT_MAGENTA};
        const int n = sizeof(colors) / sizeof(colors[0]);
        const int band_h = LCD_HEIGHT / n;
        for (int i = 0; i < n; ++i) {
            lcd.fillRect(0, i * band_h, LCD_WIDTH, band_h, colors[i]);
            delay(400);
        }

        // crosshair
        lcd.fillRect(LCD_WIDTH / 2 - 2, 0, 4, LCD_HEIGHT, TFT_BLACK);
        lcd.fillRect(0, LCD_HEIGHT / 2 - 2, LCD_WIDTH, 4, TFT_BLACK);
        delay(300);

        // test pixels along top and left edges
        for (int x = 0; x < LCD_WIDTH; x += 10) lcd.drawPixel(x, 0, TFT_WHITE);
        for (int y = 0; y < LCD_HEIGHT; y += 10) lcd.drawPixel(0, y, TFT_WHITE);
        delay(500);

        // clear to black and finish
        lcd.fillScreen(TFT_BLACK);
        Serial.println("Display self-test: complete");
    };

    // run the self-test once at boot
    display_self_test();

    // ====== LVGL Init ======
    lv_init();

    // ====== Create LVGL display driver ======
    disp = lv_display_create(480, 272);

    // ====== LVGL Buffers (DOUBLE BUFFERING) ======
    static lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(480 * 40 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    static lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(480 * 40 * sizeof(lv_color_t), MALLOC_CAP_DMA);

    lv_display_set_buffers(
        disp,
        buf1, buf2,
        480 * 40,
        LV_DISPLAY_RENDER_MODE_PARTIAL
    );

    // ====== Register FLUSH callback ======
    lv_display_set_flush_cb(disp, lvgl_flush_cb);

    // ====== Load UI dari SquareLine ======
    ui_init();

    Serial.println("LCD + LGFX + LVGL successfully initialized!");
}


void loop()
{
    lv_timer_handler();
    delay(5);
}
