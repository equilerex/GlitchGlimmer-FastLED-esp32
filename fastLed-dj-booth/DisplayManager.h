#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <TFT_eSPI.h>
#include "AudioProcessor.h"

class HybridController;

class DisplayManager {
public:
    DisplayManager(TFT_eSPI &display);

    void showStartupScreen();
    void updateAudioVisualization(const AudioFeatures& features, HybridController* hybrid);
    void updateAudioVisualization(const AudioFeatures& features);

private:
    TFT_eSPI& _tft;
    String lastModeName;
    int ringRadius;
    int ringFade;
    double smoothedBPM;
    double bpmSmoothingFactor;

    void drawFFTWaterfall(const double* fft, int bins);
    void drawWaveform(const int16_t* waveform, int samples);
    void drawDebugInfo(const String& reason);
};

#endif
