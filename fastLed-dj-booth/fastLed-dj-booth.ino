#include <Arduino.h>
#include <FastLED.h>
#include <TFT_eSPI.h>
#include <Button2.h>
#include "Config.h"
#include "AudioProcessor.h"
#include "Animations.h"
#include "DisplayManager.h"
#include "ModeManager.h"
#include "HybridController.h"

// Hardware
CRGB leds[NUM_LEDS];
TFT_eSPI tft = TFT_eSPI();
Button2 button(BTN_PIN);
Button2 modeButton(35);
AudioProcessor audioProcessor(I2S_WS, I2S_SD, I2S_SCK);
DisplayManager displayManager(tft);
ModeManager modeManager;
HybridController hybridController;

void setup() {
  Serial.begin(115200);

  // LEDs
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(128);

  // Display
  tft.init();
  tft.setRotation(1);
  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, HIGH);
  displayManager.showStartupScreen();

  // Audio
  audioProcessor.begin();

  // Register animations once and share between manual and hybrid
  auto registerAnimation = [](const String& name, AnimationFunction func) {
    modeManager.addMode(name, func);
    hybridController.addAnimation(func, name);
  };

  registerAnimation("Bio-Signal", bioSignalAnimation);
  registerAnimation("Bass-Driven Firestorm", firestormAnimation);
  registerAnimation("Spectrum Ripple Cascade", rippleCascadeAnimation);
  registerAnimation("Beat-Synced Color Tunnel", colorTunnelAnimation);
  registerAnimation("Dynamic Energy Swirl", energySwirlAnimation);
  registerAnimation("Rhythmic Strobe Matrix", strobeMatrixAnimation);
  registerAnimation("Bass Bloom", bassBloomAnimation);
  registerAnimation("Color Drip", colorDripAnimation);
  registerAnimation("Frequency River", frequencyRiverAnimation);
  registerAnimation("Party Pulse", partyPulseAnimation);
  registerAnimation("Cyber Flux", cyberFluxAnimation);
  registerAnimation("Chaos Engine", chaosEngineAnimation);
  registerAnimation("Galactic Drift", galacticDriftAnimation);
  registerAnimation("Audio Storm", audioStormAnimation);

  // Button control
button.setPressedHandler([](Button2 &btn) {
  if (!modeManager.isAutoMode()) {
    modeManager.nextMode();
    displayManager.updateMode(
      modeManager.getCurrentModeName(),
      false,
      modeManager.getCurrentIndex(),
      modeManager.getModeCount()
    );
  }
});

 modeButton.setPressedHandler([](Button2 &btn) {
    modeManager.toggleAutoMode();
    if (modeManager.isAutoMode()) {
      hybridController.resetToFirst();
      displayManager.updateMode(
        hybridController.getCurrentName(),
        true,
        hybridController.getCurrentIndex(),
        hybridController.getAnimationCount()
      );
    } else {
      displayManager.updateMode(
        modeManager.getCurrentModeName(),
        false,
        modeManager.getCurrentIndex(),
        modeManager.getModeCount()
      );
    }
  });

  // Show initial mode
  displayManager.updateMode(modeManager.getCurrentModeName(), false, modeManager.getCurrentIndex(), modeManager.getModeCount());
}

void loop() {
   button.loop();
  modeButton.loop();

  // Audio input
  audioProcessor.captureAudio();
  AudioFeatures features = audioProcessor.analyzeAudio();

  // Update mode logic (handles auto switch timing)
  modeManager.update();

  // Run current animation
if (modeManager.isAutoMode()) {
    hybridController.update(leds, NUM_LEDS, features);
    displayManager.updateAudioVisualization(features, &hybridController);
} else {
    modeManager.executeCurrentMode(leds, NUM_LEDS, features);
    displayManager.updateAudioVisualization(features);
}

FastLED.show();
}
