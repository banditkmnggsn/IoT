// ==================== INA219 SENSOR HEADER ====================
// Sensor Arus dan Tegangan

#ifndef INA219_SENSOR_H
#define INA219_SENSOR_H

#include <Arduino.h>
#include <Adafruit_INA219.h>
#include <Wire.h>

struct INA219Data {
  float busVoltageV;
  float shuntVoltageV;
  float currentmA;
  float power_mW;
  float voltage;
  float current;
  float power;
  bool isValid;
};

Adafruit_INA219 ina219Sensor;
INA219Data ina219Data = {0, 0, 0, 0, 0, 0, 0, false};

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
  ina219Data.shuntVoltageV = ina219Sensor.getShuntVoltage_mV() / 1000.0f;
  ina219Data.busVoltageV = ina219Sensor.getBusVoltage_V();
  ina219Data.currentmA = ina219Sensor.getCurrent_mA();
  ina219Data.power_mW = ina219Sensor.getPower_mW();
  ina219Data.voltage = ina219Data.busVoltageV;
  ina219Data.current = ina219Data.currentmA;
  ina219Data.power = ina219Data.power_mW;
}

void printINA219Data() {
  if (!ina219Data.isValid) {
    Serial.println("INA219: Sensor not ready");
    return;
  }
  Serial.printf("Bus Volt   : %.3f V\n", ina219Data.busVoltageV);
  Serial.printf("Shunt Volt : %.3f V\n", ina219Data.shuntVoltageV);
  Serial.printf("Current    : %.2f mA\n", ina219Data.currentmA);
  Serial.printf("Power      : %.2f mW\n", ina219Data.power_mW);
}

#endif
