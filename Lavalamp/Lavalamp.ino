/*
/ Method declarations
*/
void tick();
void animLavaFrame();
void adjustColor(uint8_t*, uint8_t*);
uint32_t lerpColor(uint8_t*, uint8_t*, float);

/*
/ Includes and defines
*/
#include <Adafruit_NeoPixel.h>
#define PIXEL_PIN 0
#define NUMPIXELS 30
Adafruit_NeoPixel neo(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

#include <Ticker.h>
Ticker ticker;

/*
/ Defaults and starting values
*/
uint8_t lavaColor[3] = {255, 15, 127};  // R, G, B
uint8_t baseColor[3] = {0, 0, 0};
uint8_t lavaColorTarget[3];             // Used to update the colors
uint8_t baseColorTarget[3];
int8_t lava[NUMPIXELS];                 // Used to calculate the lerp value between baseColor and lavaColor, note that it's signed
uint8_t frame = 0;                      // Used to determine when the animation should update
uint8_t mspf = 25;                      // Milliseconds per frame
uint8_t spawnrate = 4;                  // Add new lava pixel every # frames
uint8_t brightness = 127;               // Default brightness

/*
/ 
*/
const char* name = "Lavalamp_Tester";
#include "OTA.h"
#include "MQTT.h"

void setup() {
  setupOTA();
  setupMQTT();
  
  memcpy(lavaColorTarget, lavaColor, 3);
  memcpy(baseColorTarget, baseColor, 3); // Copy 3 bytes from *Color to *ColorTarget
  memset(lava, 1, sizeof(lava)); // Fill the lava array with 1

  neo.begin();
  neo.setBrightness(brightness);
  neo.show();

  ticker.attach_ms(mspf, tick);
}

/*
/ The new loop()
*/
void tick() {
  loopOTA();
  loopMQTT();

  animLavaFrame();
}

/*
/ Generates a new frame of animation, adding new lava and updating colors
*/
void animLavaFrame() {
  adjustColor(lavaColor, lavaColorTarget);
  adjustColor(baseColor, baseColorTarget);
  for(uint8_t i = 0; i < NUMPIXELS; i++) {
    lava[i] += lava[i] != 0 ? 1 : 0;
    neo.setPixelColor(i, neo.gamma32(lerpColor(baseColor, lavaColor, (float)abs(lava[i]) / 128.0)));
  }

  if(frame == 0) lava[random(0, NUMPIXELS)] += 1;
  frame++;
  frame %= spawnrate;

  neo.show();
}

/*
/ Adjusts the RGB values by 1 each frame.
/ sets the value rather than returning it
*/
void adjustColor(uint8_t *c, uint8_t *t) {
  if(memcmp(c, t, 3) == 0) return;
  for(uint8_t i = 0; i < 3; i++) {
    *(c + i) += 
      *(c + i) == *(t + i)
      ? 0
      : *(c + i) > *(t + i)
        ? -1
        : 1;
  }
}

/*
/ Returns c1 interpolated with c2 according to t (0.0 - 1.0)
/ As a packed 32-bit int
*/
uint32_t lerpColor(uint8_t *c1, uint8_t *c2, float t) {
  return neo.Color(
    (uint8_t)(*(c1 + 0) * (1.0 - t)) | (uint8_t)(*(c2 + 0) * t), // Red
    (uint8_t)(*(c1 + 1) * (1.0 - t)) | (uint8_t)(*(c2 + 1) * t), // Green
    (uint8_t)(*(c1 + 2) * (1.0 - t)) | (uint8_t)(*(c2 + 2) * t)  // Blue
  );
}

/*
/ Useless...
*/
void loop() {}
