#pragma once
#include "Widget.h"
#include "WidgetColorTheme.h"
#include "ThemeManager.h"

class WaveformWidget : public Widget {
private:
    const int16_t* waveform;
    int samples;
    const WidgetColorTheme& theme;
    bool beatPulse;

public:
    WaveformWidget(const int16_t* wf, int samp, const WidgetColorTheme& themeRef = ThemeManager::get(), bool pulseOnBeat = false)
        : waveform(wf), samples(samp), theme(themeRef), beatPulse(pulseOnBeat) {}

    void draw(TFT_eSPI& tft, int x, int y, int width, int height) override {
        Serial.printf("[WaveformWidget] draw: samples=%d, ptr=%p at (%d,%d,%d,%d)\n", samples, (void*)waveform, x, y, width, height);

        // More aggressive validation
        if (!waveform) {
            Serial.println("[WaveformWidget] ERROR: Null waveform pointer!");
            return;
        }
        if (samples <= 1) {
            Serial.println("[WaveformWidget] ERROR: Invalid sample count!");
            return;
        }
        // Check if pointer seems valid (basic validity test)
        if ((uintptr_t)waveform < 0x3FF00000 || (uintptr_t)waveform >= 0x40000000) {
            Serial.printf("[WaveformWidget] ERROR: Suspicious waveform pointer value: %p\n", waveform);
            return;
        }

        int baseY = y + height / 2;
        uint16_t fillColor = theme.primary;
        uint16_t lineColor = theme.accent;

        // Optional outline
        tft.drawRect(x - 1, y - 1, width + 2, height + 2, theme.secondary);

        for (int i = 0; i < width - 1; ++i) {
            int idx1 = map(i, 0, width - 1, 0, samples - 1);
            int idx2 = map(i + 1, 0, width - 1, 0, samples - 1);

            int s1 = map(waveform[idx1], -32768, 32767, -height / 2, height / 2);
            int s2 = map(waveform[idx2], -32768, 32767, -height / 2, height / 2);

            // Fill under waveform
            tft.drawLine(x + i, baseY + s1, x + i, baseY, fillColor);
            // Draw waveform line
            tft.drawLine(x + i, baseY + s1, x + i + 1, baseY + s2, lineColor);
        }
    }

    int getMinWidth() const override { return 100; }
    int getMinHeight() const override { return 20; }

    const WidgetColorTheme& getTheme() const override { return theme; }
    int getTypeId() const override { return 3; }

    void triggerPulse() {
        // ...existing code...
    }
};
