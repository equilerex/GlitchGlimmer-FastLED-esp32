// Config.h
#ifndef CONFIG_H
#define CONFIG_H

#define AUDIO_DEBUG false
#define MODE_DEBUG false


#define NUM_LEDS 60
#define NUM_SAMPLES 512
#define SAMPLE_RATE 44100

#define I2S_WS 26
#define I2S_SD 32
#define I2S_SCK 27
#define LED_PIN 25
#define BTN_PIN 0
#define BACKLIGHT_PIN 4
// Synchronize this count with the number of animations defined in the animations[] array in Animations.h will fail build if mismatching
#define HYBRID_ANIM_COUNT 14

#endif
