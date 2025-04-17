
#include "DisplayManager.h"
#include "HybridController.h"
#include "ModeManager.h"
#include "Config.h"
#include <Arduino.h>

// External reference to the global modeManager instance
extern ModeManager modeManager;

DisplayManager::DisplayManager(TFT_eSPI &display)
    : _tft(display), ringRadius(30), ringFade(0), smoothedBPM(0), bpmSmoothingFactor(0.1) {
}

void DisplayManager::showStartupScreen() {
    _tft.fillScreen(TFT_BLACK);
    _tft.setTextColor(TFT_WHITE);
    _tft.setTextSize(2);
    _tft.setCursor(20, 30);
    _tft.print("LED DJ BOOTH");
    _tft.setTextSize(1);
    _tft.setCursor(20, 60);
    _tft.print("Initializing...");
}

void DisplayManager::updateMode(const String& modeName) {
    lastModeName = modeName;
}

void DisplayManager::updateMode(const String& modeName, bool isAuto, int index, int total) {
    lastModeName = modeName;
    _tft.fillRect(0, 0, 240, 30, TFT_NAVY);
    _tft.setTextSize(2);
    _tft.setTextColor(TFT_WHITE);
    _tft.setCursor(5, 7);
    _tft.print(modeName);

    _tft.setTextSize(1);
    _tft.setCursor(180, 12);
    _tft.printf("%d/%d", index + 1, total);
}

void DisplayManager::drawFFTWaterfall(const double* fft, int bins) {
    const int START_X = 101;
    const int START_Y = 95;
    const int WIDTH = 128;
    const int HEIGHT = 58;

    // Draw 16 frequency bands
    int bandWidth = WIDTH / 16;
    int binsPerBand = bins / 16;

    for (int band = 0; band < 16; band++) {
        double bandValue = 0;
        // Average the bins in this band
        for (int i = 0; i < binsPerBand; i++) {
            bandValue += fft[band * binsPerBand + i];
        }
        bandValue /= binsPerBand;

        int barHeight = constrain((int)(bandValue * HEIGHT), 0, HEIGHT);
        _tft.fillRect(
            START_X + (band * bandWidth),
            START_Y - barHeight,
            bandWidth - 1,
            barHeight,
            TFT_GREEN
        );
    }
}

void DisplayManager::updateAudioVisualization(const AudioFeatures& features, HybridController* hybrid) {
    // Clear screen with a dark background
    _tft.fillScreen(TFT_BLACK);

    // Header section - Mode info
    _tft.fillRect(0, 0, 240, 30, TFT_NAVY);
    _tft.setTextSize(2);
    _tft.setTextColor(TFT_WHITE);
    _tft.setCursor(5, 7);
    if (hybrid) {
        _tft.print(hybrid->getCurrentName());
    } else {
        _tft.print(lastModeName);
    }

    // Progress indicator
    int total = hybrid ? hybrid->getAnimationCount() : modeManager.getModeCount();
    int current = hybrid ? hybrid->getCurrentIndex() + 1 : modeManager.getCurrentIndex() + 1;
    _tft.setTextSize(1);
    _tft.setCursor(180, 12);
    _tft.printf("%d/%d", current, total);

    // Mode type indicator (Manual/Hybrid)
    _tft.fillRoundRect(5, 35, 70, 20, 4, 0x0320);  // Dark green
    _tft.setTextColor(TFT_WHITE);
    _tft.setCursor(12, 40);
    _tft.print(hybrid ? "HYBRID" : "MANUAL");

    // BPM Display with visual indicator
    _tft.fillRoundRect(85, 35, 70, 20, 4, features.beatDetected ? TFT_RED : 0x0492);  // Dark cyan
    _tft.setCursor(92, 40);
    _tft.printf("BPM:%d", (int)features.bpm);

    // Volume Level
    _tft.fillRoundRect(165, 35, 70, 20, 4, 0x7BEF);  // Dark grey
    _tft.setCursor(172, 40);
    _tft.printf("dB:%.2f", features.decibels);

    // Audio levels visualization
    // Bass level bar
    int bassHeight = constrain((int)(features.bass * 30), 0, 30);
    _tft.fillRect(10, 65, 20, 30, 0x7BEF);  // Dark grey
    _tft.fillRect(10, 95 - bassHeight, 20, bassHeight, TFT_BLUE);
    _tft.setCursor(8, 100);
    _tft.print("BASS");

    // Treble level bar
    int trebleHeight = constrain((int)(features.treble * 30), 0, 30);
    _tft.fillRect(40, 65, 20, 30, 0x7BEF);  // Dark grey
    _tft.fillRect(40, 95 - trebleHeight, 20, trebleHeight, TFT_YELLOW);
    _tft.setCursor(35, 100);
    _tft.print("TREB");

    // Mid level bar
    int midHeight = constrain((int)(features.mid * 30), 0, 30);
    _tft.fillRect(70, 65, 20, 30, 0x7BEF);  // Dark grey
    _tft.fillRect(70, 95 - midHeight, 20, midHeight, TFT_GREEN);
    _tft.setCursor(70, 100);
    _tft.print("MID");

    // FFT Waterfall
    _tft.drawRect(100, 65, 130, 60, 0x7BEF);  // Dark grey border
    drawFFTWaterfall(features.spectrum, NUM_SAMPLES / 2);
}

void DisplayManager::updateAudioVisualization(const AudioFeatures& features) {
    updateAudioVisualization(features, nullptr);
}

void DisplayManager::drawWaveform(const int16_t* waveform, int samples) {
    const int START_X = 101;
    const int START_Y = 95;
    const int WIDTH = 128;
    const int HEIGHT = 58;

    for (int i = 0; i < WIDTH && i < samples - 1; i++) {
        int sample1 = map(waveform[i], -32768, 32767, -HEIGHT/2, HEIGHT/2);
        int sample2 = map(waveform[i + 1], -32768, 32767, -HEIGHT/2, HEIGHT/2);

        _tft.drawLine(
            START_X + i,
            START_Y + sample1,
            START_X + i + 1,
            START_Y + sample2,
            TFT_GREEN
        );
    }
}

void DisplayManager::drawVolumeBar(double volume) {
    int barHeight = (int)(volume * 100);
    _tft.fillRect(220, 100 - barHeight, 10, barHeight, TFT_GREEN);
}

void DisplayManager::drawTriggerIcons(const AudioFeatures& features) {
    if (features.beatDetected) {
        _tft.fillCircle(220, 20, 5, TFT_RED);
    } else {
        _tft.fillCircle(220, 20, 5, TFT_DARKGREY);
    }
}

void DisplayManager::drawBPM(int bpm) {
    smoothedBPM = (smoothedBPM * (1 - bpmSmoothingFactor)) + (bpm * bpmSmoothingFactor);
    _tft.setTextColor(TFT_WHITE);
    _tft.setTextSize(1);
    _tft.setCursor(180, 40);
    _tft.printf("BPM: %d", (int)smoothedBPM);
}

void DisplayManager::drawBPMRing(bool beatDetected) {
    if (beatDetected) {
        ringFade = 255;
    }

    if (ringFade > 0) {
        _tft.drawCircle(220, 20, ringRadius, _tft.color565(ringFade, ringFade, ringFade));
        ringFade = max(0, ringFade - 20);
    }
}