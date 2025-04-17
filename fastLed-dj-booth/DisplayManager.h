#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#pragma once
#include <TFT_eSPI.h>
class AudioFeatures;
class HybridController;

class DisplayManager {
public:
    DisplayManager(TFT_eSPI &display);

    void showStartupScreen();

    // Mode update methods
    void updateMode(const String& modeName); // Simple overload
    void updateMode(const String& modeName, bool isAuto, int index, int total);

    // Visual elements
    void drawVolumeBar(double volume);
    void drawTriggerIcons(const AudioFeatures& features);
    void drawFFTWaterfall(const double* fft, int bins);
    void drawWaveform(const int16_t* waveform, int samples);
    void drawBPM(int bpm);
    void drawBPMRing(bool beatDetected);

    // Full screen visual update
    void updateAudioVisualization(const AudioFeatures& features);
    void updateAudioVisualization(const AudioFeatures& features, HybridController* hybrid);

private:
    TFT_eSPI& _tft;
    String lastModeName;

    // BPM ring and smoothing
    int ringRadius;
    int ringFade;
    double smoothedBPM;
    double bpmSmoothingFactor;
};

#endif
