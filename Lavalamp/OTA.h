#ifndef OTA
#define OTA

//
// Method declarations
//
void setupOTA();
void loopOTA();

#ifndef MYWIFI
  #include "Wifi.h"
#endif
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

void setupOTA() {
  if(!wifiSetup) setupWifi();
  ArduinoOTA.setHostname(hostname);
  ArduinoOTA.begin();
  
  #ifdef DEBUG
    Serial.println("OTA Started");
    Serial.println();
  #endif
}

void loopOTA() {
  ArduinoOTA.handle();
}

#endif // OTA
