// WaveformWidget.h
#pragma once
#include "Widget.h"

class WaveformWidget : public Widget {
private:
    const int16_t* waveform;
    int samples;
    uint16_t color;

public:
    WaveformWidget(const int16_t* wf, int samp, uint16_t col = TFT_GREEN)
        : waveform(wf), samples(samp), color(col) {}

    void draw(TFT_eSPI& tft, int x, int y, int width, int height) override {
        if (!waveform || samples <= 1) return;

        for (int i = 0; i < width - 1; ++i) {
            int index1 = map(i, 0, width - 1, 0, samples - 1);
            int index2 = map(i + 1, 0, width - 1, 0, samples - 1);

            int sample1 = map(waveform[index1], -32768, 32767, -height / 2, height / 2);
            int sample2 = map(waveform[index2], -32768, 32767, -height / 2, height / 2);

            tft.drawLine(
                x + i,
                y + height / 2 + sample1,
                x + i + 1,
                y + height / 2 + sample2,
                color
            );
        }
    }

    int getMinWidth() const override { return 100; }
    int getMinHeight() const override { return 20; }
};
