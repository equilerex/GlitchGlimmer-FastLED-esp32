#include "HybridController.h"
#include "Config.h"


void HybridController::debugLog(const String& message) {
#if MODE_DEBUG
    Serial.printf("%s | Index: %d, Count: %d, Vol: %.3f, BuildUp: %d, Drop: %d, Debounce: %d, Reason: %s\n",
                  message.c_str(), currentIndex, animationCount, avgVolume, buildUp, drop, debounceCounter,
                  autoSwitchEnabled ? modeSwapReason.c_str() : modeKeepReason.c_str());
#endif
}

HybridController::HybridController()
    : currentIndex(0), animationCount(0), lastSwitch(0),
      avgVolume(0), volumePos(0), smoothedVolume(0), debounceCounter(0),
      buildUp(false), drop(false) {
    memset(volumeHistory, 0, sizeof(volumeHistory));
    debugLog("HybridController initialized");
}

void HybridController::addAnimation(HybridAnimation animation, const String& name) {
    if (animationCount < HYBRID_ANIM_COUNT) {
        animations[animationCount] = animation;
        names[animationCount] = name;
        animationCount++;
        Serial.print("Added animation: ");
        debugLog(name);
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

String HybridController::getModeSwapReason() const {
    return modeSwapReason;
}

String HybridController::getModeKeepReason() const {
    return modeKeepReason;
}

bool HybridController::isBuildUp() {
    float delta = volumeHistory[(volumePos + 9) % 10] - volumeHistory[(volumePos + 5) % 10];
    buildUp = delta > 0.1;
    return buildUp;
}

bool HybridController::isDrop() {
    float delta = volumeHistory[(volumePos + 9) % 10] - volumeHistory[(volumePos + 5) % 10];
    drop = delta < -0.15;
    return drop;
}

bool HybridController::shouldSwitch(const AudioFeatures& features) {
    if (!autoSwitchEnabled) {
        modeKeepReason = "Auto mode disabled";
        return false;
    }

    // Tempo-aware min switch time
    unsigned long now = millis();
    float bpm = features.bpm > 0 ? features.bpm : 120;
    const unsigned long ABS_MIN = 6000;
    unsigned long beatDuration = 1000 * (60.0 / bpm) * 8;
    unsigned long requiredDelay = max(ABS_MIN, beatDuration);

    bool enoughTimePassed = (now - lastSwitch) > requiredDelay;

    if (features.beatDetected) {
        debounceCounter++;
    } else {
        debounceCounter = 0;
        modeKeepReason = "No beat";
    }

    bool beatStable = debounceCounter >= 3;

    if (isBuildUp()) {
        debounceCounter = 0;
        modeKeepReason = "Build-up detected";
        return false;
    }

    if (isDrop() && (now - lastSwitch) < 10000) {
        debounceCounter = 0;
        modeKeepReason = "Recent drop";
        return false;
    }

    if (!enoughTimePassed) {
        modeKeepReason = "...";
        return false;
    }

    if (!beatStable) {
        modeKeepReason = "Unstable beat";
        return false;
    }

    modeSwapReason = "Stable beat + time passed";
    return true;
}

void HybridController::switchAnimation() {
    if (animationCount <= 1) return;  // Do nothing if there’s only one animation

    int newIndex = currentIndex;
    if (!autoSwitchEnabled) {
        // In manual mode, move to the next animation in sequence
        newIndex = (currentIndex + 1) % animationCount;
    } else {
        // In automatic mode, pick a random animation
        newIndex = random(animationCount);
    }

    // Update the current animation
    currentIndex = newIndex;
    lastSwitch = millis();
    debounceCounter = 0;
    debugLog("Switched animation");
}

bool HybridController::isAutoSwitchEnabled() const {
    return autoSwitchEnabled;
}



void HybridController::setAutoSwitchEnabled(bool enabled) {
    autoSwitchEnabled = enabled;
    debugLog(enabled ? "Auto-switching ENABLED" : "Auto-switching DISABLED");
}


void HybridController::update(CRGB* leds, int numLeds, const AudioFeatures& features) {
    debugLog("Update called");

    // Low-pass smoothing of volume
    const float alpha = 0.2;
    smoothedVolume = alpha * features.volume + (1.0f - alpha) * smoothedVolume;

    // Update rolling history
    volumeHistory[volumePos] = smoothedVolume;
    volumePos = (volumePos + 1) % 10;

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
