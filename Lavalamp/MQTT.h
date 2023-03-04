#ifndef MQTT
#define MQTT

#define MQTT_Connected (char*)"<3"
#define MQTT_Disconnected (char*)"</3"
#define MQTT_HeartbeatTopic (char*)"heartbeat"
#define MQTT_DebugTopic (char*)"0/debug"

//
// Method declarations
//
void callback(char*, byte*, unsigned int);
void reconnect();
void setupMQTT();
void loopMQTT();
void heartbeat();
void heartbeat(const char*);
void debug(const char*);
char* localizeTopic(const char*);

//
// Includes and variables
//
#ifndef MYWIFI
  #include "Wifi.h"
#endif
#include <PubSubClient.h>
WiFiClient espClient;
PubSubClient client(espClient);

const char* mqtt_server = "BROKER";
char msg[128];
char topic[64];
uint8_t numTopics;
#include "MQTT_Handlers.h"

//
// Methods
//

void callback(char* _t, byte* _p, unsigned int _l) {
  memcpy(msg, _p, _l); // Copy payload to the msg buffer

  for(uint8_t i = 0; i < numTopics; i++) { // Determine which topic was received
    MQTT_TopicHandler t = Topics[i];
    strcpy(topic, t.topic);
    if(strcmp(_t, topic) == 0) {
      t.handler(true); // Global
    } else if(strcmp(_t, localizeTopic(topic)) == 0) {
      t.handler(false); // Local
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect(chipId, localizeTopic(MQTT_HeartbeatTopic), 0, false, MQTT_Disconnected)) {
      heartbeat();

      #ifdef DEBUG
        Serial.println("Subscribing to topics:");
      #endif
      for(uint8_t i = 0; i < numTopics; i++) {
        if(Topics[i].local) {
          client.subscribe(localizeTopic(Topics[i].topic));
          #ifdef DEBUG
            Serial.println(localizeTopic(Topics[i].topic));
          #endif
        }
        if(Topics[i].global) {
          client.subscribe(Topics[i].topic);
          #ifdef DEBUG
            Serial.println(Topics[i].topic);
          #endif
        }
      }
    } else {
      delay(5000);
    }
  }
}

void heartbeat() {
  heartbeat(MQTT_Connected);
}

void heartbeat(const char* payload) {
  client.publish(localizeTopic(MQTT_HeartbeatTopic), payload);
}

void debug(const char* payload) {
  client.publish(localizeTopic(MQTT_DebugTopic), payload);
}

char *localizeTopic(const char* lt) {
  static char t[64];
  sprintf(t, "%s/%s", chipId, lt);
  return t;
}

void setupMQTT() {
  if(!wifiSetup) setupWifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  numTopics = sizeof(Topics) / sizeof(MQTT_TopicHandler);
  
  #ifdef DEBUG
    Serial.println("Starting MQTT...");
  #endif

  loopMQTT();
}

void loopMQTT() {
  if(!client.connected()) {  
    #ifdef DEBUG
      Serial.println("Connecting MQTT...");
    #endif

    reconnect();
    
    #ifdef DEBUG
      Serial.println("MQTT Connected\n");
    #endif
  }

  client.loop();
}

#endif // MQTT
