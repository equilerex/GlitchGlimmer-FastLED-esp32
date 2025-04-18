// DisplayManager.h
#pragma once

#include <TFT_eSPI.h>
#include "GridLayout.h"
#include "AudioProcessor.h"
#include "HybridController.h" // <-- Add this include

class DisplayManager {
private:
    TFT_eSPI& _tft;
    GridLayout layout;

public:
    DisplayManager(TFT_eSPI& display); // Declare constructor only once
    void showStartupScreen();
    void updateAudioVisualization(const AudioFeatures& features, HybridController* hybrid);
    void drawFFTWaterfall(const double* fft, int bins);
};