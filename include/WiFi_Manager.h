// ==================== WIFI MANAGER HEADER ====================
// Koneksi WiFi dengan auto reconnect

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>

struct WiFiConfig {
  const char *ssid;
  const char *password;
};

struct WiFiStatusInfo {
  bool connected;
  String ip;
  long rssi;
};

const WiFiConfig WIFI_CONFIG = {"The Green Guardian", "12345678"};
WiFiStatusInfo wifiStatus = {false, "0.0.0.0", 0};

inline void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_CONFIG.ssid, WIFI_CONFIG.password);

  Serial.println("WiFi connection");
  Serial.printf("SSID: %s\n", WIFI_CONFIG.ssid);

  int retry = 0;
  const int maxRetry = 30;
  while (WiFi.status() != WL_CONNECTED && retry < maxRetry) {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifiStatus.connected = true;
    wifiStatus.ip = WiFi.localIP().toString();
    wifiStatus.rssi = WiFi.RSSI();
    Serial.printf("\nWiFi connected. IP: %s, RSSI: %ld dBm\n", wifiStatus.ip.c_str(), wifiStatus.rssi);
  } else {
    wifiStatus.connected = false;
    Serial.println("\nWiFi connection failed");
  }
}

inline void initWiFiManager() {
  connectWiFi();
}

inline void handleWiFiReconnect() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WARN] WiFi lost, reconnecting...");
    connectWiFi();
  }
}

inline void updateWiFiInfo() {
  wifiStatus.connected = (WiFi.status() == WL_CONNECTED);
  wifiStatus.ip = wifiStatus.connected ? WiFi.localIP().toString() : "0.0.0.0";
  wifiStatus.rssi = wifiStatus.connected ? WiFi.RSSI() : 0;
}

inline bool isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

inline void printWiFiStatus() {
  Serial.printf("Connected : %s\n", wifiStatus.connected ? "YES" : "NO");
  Serial.printf("IP Addr   : %s\n", wifiStatus.ip.c_str());
  Serial.printf("RSSI      : %ld dBm\n", wifiStatus.rssi);
}

#endif
