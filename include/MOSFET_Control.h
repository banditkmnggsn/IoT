// ==================== MOSFET CONTROL HEADER ====================
// Kontrol Pompa via MOSFET (Active HIGH)

#ifndef MOSFET_CONTROL_H
#define MOSFET_CONTROL_H

#include <Arduino.h>

const uint8_t MOSFET_PIN_Z4 = D7;   // GPIO13
const uint8_t V5_ENABLE_PIN = D4;   // GPIO2 (5V enable control if used)

struct MOSFETData {
  bool state[2];  // index 1 used (zone 4)
};

MOSFETData mosfetData = {{false, false}};

inline void initMOSFET() {
  pinMode(MOSFET_PIN_Z4, OUTPUT);
  digitalWrite(MOSFET_PIN_Z4, LOW);

  pinMode(V5_ENABLE_PIN, OUTPUT);
  digitalWrite(V5_ENABLE_PIN, HIGH);

  mosfetData.state[1] = false;
}

inline void setMOSFET(uint8_t zone, bool on) {
  if (zone == 1) { 
    mosfetData.state[1] = on;
  }
}

inline void printMOSFETStatus() {
  Serial.printf("Pump Z4    : %s\n", mosfetData.state[1] ? "ON" : "OFF");
}

#endif
