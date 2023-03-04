#ifndef MYWIFI
#define MYWIFI

//
// Method declarations
//
void setupWifi();

const char* ssid = "SSID";
const char* pass = "PASS";
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
  
  #ifdef DEBUG
    Serial.println("Connecting Wifi...");
    Serial.print("chipId: ");
    Serial.println(chipId);
    Serial.print("Hostname: ");
    Serial.println(hostname);
  #endif

  WiFi.mode(WIFI_STA);
  WiFi.setHostname(hostname);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    
    #ifdef DEBUG
      Serial.print(".");
    #endif
  }

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  #ifdef DEBUG
    Serial.println();
    Serial.println("Wifi Connected");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();
  #endif

  wifiSetup = true;
}

#endif // MYWIFI
