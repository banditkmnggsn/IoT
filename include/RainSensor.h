// ==================== RAIN SENSOR HEADER ====================
// Rain Sensor MH-RD (Digital OUT)

#ifndef RAIN_SENSOR_H
#define RAIN_SENSOR_H

#include <Arduino.h>

// NodeMCU V3: D6 = GPIO12
const uint8_t RAIN_PIN = D6;

struct RainSensorData {
  int raw;
  bool isRaining;
  unsigned long lastChangeTime;
  unsigned long rainDuration;
  unsigned long dryDuration;
};

RainSensorData rainSensorData = {HIGH, false, 0, 0, 0};

inline void initRainSensor() {
  pinMode(RAIN_PIN, INPUT);
  rainSensorData.raw = digitalRead(RAIN_PIN);
  rainSensorData.isRaining = (rainSensorData.raw == LOW);
  rainSensorData.lastChangeTime = millis();
}

inline void readRainSensor() {
  int newRaw = digitalRead(RAIN_PIN);
  bool newState = (newRaw == LOW);

  if (newState != rainSensorData.isRaining) {
    unsigned long now = millis();
    if (rainSensorData.isRaining) {
      rainSensorData.rainDuration += (now - rainSensorData.lastChangeTime);
    } else {
      rainSensorData.dryDuration += (now - rainSensorData.lastChangeTime);
    }
    rainSensorData.lastChangeTime = now;
    rainSensorData.isRaining = newState;
  }

  rainSensorData.raw = newRaw;
}

inline void printRainSensorData() {
  Serial.printf("Rain Raw   : %d (LOW=rain)\n", rainSensorData.raw);
  Serial.printf("Rain State : %s\n", rainSensorData.isRaining ? "RAINING" : "NO RAIN");
}

#endif
