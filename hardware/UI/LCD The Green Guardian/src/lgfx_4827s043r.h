#pragma once
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
// include platform-specific headers to ensure Bus_RGB / Panel_RGB are visible
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>

// ==== DRIVER UNTUK ESP32-S3 + LCD 480x272 RGB565 ====
//
// Menggunakan class LGFX_Device dari LovyanGFX v1.2.0
// Tidak memakai _bus_instance atau _panel_instance (format lama)


class LGFX : public lgfx::LGFX_Device {

  // gunakan namespace v1 secara eksplisit dan hindari nama yang bentrok
  lgfx::v1::Bus_RGB   bus_;
  lgfx::v1::Panel_RGB panel_;

public:
  LGFX(void) {

    // ========= BUS RGB CONFIG =========
    {
      auto cfg = bus_.config();
      
      (void)0; // no panel_width/height in Bus_RGB::config_t

      // --- PIN DATA RGB565 ---
      cfg.pin_d0  = 8;    // B0
      cfg.pin_d1  = 3;    // B1
      cfg.pin_d2  = 46;   // B2
      cfg.pin_d3  = 9;    // B3
      cfg.pin_d4  = 1;    // B4

      cfg.pin_d5  = 5;    // G0
      cfg.pin_d6  = 6;    // G1
      cfg.pin_d7  = 7;    // G2
      cfg.pin_d8  = 15;   // G3
      cfg.pin_d9  = 16;   // G4
      cfg.pin_d10 = 4;    // G5

      cfg.pin_d11 = 45;   // R0
      cfg.pin_d12 = 48;   // R1
      cfg.pin_d13 = 47;   // R2
      cfg.pin_d14 = 21;   // R3
      cfg.pin_d15 = 14;   // R4

      // --- SYNC SIGNALS ---
      cfg.pin_henable = 40; // DE
      cfg.pin_vsync   = 41;
      cfg.pin_hsync   = 39;
      cfg.pin_pclk    = 42; // Pixel clock

      // --- TIMING for RGB bus (standard 480x272) ---
      cfg.hsync_polarity    = 1;
      cfg.hsync_front_porch = 8;
      cfg.hsync_pulse_width = 4;
      cfg.hsync_back_porch  = 43;

      cfg.vsync_polarity    = 1;
      cfg.vsync_front_porch = 4;
      cfg.vsync_pulse_width = 2;
      cfg.vsync_back_porch  = 12;

      cfg.pclk_active_neg = 1;
      cfg.pclk_idle_high  = 0;

      // --- PANEL CLOCK / bus speed ---
      cfg.freq_write = 16000000;  // 16 MHz aman

      bus_.config(cfg);
    }

    // ========= PANEL CONFIG =========
    {
      auto cfg = panel_.config();

      cfg.memory_width  = 480;
      cfg.memory_height = 272;
      cfg.panel_width   = 480;
      cfg.panel_height  = 272;

      cfg.offset_x = 0;
      cfg.offset_y = 0;

      // Timing standar 480×272 (4.3 inch RGB)
      panel_.config(cfg);
    }

    // Attach bus → panel
    panel_.setBus(&bus_);

    // Register panel ke LGFX device
    setPanel(&panel_);
  }
};
