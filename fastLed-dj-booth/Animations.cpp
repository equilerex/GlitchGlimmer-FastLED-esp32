#include "Animations.h"
#include "Config.h"  // where NUM_LEDS is defined
#include <vector>
// Declare drips globally if needed across multiple functions
static std::vector<int> drips;

// Firestorm: Flames that pulse harder with bass
void firestormAnimation(CRGB* leds, int numLeds, const AudioFeatures& features) {
    static uint8_t* heat = nullptr;
    if (!heat) {
        heat = new uint8_t[numLeds];
        memset(heat, 0, numLeds);
    }

    for (int i = 0; i < numLeds; i++) {
        heat[i] = qsub8(heat[i], random8(0, constrain((features.volume * 255) / 12, 2, 10)));
    }

    for (int k = numLeds - 1; k >= 2; k--) {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    if (features.beatDetected) {
        for (int j = 0; j < 3; j++) {
            heat[random8(numLeds / 4)] = qadd8(heat[random8(numLeds / 4)], random8(180, 255));
        }
    }

    for (int j = 0; j < numLeds; j++) {
        leds[j] = HeatColor(heat[j]);
    }
}

// Ripple Cascade
void rippleCascadeAnimation(CRGB* leds, int numLeds, const AudioFeatures& features) {
    static uint8_t rippleColor = 0;
    static int rippleStep = -1;

    if (features.beatDetected) {
        rippleColor = random8();
        rippleStep = 0;
    }

    fadeToBlackBy(leds, numLeds, 64);

    if (rippleStep >= 0) {
        for (int i = 0; i < numLeds; i++) {
            int dist = abs((numLeds / 2) - i);
            if (dist == rippleStep) {
                leds[i] = CHSV(rippleColor, 255, 255 - dist * 20);
            }
        }
        rippleStep++;
        if (rippleStep > numLeds / 2) rippleStep = -1;
    }
}

// Color Tunnel
void colorTunnelAnimation(CRGB* leds, int numLeds, const AudioFeatures& features) {
    static uint8_t hue = 0;
    hue += features.volume * 8;

    for (int i = 0; i < numLeds; i++) {
        leds[i] = CHSV(hue + i * 3, 255, sin8(i * 5 + millis() / 12));
    }

    if (features.beatDetected) {
        for (int i = 0; i < numLeds; i += 2) {
            leds[i] += CHSV(hue + i * 3, 255, 255);
        }
    }
}

// Energy Swirl
void energySwirlAnimation(CRGB* leds, int numLeds, const AudioFeatures& features) {
    static uint8_t swirl = 0;
    swirl += features.mid * 8;

    for (int i = 0; i < numLeds; i++) {
        leds[i] = CHSV((i * 5 + swirl), 255, features.volume * 255);
    }

    blur1d(leds, numLeds, 30);
}

// Strobe Matrix
void strobeMatrixAnimation(CRGB* leds, int numLeds, const AudioFeatures& features) {
    static bool state = false;
    static unsigned long lastChange = 0;

    if (millis() - lastChange > (features.bass > 0.5 ? 60 : 180)) {
        state = !state;
        lastChange = millis();
    }

    if (state) {
        for (int i = 0; i < numLeds; i += random8(1, 5)) {
            leds[i] = CHSV(random8(), 255, 255);
        }
    } else {
        fill_solid(leds, numLeds, CRGB::Black);
    }
}

// Bass Bloom
void bassBloomAnimation(CRGB* leds, int numLeds, const AudioFeatures& features) {
    static uint8_t hue = 0;
    static int size = 0;

    if (features.bass > 0.5 || features.beatDetected) {
        size = numLeds / 2;
        hue = random8();
    }

    fadeToBlackBy(leds, numLeds, 25);
    for (int i = 0; i < size; i++) {
        int l = (numLeds / 2) - i;
        int r = (numLeds / 2) + i;
        if (l >= 0) leds[l] += CHSV(hue + i * 2, 255, 255 - i * 5);
        if (r < numLeds) leds[r] += CHSV(hue + i * 2, 255, 255 - i * 5);
    }
    if (size > 0) size--;
}

// Color Drip
void colorDripAnimation(CRGB* leds, int numLeds, const AudioFeatures& features) {
    static std::vector<int> drips;
    static uint8_t hue = 0;

    fadeToBlackBy(leds, numLeds, 30);

    if (features.treble > 0.25 || random8() < 4) {
        drips.push_back(0);
        hue += random8(5, 15);
    }

    for (int i = 0; i < drips.size(); ++i) {
        int pos = drips[i];
        if (pos < numLeds) {
            leds[pos] = CHSV(hue, 200, 255);
            if (pos > 0) leds[pos - 1].fadeToBlackBy(180);
            drips[i]++;
        }
    }

    drips.erase(std::remove_if(drips.begin(), drips.end(), [numLeds](int p) {
        return p >= numLeds;
    }), drips.end());
}

// Frequency River
void frequencyRiverAnimation(CRGB* leds, int numLeds, const AudioFeatures& features) {
    int third = numLeds / 3;
    fill_solid(leds, third, CHSV(160, 255, features.bass * 255));
    fill_solid(leds + third, third, CHSV(96, 255, features.mid * 255));
    fill_solid(leds + 2 * third, third, CHSV(0, 255, features.treble * 255));
    blur1d(leds, numLeds, 16);
}

// Party Pulse
void partyPulseAnimation(CRGB* leds, int numLeds, const AudioFeatures& features) {
    static uint8_t hue = 0;
    if (features.beatDetected) hue += 30;

    fill_gradient(leds, numLeds, CHSV(hue, 255, features.volume * 180), CHSV(hue + 64, 255, features.volume * 180));

    static int radius = 0;
    if (features.bass > 0.5) radius = numLeds / 2;

    for (int i = 0; i < radius; i++) {
        int l = (numLeds / 2) - i;
        int r = (numLeds / 2) + i;
        if (l >= 0) leds[l] += CHSV(hue + 60, 255, 255 - i * 4);
        if (r < numLeds) leds[r] += CHSV(hue + 60, 255, 255 - i * 4);
    }
    if (radius > 0) radius--;

    for (int i = 0; i < numLeds / 6; i++) {
        if (random8() < features.treble * 255 || random8() < features.mid * 100) {
            leds[random16(numLeds)] += CHSV(hue + random8(), 200, 255);
        }
    }
    blur1d(leds, numLeds, 18);
}

// Cyber Flux
void cyberFluxAnimation(CRGB* leds, int numLeds, const AudioFeatures& features) {
    static uint8_t hue = 0;
    hue += features.volume * 4;

    if (features.bass > 0.4) {
        int center = random16(numLeds);
        leds[center] = CHSV(hue, 255, 255);
        if (center > 0) leds[center - 1] = CHSV(hue + 20, 255, 180);
        if (center < numLeds - 1) leds[center + 1] = CHSV(hue - 20, 255, 180);
    }

    for (int i = 0; i < numLeds; i++) {
        leds[i] += CHSV(hue + (i * 2), 255, sin8(i * 4 + millis() / 6));
    }

    for (int i = 0; i < numLeds; i++) {
        if (random8() < features.treble * 220) {
            leds[i] = CRGB::White;
        }
    }
    fadeToBlackBy(leds, numLeds, 22);
}

// Bio-Signal
void bioSignalAnimation(CRGB* leds, int numLeds, const AudioFeatures& features) {
    static uint8_t offset = 0;
    offset += 2;

    uint8_t breath = sin8(millis() / 12);
    uint8_t brightness = (features.bass > 0.3) ? breath : 25;

    for (int i = 0; i < numLeds; i++) {
        uint8_t wave = sin8(i * 3 + offset);
        leds[i] = CHSV(wave + offset, 220, brightness);
        if (random8() < features.treble * 200) {
            leds[i] += CHSV(random8(), 255, 255);
        }
    }
    if (features.beatDetected) {
        for (int i = 0; i < numLeds; i++) {
            leds[i] += CHSV(0, 0, 40);
        }
    }
    blur1d(leds, numLeds, 30);
}

void chaosEngineAnimation(CRGB* leds, int numLeds, const AudioFeatures& features) {
    fill_rainbow(leds, numLeds, millis() / 10, 7);
}

void galacticDriftAnimation(CRGB* leds, int numLeds, const AudioFeatures& features) {
    for (int i = 0; i < numLeds; i++) {
        leds[i] = CHSV((i * 4 + millis() / 5) % 255, 255, sin8(i * 3 + millis() / 7));
    }
}

void audioStormAnimation(CRGB* leds, int numLeds, const AudioFeatures& features) {
    static uint8_t baseHue = 0;
    baseHue += features.volume * 10;
    for (int i = 0; i < numLeds; i++) {
        leds[i] = CHSV(baseHue + i * 5, 255, features.beatDetected ? 255 : 128);
    }
    fadeToBlackBy(leds, numLeds, 10);
}

