#include "ModeManager.h"

ModeManager::ModeManager()
    : modeCount(0), currentModeIndex(0), autoMode(false),
      lastAutoSwitchTime(0), autoSwitchInterval(10000) // 10 seconds
{}

void ModeManager::addMode(const String& name, AnimationFunction func) {
    if (modeCount < MAX_MODES) {
        modes[modeCount++] = { name, func };
    }
}

void ModeManager::nextMode() {
    unsigned long now = millis();

    currentModeIndex = (currentModeIndex + 1) % modeCount;
    lastAutoSwitchTime = now;
}

void ModeManager::executeCurrentMode(CRGB* leds, int numLeds, const AudioFeatures& features) {

    if (modeCount > 0) {
        modes[currentModeIndex].func(leds, numLeds, features);
    }
}

String ModeManager::getCurrentModeName() const {
    return modeCount > 0 ? modes[currentModeIndex].name : "No Mode";
}

void ModeManager::toggleAutoMode() {
    autoMode = !autoMode;
    lastAutoSwitchTime = millis();
}

bool ModeManager::isAutoMode() const {
    return autoMode;
}

void ModeManager::update() {
    if (autoMode && millis() - lastAutoSwitchTime > autoSwitchInterval) {
        nextMode();
        lastAutoSwitchTime = millis();
    }
}

void ModeManager::prevMode() {
    currentModeIndex = (currentModeIndex - 1 + modeCount) % modeCount;
}