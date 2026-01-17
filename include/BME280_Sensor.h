// ==================== BME280 SENSOR HEADER ====================
// File: BME280_Sensor.h
// Sensor Suhu, Kelembaban, dan Tekanan Udara
// I2C Address: 0x76 (SDO LOW) atau 0x77 (SDO HIGH)

#ifndef BME280_SENSOR_H
#define BME280_SENSOR_H

#include <Arduino.h>
#include <Adafruit_BME280.h>
#include <Wire.h>

// ==================== KONFIGURASI BME280 ====================
// Tentukan alamat BME280 berdasarkan SDO pin configuration
#define BME280_I2C_ADDR_PRIMARY 0x76
#define BME280_I2C_ADDR_SECONDARY 0x77 

// ==================== STRUKTUR DATA BME280 ====================
struct BME280Data {
  float temperature;
  float humidity;
  float pressure; 
  uint8_t addressFound;
  bool isValid;
};

// ==================== OBJEK GLOBAL ====================
Adafruit_BME280 bme280Sensor;
BME280Data bme280Data = {0, 0, 0, 0, false};

// ==================== FUNGSI SCAN I2C UNTUK DEBUG ====================
void scanI2C(TwoWire *wire = &Wire) {
  Serial.println("I2C device scan");
  Serial.println("Scanning I2C devices on SDA=GPIO11, SCL=GPIO12...");
  
  byte error, address;
  int nDevices = 0;
  
  for(address = 1; address < 127; address++) {
    wire->beginTransmission(address);
    error = wire->endTransmission();
    
    if (error == 0) {
      Serial.printf("Device found at 0x%02X\n", address);
      nDevices++;
    }
  }
  
  if (nDevices == 0) {
    Serial.println("No I2C devices found");
  } else {
    Serial.printf("Total devices found: %d\n", nDevices);
  }
}

// ==================== RAW I2C READ TEST ====================
uint8_t readBME280Register(TwoWire *wire, uint8_t address, uint8_t reg) {
  wire->beginTransmission(address);
  wire->write(reg);
  uint8_t error = wire->endTransmission(false);
  
  if (error != 0) {
    return 0xFF; // Error
  }
  
  wire->requestFrom(address, (uint8_t)1);
  if (wire->available()) {
    return wire->read();
  }
  return 0xFF;
}
void initBME280(TwoWire *wire = &Wire) {
  Serial.println("BME280 initialization");
  Serial.println("Attempting to detect BME280...");
  
  bme280Data.isValid = false;
  
  // Try address 0x76 first
  Serial.println("Trying address 0x76 (SDO LOW)...");
  if (bme280Sensor.begin(BME280_I2C_ADDR_PRIMARY, wire)) {
    bme280Data.isValid = true;
    bme280Data.addressFound = BME280_I2C_ADDR_PRIMARY;
    Serial.printf("BME280 found at 0x%02X!\n", BME280_I2C_ADDR_PRIMARY);
  }
  // Try address 0x77 if 0x76 failed
  else {
    Serial.println("Not found at 0x76, trying 0x77 (SDO HIGH)...");
    if (bme280Sensor.begin(BME280_I2C_ADDR_SECONDARY, wire)) {
      bme280Data.isValid = true;
      bme280Data.addressFound = BME280_I2C_ADDR_SECONDARY;
      Serial.printf("BME280 found at 0x%02X!\n", BME280_I2C_ADDR_SECONDARY);
    }
    else {
      Serial.println("BME280 not found at either address");
      Serial.println("Check: GND connection, VCC power, I2C wiring");
      bme280Data.isValid = false;
    }
  }
}
// ==================== INISIALISASI BME280 DENGAN TIMEOUT ====================
// Extended debug initialization (disabled). Use the simple initBME280() above.

// ==================== BACA DATA BME280 ====================
void readBME280() {
  if (!bme280Data.isValid) {
    // ===== DEBUG: Try to re-initialize every 10 seconds =====
    static unsigned long lastRetry = 0;
    if (millis() - lastRetry > 10000) {
      Serial.println("BME280 still not valid, retrying initialization...");
      // Try again without Wire parameter first
      if (bme280Sensor.begin(BME280_I2C_ADDR_PRIMARY)) {
        bme280Data.isValid = true;
        bme280Data.addressFound = BME280_I2C_ADDR_PRIMARY;
        Serial.println("BME280 recovered at 0x76");
      } else if (bme280Sensor.begin(BME280_I2C_ADDR_SECONDARY)) {
        bme280Data.isValid = true;
        bme280Data.addressFound = BME280_I2C_ADDR_SECONDARY;
        Serial.println("BME280 recovered at 0x77");
      }
      lastRetry = millis();
    }
    return;
  }
  
  bme280Data.temperature = bme280Sensor.readTemperature();
  bme280Data.humidity = bme280Sensor.readHumidity();
  bme280Data.pressure = bme280Sensor.readPressure() / 100.0F; // Convert to hPa
}

// ==================== TAMPILKAN DATA BME280 ====================
void printBME280Data() {
  if (!bme280Data.isValid) {
    Serial.println("BME280: Sensor not ready");
    Serial.printf("Checked at address 0x%02X\n", bme280Data.addressFound);
    return;
  }
  Serial.printf("Temperature   : %.2f °C\n", bme280Data.temperature);
  Serial.printf("Humidity      : %.2f %%RH\n", bme280Data.humidity);
  Serial.printf("Pressure      : %.2f hPa\n", bme280Data.pressure);
}

#endif
