#include "DisplayManager.h"
#include "HybridController.h"
#include "Config.h"
#include <Arduino.h>

DisplayManager::DisplayManager(TFT_eSPI &display)
    : _tft(display), ringRadius(30), ringFade(0), smoothedBPM(0), bpmSmoothingFactor(0.1) {}

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

void DisplayManager::drawWaveform(const int16_t* waveform, int samples) {
    const int WIDTH = 240;
    const int HEIGHT = 50;
    const int CENTER_Y = 140;

    for (int i = 0; i < WIDTH - 1 && i < samples - 1; i++) {
        int16_t sample1 = waveform[i];
        int16_t sample2 = waveform[i + 1];

        int y1 = map(sample1, -32768, 32767, CENTER_Y - HEIGHT/2, CENTER_Y + HEIGHT/2);
        int y2 = map(sample2, -32768, 32767, CENTER_Y - HEIGHT/2, CENTER_Y + HEIGHT/2);

        _tft.drawLine(i, y1, i + 1, y2, TFT_CYAN);
    }
}

void DisplayManager::drawDebugInfo(const String& reason) {
    _tft.setTextSize(1);
    _tft.setTextColor(TFT_LIGHTGREY);
    _tft.setCursor(5, 190);
    _tft.print("Keep Reason: ");
    _tft.print(reason);
}

void DisplayManager::updateAudioVisualization(const AudioFeatures& features, HybridController* hybrid) {
    // Clear screen with a dark background
    _tft.fillScreen(TFT_BLACK);

    // Header section - Mode info
    _tft.fillRect(0, 0, 240, 30, TFT_NAVY);
    _tft.setTextSize(2);
    _tft.setTextColor(TFT_WHITE);
    _tft.setCursor(5, 7);
    _tft.print(hybrid ? hybrid->getCurrentName() : "??");


    // Mode info (auto/manual)
    int current = hybrid ? hybrid->getCurrentIndex() + 1 : 0;
    int total = hybrid ? hybrid->getAnimationCount() : 0;
    _tft.setTextSize(1);
    _tft.setCursor(180, 12);
    _tft.printf("%d/%d", current, total);

    _tft.fillRoundRect(5, 35, 70, 20, 4, hybrid->autoSwitchEnabled ? TFT_GREEN : TFT_ORANGE);
    _tft.setTextColor(TFT_WHITE);
    _tft.setCursor(12, 40);
    _tft.print(hybrid->autoSwitchEnabled ? "AUTO" : "MANUAL");

    // BPM
    _tft.fillRoundRect(85, 35, 70, 20, 4, features.beatDetected ? TFT_RED : TFT_BLUE);
    _tft.setCursor(92, 40);
    _tft.printf("BPM:%d", (int)features.bpm);

    // PWR
    _tft.fillRoundRect(165, 35, 70, 20, 4, TFT_DARKGREY);
    _tft.setCursor(172, 40);
    _tft.printf("PWR:%d", (int)features.loudness);

    // Spectrum Bars
    int bassHeight = constrain((int)(features.bass * 30), 0, 30);
    _tft.fillRect(10, 65, 20, 30, TFT_DARKGREY);
    _tft.fillRect(10, 95 - bassHeight, 20, bassHeight, TFT_BLUE);
    _tft.setCursor(8, 100); _tft.print("BASS");

    int trebleHeight = constrain((int)(features.treble * 30), 0, 30);
    _tft.fillRect(40, 65, 20, 30, TFT_DARKGREY);
    _tft.fillRect(40, 95 - trebleHeight, 20, trebleHeight, TFT_YELLOW);
    _tft.setCursor(35, 100); _tft.print("TREB");

    int midHeight = constrain((int)(features.mid * 30), 0, 30);
    _tft.fillRect(70, 65, 20, 30, TFT_DARKGREY);
    _tft.fillRect(70, 95 - midHeight, 20, midHeight, TFT_GREEN);
    _tft.setCursor(70, 100); _tft.print("MID");

    _tft.drawRect(100, 65, 130, 60, TFT_DARKGREY);
    drawFFTWaterfall(features.spectrum, NUM_SAMPLES / 2);

    // Draw waveform at bottom
    drawWaveform(features.waveform, NUM_SAMPLES);

    // Reason for keeping current mode
    if (hybrid) drawDebugInfo(hybrid->getModeKeepReason());
}

void DisplayManager::updateAudioVisualization(const AudioFeatures& features) {
    updateAudioVisualization(features, nullptr);
}
