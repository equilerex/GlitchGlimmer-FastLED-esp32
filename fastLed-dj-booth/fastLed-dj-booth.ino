#include <Arduino.h>
#include <FastLED.h>
#include <TFT_eSPI.h>
#include <Button2.h>
#include "Config.h"
#include "AudioProcessor.h"
#include "Animations.h"
#include "DisplayManager.h"
#include "HybridController.h"

// Hardware
CRGB leds[NUM_LEDS];
TFT_eSPI tft = TFT_eSPI();
Button2 nextModeBtn(BTN_PIN);
Button2 autoModeBtn(35);
AudioProcessor audioProcessor(I2S_WS, I2S_SD, I2S_SCK);
DisplayManager displayManager(tft);
HybridController hybridController;

void setup() {
  Serial.begin(9600);

  // Initialize LEDs
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(128);

  // Initialize Display
  tft.init();
  tft.setRotation(1);
  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, HIGH);
  displayManager.showStartupScreen();

  // Initialize Audio
  audioProcessor.begin();

  // Register Animations
  registerAnimations();

  // Initialize Buttons
  setupButtons();
}

void registerAnimations() {
  for (size_t i = 0; i < HYBRID_ANIM_COUNT; i++) {
    hybridController.addAnimation(animations[i].function, animations[i].name);

    // Debug print for verification
    Serial.printf("Registered animation: %s\n", animations[i].name);
  }
}

void setupButtons() {
  nextModeBtn.setPressedHandler([](Button2 &btn) {
    hybridController.switchAnimation();
  });

 autoModeBtn.setPressedHandler([](Button2 &btn) {
   if (hybridController.autoSwitchEnabled) {
     hybridController.disableAutoSwitching();
   } else {
     hybridController.enableAutoSwitching();
   }
  });
}

void loop() {
  nextModeBtn.loop();
  autoModeBtn.loop();

  // Audio input
  audioProcessor.captureAudio();
  AudioFeatures features = audioProcessor.analyzeAudio();


  hybridController.update(leds, NUM_LEDS, features);
  displayManager.updateAudioVisualization(features, &hybridController);

FastLED.show();
}
