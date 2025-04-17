#ifndef HYBRID_CONTROLLER_H
#define HYBRID_CONTROLLER_H

#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include "AudioProcessor.h"
#include "Config.h"  // 👈 Make sure this is included

class AudioFeatures;

typedef void (*HybridAnimation)(CRGB*, int, const AudioFeatures&);

class HybridController {
public:
    HybridController();
    void addAnimation(HybridAnimation animation, const String& name);
    void update(CRGB* leds, int numLeds, const AudioFeatures& features);
    String getCurrentName();
    int getCurrentIndex();
    int getAnimationCount();
    float getAverageVolume();
    bool getBuildUpFlag();
    bool getDropFlag();
    void resetToFirst();
    void prevMode();

private:
    HybridAnimation animations[HYBRID_ANIM_COUNT];
    String names[HYBRID_ANIM_COUNT];
    int currentIndex;
    int animationCount;
    unsigned long lastSwitch;
    float avgVolume;
    float volumeHistory[10];
    int volumePos;
    bool buildUp;
    bool drop;

    bool isBuildUp();
    bool isDrop();
    bool shouldSwitch(const AudioFeatures& features);
    void switchAnimation();
    float smoothedVolume;     // For volume smoothing
    int debounceCounter;      // For stable beat detection
};

#endif
