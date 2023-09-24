#ifndef MYWIFI
#define MYWIFI

//
// Method declarations
//
void setupWifi();

const char* ssid = "";
const char* pass = "";
bool wifiSetup = false;
char chipId[10];
char hostname[32];

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266mDNS.h>
#elif defined(ESP32)
  #include <ESPWiFi.h>
  #include <ESPmDNS.h>
#endif

void setupWifi() {
  #if defined(ESP8266)
    sprintf(chipId, "%08X", ESP.getChipId());
  #elif defined(ESP32)
    sprintf(chipId, "%08X", ESP.getEfuseMac());
  #endif
  sprintf(hostname, "%s-%s", name, chipId);

  WiFi.mode(WIFI_STA);
  WiFi.setHostname(hostname);
  WiFi.begin(ssid, pass);
  uint8_t retry = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if(retry >= 3) ESP.restart();
    delay(5000);
    retry++;
  }

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  wifiSetup = true;
}

#endif // MYWIFI
