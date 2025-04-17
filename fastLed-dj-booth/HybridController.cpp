#include "HybridController.h"

HybridController::HybridController()
  : currentIndex(0), animationCount(0), lastSwitch(0),
    avgVolume(0), volumePos(0), buildUp(false), drop(false),
    smoothedVolume(0), debounceCounter(0) {
    memset(volumeHistory, 0, sizeof(volumeHistory));
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
    Serial.print("Build-up delta: ");
    Serial.println(delta);
    return buildUp;
}

bool HybridController::isDrop() {
    float delta = volumeHistory[(volumePos + 9) % 10] - volumeHistory[(volumePos + 5) % 10];
    drop = delta < -0.15;
    Serial.print("Drop delta: ");
    Serial.println(delta);
    return drop;
}

bool HybridController::shouldSwitch(const AudioFeatures& features) {
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
    }

    bool beatStable = debounceCounter >= 3;

    Serial.print("shouldSwitch check - TimePassed: ");
    Serial.print(enoughTimePassed);
    Serial.print(", BeatStable: ");
    Serial.print(beatStable);
    Serial.print(", BuildUp: ");
    Serial.print(isBuildUp());
    Serial.print(", Drop: ");
    Serial.println(isDrop());

    if (isBuildUp()) {
        debounceCounter = 0;
        return false;
    }

    if (isDrop() && now - lastSwitch < 10000) {
        debounceCounter = 0;
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
    Serial.print("Switched to animation: ");
    Serial.println(names[currentIndex]);
}

void HybridController::update(CRGB* leds, int numLeds, const AudioFeatures& features) {
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

    Serial.print("Smoothed Volume: ");
    Serial.print(smoothedVolume);
    Serial.print(", BPM: ");
    Serial.print(features.bpm);
    Serial.print(", BeatDetected: ");
    Serial.println(features.beatDetected);

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
}

