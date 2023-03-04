//
// Method declarations
//
void handleColors(bool);
void handleSettings(bool);
void restart(bool);

typedef void (*mqtt_h)(bool);
typedef struct {
  const char topic[64]; // Base topic name
  uint8_t local; // If true, the chipId is prepended to the topic and subscribed
  uint8_t global; // If true, the topic is subscribed as-is
  mqtt_h handler; // The method that will handle messages to this topic
} MQTT_TopicHandler;

//
// Topics and their associated handler methods
//
MQTT_TopicHandler Topics[] = {
  {
    "lavalamp/lava",
    true,
    true,
    &handleColors
  },
  {
    "lavalamp/base",
    true,
    true,
    &handleColors
  },
  {
    "lavalamp/settings",
    true,
    true,
    &handleSettings
  },
  {
    "restart",
    true,
    false,
    &restart
  }
};

void handleColors(bool global) {
  if(global) {
    client.publish(localizeTopic(topic), msg, true); // Redirect global messages to the local topic and retain
    return;
  }

  uint8_t *c;
  c = strcmp(strrchr(topic, '/') + 1, "lava") == 0
    ? lavaColorTarget
    : baseColorTarget;
  uint8_t tempColor[3] = { msg[0], msg[1], msg[2] };
  memcpy(c, tempColor, 3); 
}

void handleSettings(bool global) {
  // msg[0]: mspf
  // msg[1]: spawnrate
  // msg[2]: brightness
  if(global) {
    client.publish(localizeTopic(topic), msg, true);
    return;
  }

  if(msg[0] != 0) mspf = msg[0];
  if(msg[1] != 0) spawnrate = msg[1];
  if(msg[2] != 0) brightness = msg[2];
  neo.setBrightness(brightness);
  neo.show();
}

void restart(bool global = false) {
  debug("Restarting...");
  delay(10);
  ESP.restart();
}
