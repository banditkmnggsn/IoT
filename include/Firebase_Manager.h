// ==================== FIREBASE MANAGER HEADER ====================
// Kirim data ke Firebase Realtime Database via REST (https)

#ifndef FIREBASE_MANAGER_H
#define FIREBASE_MANAGER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266HTTPClient.h>
#include <memory>

struct FirebaseConfig {
  String host;
  String path;
  String auth;
};

struct FirebaseSendResult {
  bool success;
  int httpCode;
  String response;
};

FirebaseSendResult firebaseResult = {false, 0, ""};

// TODO: sesuaikan host/path/auth sesuai project
FirebaseConfig firebaseConfig = {
  "your-project-id.firebaseio.com",
  "/esp8266",
  ""
};

const unsigned long FIREBASE_UPDATE_INTERVAL = 10UL * 1000UL;  // 10s

inline String buildFirebaseURL(const FirebaseConfig &cfg, const String &subPath) {
  String url = "https://" + cfg.host + cfg.path + subPath;
  if (cfg.auth.length() > 0) {
    url += "?auth=" + cfg.auth;
  }
  return url;
}

inline void initFirebase() {
  Serial.println("Firebase config loaded");
}

inline FirebaseSendResult sendToFirebase(const String &path, const String &jsonPayload) {
  firebaseResult = {false, 0, ""};

  if (WiFi.status() != WL_CONNECTED) {
    firebaseResult.response = "WiFi not connected";
    Serial.println("WiFi not connected, skip Firebase");
    return firebaseResult;
  }

  std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
  client->setInsecure();
  HTTPClient https;

  String url = buildFirebaseURL(firebaseConfig, path);
  Serial.printf("Firebase URL: %s\n", url.c_str());

  if (!https.begin(*client, url)) {
    firebaseResult.response = "HTTPS begin failed";
    Serial.println("HTTPS begin failed");
    return firebaseResult;
  }

  https.addHeader("Content-Type", "application/json");
  int httpCode = https.PUT(jsonPayload);
  firebaseResult.httpCode = httpCode;

  if (httpCode > 0) {
    firebaseResult.response = https.getString();
    firebaseResult.success = (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED);
    Serial.printf("Firebase HTTP %d\n", httpCode);
  } else {
    firebaseResult.response = https.errorToString(httpCode);
    Serial.printf("Firebase request failed: %s\n", firebaseResult.response.c_str());
  }

  https.end();
  return firebaseResult;
}

inline void printFirebaseStatus() {
  Serial.printf("Last HTTP  : %d\n", firebaseResult.httpCode);
  Serial.printf("Success    : %s\n", firebaseResult.success ? "YES" : "NO");
}

#endif
