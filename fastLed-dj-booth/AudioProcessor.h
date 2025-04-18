#ifndef AUDIO_PROCESSOR_H
#define AUDIO_PROCESSOR_H

#include <Arduino.h>
#include <driver/i2s.h>
#include <arduinoFFT.h>
#include "Config.h"

#define I2S_PORT I2S_NUM_0


struct AudioFeatures {
    double volume = 0.0;
    double bass = 0.0;
    double mid = 0.0;
    double treble = 0.0;
    bool beatDetected = false;
    double bpm = 0.0;
    int loudness = 0;
    double spectrum[NUM_SAMPLES/2] = {0};
    const int16_t* waveform = nullptr;  // Initialize to nullptr
};

class AudioProcessor {
public:
    AudioProcessor();
    ~AudioProcessor();

    void begin();
    void captureAudio();
    AudioFeatures analyzeAudio();

    const double* getFFTData() const;
    const int16_t* getRawAudio() const;
    float getCurrentBPM() const;
    float getNormalizedVolume() const;

private:
    // Audio processing
    double vReal[NUM_SAMPLES];
    double vImag[NUM_SAMPLES];
    int16_t buffer[NUM_SAMPLES];  // Raw int waveform for display
    ArduinoFFT<double>* FFT;

    // State
    double previousVolume;
    unsigned long lastBeatTime;
    float currentBPM;
    float normalizedVolume;

    // Dynamic gain normalization
    float rollingMin = 1.0;
    float rollingMax = 0.0;
    float gainSmoothing = 0.95;
};

#endif
