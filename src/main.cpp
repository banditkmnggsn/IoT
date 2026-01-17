#include <Arduino.h>
#include <Wire.h>

// ==================== I2C CONFIGURATION ====================
// ESP32-S3 Nano (KEBAKAR)
// #define I2C_SDA 11
// #define I2C_SCL 12

// ESP8266 NodeMCU V3 (AKTIF)
#define I2C_SDA D2  // GPIO4
#define I2C_SCL D1  // GPIO5

// ==================== INCLUDE ALL SENSOR HEADERS ====================
#include "BME280_Sensor.h"
#include "BH1750_Sensor.h"
#include "RainSensor.h"
#include "INA219_Sensor.h"
#include "MOSFET_Control.h"
#include "WiFi_Manager.h"
#include "Firebase_Manager.h"

// ==================== VARIABEL GLOBAL ====================
unsigned long lastFirebaseUpdate = 0;

// ==================== AUTO CONTROL CONFIGURATION ====================
#define TEMP_THRESHOLD_HIGH 31.0
#define TEMP_THRESHOLD_LOW 29.0
bool autoControlEnabled = true; 

// ==================== SETUP ====================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("GREEN GUARDIAN - IOT SYSTEM ");

  
  // ===== INISIALISASI I2C =====
  Serial.println("\n[DEBUG] Initializing I2C...");
  Wire.begin(I2C_SDA, I2C_SCL);  
  Serial.println("[DEBUG] ✓ I2C Clock initialized");
  Serial.printf("[DEBUG] SDA=D2 (GPIO%d), SCL=D1 (GPIO%d)\n", I2C_SDA, I2C_SCL);
  delay(1000);
  
  // ===== I2C SCAN =====
  Serial.println("\n[DEBUG] ===== I2C DEVICE SCAN =====");
  int deviceCount = 0;
  for(byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();
    if(error == 0) {
      Serial.printf("[DEBUG] ✓ Device found at 0x%02X\n", addr);
      deviceCount++;
    }
  }
  Serial.printf("[DEBUG] Total devices found: %d\n", deviceCount);
  if(deviceCount == 0) {
    Serial.println("[ERROR] NO I2C DEVICES DETECTED!");

  }
  delay(1000);
  
  // ===== INISIALISASI SEMUA SENSOR =====
  Serial.println("\n[DEBUG] ===== SENSOR INITIALIZATION =====");
  
  Serial.println("[DEBUG] 1. Initializing BME280...");
  initBME280(&Wire);
  delay(500);
  
  Serial.println("[DEBUG] 2. Initializing BH1750...");
  initBH1750(&Wire);
  delay(500);
  
  Serial.println("[DEBUG] 3. Initializing INA219...");
  initINA219(&Wire);
  delay(500);
  
  Serial.println("[DEBUG] 4. Initializing Rain Sensor...");
  initRainSensor();
  delay(500);
  
  Serial.println("[DEBUG] 5. Initializing MOSFET & GPIO...");
  initMOSFET();
  delay(500);
  
  Serial.println("[DEBUG] ✓ All sensors initialized!");
  delay(1000);
  
  // ===== INISIALISASI WIFI & FIREBASE =====
  Serial.println("[DEBUG] 6. Initializing WiFi Manager...");
  initWiFiManager();
  delay(500);
  
  Serial.println("[DEBUG] 7. Initializing Firebase...");
  initFirebase();
  delay(500);
  
  delay(1000);

  Serial.println("System Ready! Starting data collection");

  delay(2000);
}

// ==================== LOOP ====================
void loop() {
  // ===== HANDLE WIFI RECONNECT =====
  handleWiFiReconnect();
  updateWiFiInfo();
  
  // ===== BACA SEMUA SENSOR =====
  readBME280();
  readBH1750();
  readRainSensor();
  readINA219();
  
  // ===== AUTO CONTROL: Pompa berdasarkan suhu =====
  if (autoControlEnabled && bme280Data.isValid) {
    float currentTemp = bme280Data.temperature;
    bool pumpCurrentlyOn = mosfetData.state[1];
    
    // Hysteresis control untuk mencegah on-off berulang
    if (!pumpCurrentlyOn && currentTemp >= TEMP_THRESHOLD_HIGH) {
      // Suhu tinggi & pompa mati → Nyalakan pompa
      setMOSFET(1, true);
      Serial.println("\n🌡️  [AUTO] Temperature HIGH! Pump ON");
      Serial.printf("    Temp: %.2f°C >= %.1f°C\n", currentTemp, TEMP_THRESHOLD_HIGH);
    } 
    else if (pumpCurrentlyOn && currentTemp <= TEMP_THRESHOLD_LOW) {
      // Suhu turun & pompa nyala → Matikan pompa
      setMOSFET(1, false);
      Serial.println("\n🌡️  [AUTO] Temperature normalized. Pump OFF");
      Serial.printf("    Temp: %.2f°C <= %.1f°C\n", currentTemp, TEMP_THRESHOLD_LOW);
    }
  }
  
  // ===== TAMPILKAN DATA KE SERIAL =====
  Serial.println("\nAIR ENVIRONMENT");
  printBME280Data();
  printBH1750Data();
  
  Serial.println("\nRAIN & POWER");
  printRainSensorData();
  printINA219Data();
  
  Serial.println("\n┌─── MOSFET STATUS ───────────────────────────┐");
  printMOSFETStatus();
  Serial.println("│");
  Serial.printf("│ 🤖 Auto Control : %s\n", autoControlEnabled ? "ENABLED ✓" : "DISABLED");
  if (autoControlEnabled && bme280Data.isValid) {
    Serial.printf("│ 🌡️  Threshold   : HIGH=%.1f°C, LOW=%.1f°C\n", 
      TEMP_THRESHOLD_HIGH, TEMP_THRESHOLD_LOW);
    Serial.printf("│ 📊 Current Temp : %.2f°C\n", bme280Data.temperature);
  }
  Serial.println("└─────────────────────────────────────────────┘");
  
  Serial.println("\n┌─── WIFI STATUS ─────────────────────────────┐");
  printWiFiStatus();
  Serial.println("└─────────────────────────────────────────────┘");
  
  Serial.println("\n┌─── FIREBASE STATUS ─────────────────────────┐");
  printFirebaseStatus();
  Serial.println("└─────────────────────────────────────────────┘");
  
  // ===== UPDATE FIREBASE SETIAP INTERVAL =====
  if (isWiFiConnected() && millis() - lastFirebaseUpdate >= FIREBASE_UPDATE_INTERVAL) {
    Serial.println("\n📤 Sending data to Firebase...");
    
    // Build JSON string manually
    String jsonData = "{";
    jsonData += "\"temperature\":" + String(bme280Data.temperature, 2) + ",";
    jsonData += "\"humidity\":" + String(bme280Data.humidity, 2) + ",";
    jsonData += "\"pressure\":" + String(bme280Data.pressure, 2) + ",";
    jsonData += "\"lightLevel\":" + String(bh1750Data.lightLevel, 2) + ",";
    jsonData += "\"voltage\":" + String(ina219Data.voltage, 2) + ",";
    jsonData += "\"current\":" + String(ina219Data.current, 2) + ",";
    jsonData += "\"power\":" + String(ina219Data.power, 2) + ",";
    
    // Add rain sensor data
    jsonData += "\"isRaining\":" + (rainSensorData.isRaining ? String("true") : String("false")) + ",";
    jsonData += "\"rainDuration\":" + String(rainSensorData.isRaining ? (millis() - rainSensorData.lastChangeTime) : rainSensorData.rainDuration) + ",";
    jsonData += "\"dryDuration\":" + String(!rainSensorData.isRaining ? (millis() - rainSensorData.lastChangeTime) : rainSensorData.dryDuration) + ",";
    
    // Add MOSFET status (single zone only for ESP8266)
    jsonData += "\"mosfet\":{";
    jsonData += "\"zone_4\":" + (mosfetData.state[1] ? String("true") : String("false"));  // Z4 only
    jsonData += "},";
    
    // Add auto control status
    jsonData += "\"autoControl\":{";
    jsonData += "\"enabled\":" + (autoControlEnabled ? String("true") : String("false")) + ",";
    jsonData += "\"tempThresholdHigh\":" + String(TEMP_THRESHOLD_HIGH, 1) + ",";
    jsonData += "\"tempThresholdLow\":" + String(TEMP_THRESHOLD_LOW, 1);
    jsonData += "},";
    jsonData += "\"timestamp\":" + String(millis());
    jsonData += "}";
    
    sendToFirebase("/sensor_data", jsonData);
    lastFirebaseUpdate = millis();
  }
  
  delay(2000);  // Baca setiap 2 detik
}
