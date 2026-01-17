// ==================== BH1750 SENSOR HEADER ====================
// Sensor Cahaya (Lux)

#ifndef BH1750_SENSOR_H
#define BH1750_SENSOR_H

#include <Arduino.h>
#include <BH1750.h>
#include <Wire.h>

struct BH1750Data {
  float lightLevel;
  bool isValid;
};

BH1750 bh1750Sensor;
BH1750Data bh1750Data = {0, false};

void initBH1750(TwoWire *wire = &Wire) {
  Serial.println("\n[DEBUG] ┌─── BH1750 SENSOR INITIALIZATION ─────────┐");
  bh1750Data.isValid = false;

  if (bh1750Sensor.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23, wire)) {
    bh1750Data.isValid = true;
    Serial.println("[DEBUG] │ ✓ BH1750 DETECTED at 0x23");
  } else if (bh1750Sensor.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x5C, wire)) {
    bh1750Data.isValid = true;
    Serial.println("[DEBUG] │ ✓ BH1750 DETECTED at 0x5C");
  } else {
    Serial.println("[ERROR] │ ✗ BH1750 NOT FOUND (0x23/0x5C)");
  }
  Serial.println("[DEBUG] └──────────────────────────────────────────┘");
}

void readBH1750() {
  if (!bh1750Data.isValid) return;
  bh1750Data.lightLevel = bh1750Sensor.readLightLevel();
}

void printBH1750Data() {
  if (!bh1750Data.isValid) {
    Serial.println("│ ❌ BH1750: Sensor not ready");
    return;
  }
  Serial.printf("│ 💡 Light      : %.2f lux\n", bh1750Data.lightLevel);
}

#endif
