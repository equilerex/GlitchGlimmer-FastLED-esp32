// Animations.h

#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <Arduino.h>
#include <FastLED.h>
#include "AudioProcessor.h"
#include "Config.h"

// Define a type for animation function pointers
typedef void (*AnimationFunction)(CRGB*, int, const AudioFeatures&);

// Struct to hold an animation's name and its corresponding function
struct AnimationEntry {
    const char* name;            // Animation name
    AnimationFunction function;  // Pointer to the animation function
};

// Declare all animation functions
extern void bioSignalAnimation(CRGB*, int, const AudioFeatures&);
extern void firestormAnimation(CRGB*, int, const AudioFeatures&);
extern void rippleCascadeAnimation(CRGB*, int, const AudioFeatures&);
extern void colorTunnelAnimation(CRGB*, int, const AudioFeatures&);
extern void energySwirlAnimation(CRGB*, int, const AudioFeatures&);
extern void strobeMatrixAnimation(CRGB*, int, const AudioFeatures&);
extern void bassBloomAnimation(CRGB*, int, const AudioFeatures&);
extern void colorDripAnimation(CRGB*, int, const AudioFeatures&);
extern void frequencyRiverAnimation(CRGB*, int, const AudioFeatures&);
extern void partyPulseAnimation(CRGB*, int, const AudioFeatures&);
extern void cyberFluxAnimation(CRGB*, int, const AudioFeatures&);
extern void chaosEngineAnimation(CRGB*, int, const AudioFeatures&);
extern void galacticDriftAnimation(CRGB*, int, const AudioFeatures&);
extern void audioStormAnimation(CRGB*, int, const AudioFeatures&);

// Define an array to hold all animation entries
const AnimationEntry animations[] = {
    { "Bio-Signal", bioSignalAnimation },
    { "Bass-Driven Firestorm", firestormAnimation },
    { "Spectrum Ripple Cascade", rippleCascadeAnimation },
    { "Beat-Synced Color Tunnel", colorTunnelAnimation },
    { "Dynamic Energy Swirl", energySwirlAnimation },
    { "Rhythmic Strobe Matrix", strobeMatrixAnimation },
    { "Bass Bloom", bassBloomAnimation },
    { "Color Drip", colorDripAnimation },
    { "Frequency River", frequencyRiverAnimation },
    { "Party Pulse", partyPulseAnimation },
    { "Cyber Flux", cyberFluxAnimation },
    { "Chaos Engine", chaosEngineAnimation },
    { "Galactic Drift", galacticDriftAnimation },
    { "Audio Storm", audioStormAnimation }
};

// Ensure HYBRID_ANIM_COUNT matches the number of defined animations
static_assert(HYBRID_ANIM_COUNT == (sizeof(animations) / sizeof(animations[0])),
              "HYBRID_ANIM_COUNT does not match the number of defined animations!");

#endif // ANIMATIONS_H