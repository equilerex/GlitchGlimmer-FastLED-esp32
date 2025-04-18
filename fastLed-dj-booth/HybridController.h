#ifndef HYBRID_CONTROLLER_H
#define HYBRID_CONTROLLER_H

#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include "AudioProcessor.h"
#include "Config.h"

class AudioFeatures;

typedef void (*HybridAnimation)(CRGB*, int, const AudioFeatures&);

class HybridController {
public:
    HybridController();
    void addAnimation(HybridAnimation animation, const String& name);
    void update(CRGB* leds, int numLeds, const AudioFeatures& features);
    void resetToFirst();
    void switchAnimation();
    void enableAutoSwitching();
    void disableAutoSwitching();
    void debugLog(const String& message);

    // Accessors
    String getCurrentName();
    int getCurrentIndex();
    int getAnimationCount();
    float getAverageVolume();
    bool getBuildUpFlag();
    bool getDropFlag();

    // Debug/Display reasons
    String getModeSwapReason() const;
    String getModeKeepReason() const;

    bool autoSwitchEnabled = true;

private:
    HybridAnimation animations[HYBRID_ANIM_COUNT];
    String names[HYBRID_ANIM_COUNT];
    int currentIndex;
    int animationCount;
    unsigned long lastSwitch;

    float volumeHistory[10];
    int volumePos;
    float avgVolume;
    float smoothedVolume;
    int debounceCounter;
    bool buildUp;
    bool drop;

    bool isBuildUp();
    bool isDrop();
    bool shouldSwitch(const AudioFeatures& features);

    String modeSwapReason = "Init";
    String modeKeepReason = "Init";
};

#endif
