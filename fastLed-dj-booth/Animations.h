#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <Arduino.h>
#include <FastLED.h>
#include "AudioProcessor.h"

// Define the type for animation functions
typedef void (*AnimationFunction)(CRGB* leds, int numLeds, const AudioFeatures& features);

// Animation function declarations
void firestormAnimation(CRGB* leds, int numLeds, const AudioFeatures& features);
void rippleCascadeAnimation(CRGB* leds, int numLeds, const AudioFeatures& features);
void colorTunnelAnimation(CRGB* leds, int numLeds, const AudioFeatures& features);
void energySwirlAnimation(CRGB* leds, int numLeds, const AudioFeatures& features);
void strobeMatrixAnimation(CRGB* leds, int numLeds, const AudioFeatures& features);
void bassBloomAnimation(CRGB* leds, int numLeds, const AudioFeatures& features);
void colorDripAnimation(CRGB* leds, int numLeds, const AudioFeatures& features);
void frequencyRiverAnimation(CRGB* leds, int numLeds, const AudioFeatures& features);
void partyPulseAnimation(CRGB* leds, int numLeds, const AudioFeatures& features);
void cyberFluxAnimation(CRGB* leds, int numLeds, const AudioFeatures& features);
void bioSignalAnimation(CRGB* leds, int numLeds, const AudioFeatures& features);
void chaosEngineAnimation(CRGB* leds, int numLeds, const AudioFeatures& features);
void galacticDriftAnimation(CRGB* leds, int numLeds, const AudioFeatures& features);
void audioStormAnimation(CRGB* leds, int numLeds, const AudioFeatures& features);


#endif