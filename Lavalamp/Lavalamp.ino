#define DEBUG

//
// Method declarations
//
void animLavaFrame();
void adjustColor(uint8_t*, uint8_t*);
uint32_t lerpColor(uint8_t*, uint8_t*, float);

#define PIXEL_PIN 0
#define NUMPIXELS 30
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel neo(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

uint8_t lavaColor[3] = {255, 15, 127};  // R, G, B
uint8_t baseColor[3] = {0, 0, 0};
uint8_t lavaColorTarget[3];             // Used to update the colors
uint8_t baseColorTarget[3];
int8_t lava[NUMPIXELS];                 // Used to calculate the lerp value between baseColor and lavaColor
uint16_t frame;                         // Used to determine when the animation should update
uint8_t mspf = 25;                      // Milliseconds per frame
uint8_t spawnrate = 4;                  // Add new lava pixel every # frames
uint8_t brightness = 127;               // Default brightness

const char* name = "Lavalamp";
#include "OTA.h"
#include "MQTT.h"

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
    Serial.println("\n");
    if(PIXEL_PIN == 1 || PIXEL_PIN == 3) {
      Serial.print("Pixels + DEBUG will not work with currently selected PIXEL_PIN: ");
      Serial.println(PIXEL_PIN);
    }
  #else
    // Set up pins 1 (tx) and 3 (rx) as normal GPIOs
    pinMode(1, FUNCTION_3);
    pinMode(3, FUNCTION_3);
  #endif
  
  setupOTA();
  setupMQTT();
  
  memcpy(lavaColorTarget, lavaColor, 3);
  memcpy(baseColorTarget, baseColor, 3);
  memset(lava, 1, sizeof(lava));

  neo.begin();
  neo.setBrightness(brightness);
  neo.show();
}

void loop() {
  loopOTA();
  loopMQTT();

  frame = millis() % (mspf * spawnrate);
  animLavaFrame();
}

void animLavaFrame() {
  if(frame == 0) {
    lava[random(0, NUMPIXELS)] += 1;
    delay(1);
  }

  if(frame % mspf == 0) {
    if(memcmp(lavaColor, lavaColorTarget, 3) != 0) adjustColor(lavaColor, lavaColorTarget);
    if(memcmp(baseColor, baseColorTarget, 3) != 0) adjustColor(baseColor, baseColorTarget);
    for(uint8_t i = 0; i < NUMPIXELS; i++) {
      lava[i] += lava[i] != 0 ? 1 : 0;
      neo.setPixelColor(i, neo.gamma32(lerpColor(baseColor, lavaColor, (float)abs(lava[i]) / 128.0)));
    }
    neo.show();
  }
}

void adjustColor(uint8_t *c, uint8_t *t) {
  for(uint8_t i = 0; i < 3; i++) {
    *(c + i) += 
      *(c + i) == *(t + i)
      ? 0
      : *(c + i) > *(t + i)
        ? -1
        : 1;
  }
}

uint32_t lerpColor(uint8_t *c1, uint8_t *c2, float t) {
  return neo.Color(
    (uint8_t)(*(c1 + 0) * (1.0 - t)) | (uint8_t)(*(c2 + 0) * t),
    (uint8_t)(*(c1 + 1) * (1.0 - t)) | (uint8_t)(*(c2 + 1) * t),
    (uint8_t)(*(c1 + 2) * (1.0 - t)) | (uint8_t)(*(c2 + 2) * t)
  );
}