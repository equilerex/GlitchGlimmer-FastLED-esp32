// DisplayManager.h
#pragma once

#include <TFT_eSPI.h>
#include "GridLayout.h"
#include "AudioProcessor.h"

class DisplayManager {
private:
    TFT_eSPI& _tft;
    GridLayout layout;

public:
    DisplayManager(TFT_eSPI& display); // Declare constructor only once
    void showStartupScreen();
    void updateAudioVisualization(const AudioFeatures& features);
    void drawFFTWaterfall(const double* fft, int bins);
    void drawWaveform(const int16_t* waveform, int samples);
};