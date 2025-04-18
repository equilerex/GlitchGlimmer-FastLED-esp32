#include "DisplayManager.h"
#include "HybridController.h"
#include "Config.h"
#include <Arduino.h>
#include "AcronymValueWidget.h"
#include "WaveformWidget.h"
#include "VerticalBarWidget.h"
// Include other widget headers as needed

DisplayManager::DisplayManager(TFT_eSPI &display)
    : _tft(display), layout(display.width(), display.height()) {} // Properly initialize layout

void DisplayManager::drawFFTWaterfall(const double* fft, int bins) {
    const int START_X = 101;
    const int START_Y = 95;
    const int WIDTH = 128;
    const int HEIGHT = 58;

    int bandWidth = WIDTH / 16;
    int binsPerBand = bins / 16;

    for (int band = 0; band < 16; band++) {
        double bandValue = 0;
        for (int i = 0; i < binsPerBand; i++) {
            bandValue += fft[band * binsPerBand + i];
        }
        bandValue /= binsPerBand;

        int barHeight = constrain((int)(bandValue * HEIGHT / 50.0), 0, HEIGHT); // Normalized scale
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
    const int START_X = 101;
    const int START_Y = 80; // Moved down by 20px
    const int WIDTH = 100;
    const int HEIGHT = 20;   // Shortened by 10px

    for (int i = 0; i < WIDTH - 1 && i < samples - 1; i++) {
        int index1 = map(i, 0, WIDTH - 1, 0, samples - 1);
        int index2 = map(i + 1, 0, WIDTH - 1, 0, samples - 1);

        int sample1 = map(waveform[index1], -32768, 32767, -HEIGHT / 2, HEIGHT / 2);
        int sample2 = map(waveform[index2], -32768, 32767, -HEIGHT / 2, HEIGHT / 2);

        _tft.drawLine(
            START_X + i,
            START_Y + sample1,
            START_X + i + 1,
            START_Y + sample2,
            TFT_GREEN
        );
    }
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

void DisplayManager::updateAudioVisualization(const AudioFeatures& features) {
    _tft.fillScreen(TFT_BLACK);

    // Clear previous widgets and re-initialize layout with correct size
    layout = GridLayout(_tft.width(), _tft.height());

    // Add vertical bar widgets for BASS, MID, TREB, PWR
    layout.addWidget(new VerticalBarWidget("BASS", features.bass, TFT_BLUE));
    layout.addWidget(new VerticalBarWidget("MID", features.mid, TFT_GREEN));
    layout.addWidget(new VerticalBarWidget("TREB", features.treble, TFT_YELLOW));
    layout.addWidget(new VerticalBarWidget("PWR", features.loudness / 100.0f, TFT_RED));

    // Add other widgets
    layout.addWidget(new AcronymValueWidget("BPM", static_cast<int>(features.bpm)));
    layout.addWidget(new AcronymValueWidget("PWR", static_cast<int>(features.loudness)));
    // ... add other widgets as needed

    // Add the waveform widget
    layout.addWidget(new WaveformWidget(features.waveform, NUM_SAMPLES));

    // Draw all widgets
    layout.draw(_tft);
}

