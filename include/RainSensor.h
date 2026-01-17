// ==================== RAIN SENSOR HEADER ====================
// Rain Sensor MH-RD (Digital OUT)

#ifndef RAIN_SENSOR_H
#define RAIN_SENSOR_H

#include <Arduino.h>

// NodeMCU V3: D6 = GPIO12
const uint8_t RAIN_PIN = D6;

struct RainSensorData {
  bool isRaining;
};

RainSensorData rainSensorData = {false};

inline void readRainSensor() {
  rainSensorData.isRaining = (digitalRead(RAIN_PIN) == LOW);
}

inline void printRainSensorData() {
  Serial.printf("Rain State : %s\n", rainSensorData.isRaining ? "RAINING" : "NO RAIN");
}

#endif
