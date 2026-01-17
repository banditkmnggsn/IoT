// ==================== INA219 SENSOR HEADER ====================
// Sensor Arus dan Tegangan

#ifndef INA219_SENSOR_H
#define INA219_SENSOR_H

#include <Arduino.h>
#include <Adafruit_INA219.h>
#include <Wire.h>

struct INA219Data {
  float voltage;
  bool isValid;
};

Adafruit_INA219 ina219Sensor;
INA219Data ina219Data = {0.0f, false};

void initINA219(TwoWire *wire = &Wire) {
  Serial.println("INA219 sensor initialization");
  ina219Data.isValid = false;

  if (ina219Sensor.begin()) {
    ina219Data.isValid = true;
    ina219Sensor.setCalibration_32V_2A();
    Serial.println("INA219 detected (addr 0x40)");
  } else {
    Serial.println("INA219 not found");
  }
}

void readINA219() {
  if (!ina219Data.isValid) return;
  // Read only bus voltage
  ina219Data.voltage = ina219Sensor.getBusVoltage_V();
}

void printINA219Data() {
  if (!ina219Data.isValid) {
    Serial.println("INA219: Sensor not ready");
    return;
  }
  Serial.printf("Bus Volt   : %.3f V\n", ina219Data.voltage);
}

#endif
