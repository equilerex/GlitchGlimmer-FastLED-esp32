#include "HybridController.h"

#define DEBUG_ENABLED true // Set to false to disable debugging

void HybridController::debugLog(const String& message) {
    #if DEBUG_ENABLED
        Serial.printf("%s | CurrentIndex: %d, AnimationCount: %d, AvgVolume: %.2f, BuildUp: %d, Drop: %d, DebounceCounter: %d\n",
                      message.c_str(), currentIndex, animationCount, avgVolume, buildUp, drop, debounceCounter);
    #endif
}

HybridController::HybridController()
  : currentIndex(0), animationCount(0), lastSwitch(0),
    avgVolume(0), volumePos(0), buildUp(false), drop(false),
    smoothedVolume(0), debounceCounter(0) {
    memset(volumeHistory, 0, sizeof(volumeHistory));
    debugLog("HybridController initialized");
}

void HybridController::addAnimation(HybridAnimation animation, const String& name) {
    if (animationCount < HYBRID_ANIM_COUNT) {
        animations[animationCount] = animation;
        names[animationCount] = name;
        animationCount++;
        Serial.print("Added animation: ");
        Serial.println(name);
    }
}

String HybridController::getCurrentName() {
    return names[currentIndex];
}

int HybridController::getCurrentIndex() {
    return currentIndex;
}

int HybridController::getAnimationCount() {
    return animationCount;
}

float HybridController::getAverageVolume() {
    return avgVolume;
}

bool HybridController::getBuildUpFlag() {
    return buildUp;
}

bool HybridController::getDropFlag() {
    return drop;
}

bool HybridController::isBuildUp() {
    float delta = volumeHistory[(volumePos + 9) % 10] - volumeHistory[(volumePos + 5) % 10];
    buildUp = delta > 0.1;

    if (buildUp) {
        debugLog("isBuildUp true");
    }
    return buildUp;
}

bool HybridController::isDrop() {
    float delta = volumeHistory[(volumePos + 9) % 10] - volumeHistory[(volumePos + 5) % 10];
    drop = delta < -0.15;
    debugLog("isDrop evaluated");
    return drop;
}

bool HybridController::shouldSwitch(const AudioFeatures& features) {
    if (!autoSwitchEnabled) return false;
    unsigned long now = millis();

    // Tempo-aware min switch time
    float bpm = features.bpm > 0 ? features.bpm : 120;  // Fallback BPM
    const unsigned long ABSOLUTE_MIN_DURATION = 6000; // Absolute minimum of 6 seconds
    unsigned long tempoDuration = 1000 * (60.0 / bpm) * 8;  // Duration of 8 beats
    unsigned long requiredDuration = max(ABSOLUTE_MIN_DURATION, tempoDuration);

    bool enoughTimePassed = (now - lastSwitch) > requiredDuration;

    // Debounce logic
    if (features.beatDetected) {
        debounceCounter++;
    } else {
        debounceCounter = 0;
        modeKeepReason = "no beat";
    }

    bool beatStable = debounceCounter >= 3;

    debugLog("Evaluating shouldSwitch");

    if (isBuildUp()) {
        modeKeepReason = "Build up active";
        debounceCounter = 0;
        debugLog("Build up active, not switching");
        return false;
    }

    if (isDrop() && now - lastSwitch < 10000) {
        modeKeepReason = "isDrop 10000";
        debounceCounter = 0;
        debugLog("isDrop() and last switch was less than 10 seconds ago, not switching");
        return false;
    }

    if (enoughTimePassed == false) {
        modeKeepReason = "not enough time passed";
        debugLog("Not enough time passed");
        return false;
    }

    if (beatStable == false) {
        modeKeepReason = "beat not stable";
        debugLog("Beat not stable");
        return false;
    }

    return enoughTimePassed && beatStable;
}


void HybridController::switchAnimation() {
    if (animationCount <= 1) return;

    int newIndex = currentIndex;
    while (newIndex == currentIndex) {
        newIndex = random(animationCount);
    }
    currentIndex = newIndex;
    lastSwitch = millis();
    debounceCounter = 0;
    debugLog("Switched animation");
}

void HybridController::enableAutoSwitching() {
    autoSwitchEnabled = true;
}

void HybridController::disableAutoSwitching() {
    autoSwitchEnabled = false;
}

void HybridController::update(CRGB* leds, int numLeds, const AudioFeatures& features) {

    debugLog("Update called");

    // Apply low-pass filter to smooth volume
    const float alpha = 0.2; // Smoothing factor
    smoothedVolume = alpha * features.volume + (1 - alpha) * smoothedVolume;

    // Update volume history
    volumeHistory[volumePos] = smoothedVolume;
    volumePos = (volumePos + 1) % 10;

    // Calculate average volume
    avgVolume = 0;
    for (int i = 0; i < 10; i++) {
        avgVolume += volumeHistory[i];
    }
    avgVolume /= 10.0;

    if (shouldSwitch(features)) {
        switchAnimation();
    }

    if (animationCount > 0) {
        animations[currentIndex](leds, numLeds, features);
    }
}

void HybridController::resetToFirst() {
    currentIndex = 0;
    lastSwitch = millis();
    debounceCounter = 0;
    Serial.println("Reset to first animation.");
    debugLog("Reset to first animation");
}

