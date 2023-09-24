//
// Method declarations
//
void handleColors(bool);
void handleSettings(bool);
void minuteDebug(bool);
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
    "time/minute",
    false,
    true,
    &minuteDebug
  },
  {
    "restart",
    true,
    false,
    &restart
  }
};

void handleColors(bool global) {
  // msg[0] = R
  // msg[1] = G
  // msg[2] = B
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
  // msg[0]: id
  // msg[1]: value
  if(sizeof(msg) < 2) return;

  if(global) {
    client.publish(localizeTopic(topic), msg, true);
    return;
  }

  uint8_t val = msg[1];
  switch(msg[0]) {
    case 0: // brightness
      brightness = val;
      neo.setBrightness(brightness);
      neo.show();
    break;
    case 1: // mspf
      mspf = val;
    break;
    case 2: // spawnrate
      spawnrate = val;
    break;
  }
}

void minuteDebug(bool global) {
  sprintf(msg, "%d", ESP.getFreeHeap());
  debug(msg);
}

void restart(bool global = false) {
  debug("Restarting...");
  delay(10);
  ESP.restart();
}
