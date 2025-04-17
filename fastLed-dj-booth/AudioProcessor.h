#ifndef AUDIO_PROCESSOR_H
#define AUDIO_PROCESSOR_H

#include <Arduino.h>
#include <driver/i2s.h>
#include <arduinoFFT.h>
#include "Config.h"

#define I2S_PORT I2S_NUM_0

struct AudioFeatures {
    double volume;
    double bass;
    double mid;
    double treble;
    bool beatDetected;
    float bpm;
float decibels;
    double spectrum[NUM_SAMPLES / 2];

};

class AudioProcessor {
public:
    AudioProcessor(int wsPin, int sdPin, int sckPin);
    ~AudioProcessor();
    void begin();
    void captureAudio();
    AudioFeatures analyzeAudio();

    const double* getFFTData() const;
    const int16_t* getRawAudio() const;
    float getCurrentBPM() const;
    float getNormalizedVolume() const;

private:
    int _wsPin, _sdPin, _sckPin;
    double vReal[NUM_SAMPLES];
    double vImag[NUM_SAMPLES];
    ArduinoFFT<double>* FFT;
    int16_t buffer[NUM_SAMPLES];
    double previousVolume;
    unsigned long lastBeatTime;
	void drawSpectrum(const double* spectrum, int bins);
    float currentBPM;
    float normalizedVolume;
    float rollingMin = 1.0;
    float rollingMax = 0.0;
    float gainSmoothing = 0.95;
};

#endif