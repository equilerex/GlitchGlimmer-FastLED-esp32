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
AudioProcessor audioProcessor;
DisplayManager displayManager(tft);
HybridController hybridController;

void setup() {
    Serial.begin(115200);
    Serial.println("=== SETUP BEGIN ===");

    // Initialize LEDs
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(128);
    Serial.println("LEDs initialized");

    // Initialize Display
    tft.init();
    tft.setRotation(1);
    pinMode(BACKLIGHT_PIN, OUTPUT);
    digitalWrite(BACKLIGHT_PIN, HIGH);
    displayManager.showStartupScreen();
    delay(2000); // Wait for 2 seconds
    Serial.println("Display initialized");

    // Initialize Audio
    audioProcessor.begin();
    Serial.println("AudioProcessor initialized");

    // Register Animations
    registerAnimations();
    Serial.println("Animations registered");

    // Initialize Buttons
    setupButtons();
    Serial.println("Buttons initialized");
    Serial.println("=== SETUP END ===");
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
        hybridController.setAutoSwitchEnabled(!hybridController.isAutoSwitchEnabled());
    });
}

void loop() {
    Serial.println("=== LOOP BEGIN ===");
    nextModeBtn.loop();
    autoModeBtn.loop();

    // Audio input
    Serial.println("Capturing audio...");
    audioProcessor.captureAudio();
    Serial.println("Analyzing audio...");
    AudioFeatures features = audioProcessor.analyzeAudio();

    Serial.printf("AudioFeatures: vol=%.3f, bass=%.3f, mid=%.3f, treb=%.3f, beat=%d, bpm=%.2f, loud=%d\n",
        features.volume, features.bass, features.mid, features.treble, features.beatDetected, features.bpm, features.loudness);

    // Check waveform pointer
    Serial.printf("Waveform ptr: %p\n", (void*)features.waveform);

    // Update HybridController
    Serial.println("Updating HybridController...");
    hybridController.update(leds, NUM_LEDS, features);

    // Update the Display
    Serial.println("Updating DisplayManager...");
    displayManager.updateAudioVisualization(features, &hybridController);

    Serial.println("FastLED.show()");
    FastLED.show();

    Serial.println("=== LOOP END ===");
    delay(100); // Update interval
}
