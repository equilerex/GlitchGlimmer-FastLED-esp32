#ifndef MODE_MANAGER_H
#define MODE_MANAGER_H

#include <Arduino.h>
#include <FastLED.h>
#include "AudioProcessor.h" // Only if absolutely needed

// Define the type for animation functions
typedef void (*AnimationFunction)(CRGB* leds, int numLeds, const AudioFeatures& features);

class ModeManager {
public:
    ModeManager();
    void addMode(const String& name, AnimationFunction func);
    void nextMode();
    void executeCurrentMode(CRGB* leds, int numLeds, const AudioFeatures& features);
    String getCurrentModeName() const;
    void toggleAutoMode();
    bool isAutoMode() const;
    int getCurrentIndex() const { return currentModeIndex; }
    int getModeCount() const { return modeCount; }
    void prevMode();
    void update();



private:
    struct Mode {
        String name;
        AnimationFunction func;
    };
    static const int MAX_MODES = 15;
    Mode modes[MAX_MODES];
    int modeCount;
    int currentModeIndex;
    bool autoMode;
    unsigned long lastAutoSwitchTime;
    unsigned long autoSwitchInterval = 3000;
};

#endif
