#pragma once
#include "Widget.h"

class VerticalBarWidget : public Widget {
private:
    String label;
    float value; // Expected to be between 0.0 and 1.0
    uint16_t barColor;
    uint16_t bgColor;

public:
    VerticalBarWidget(const String& lbl, float val, uint16_t barCol, uint16_t bgCol = TFT_DARKGREY)
        : label(lbl), value(val), barColor(barCol), bgColor(bgCol) {}

    void draw(TFT_eSPI& tft, int x, int y, int width, int height) override {
        // Draw background
        tft.fillRect(x, y, width, height, bgColor);

        // Clamp value to [0.0, 1.0]
        float clampedValue = value;
        if (clampedValue < 0.0f) clampedValue = 0.0f;
        if (clampedValue > 1.0f) clampedValue = 1.0f;

        // Calculate bar height
        int barHeight = static_cast<int>(clampedValue * height);
        int barY = y + height - barHeight;

        // Draw the bar
        tft.fillRect(x, barY, width, barHeight, barColor);

        // Draw rotated label using a sprite
        TFT_eSprite sprite = TFT_eSprite(&tft);
        sprite.setColorDepth(8);
        sprite.createSprite(height, width);
        sprite.fillSprite(bgColor);
        sprite.setTextColor(TFT_WHITE, bgColor);
        sprite.setTextDatum(MC_DATUM);
        sprite.setTextSize(1);
        sprite.drawString(label, height / 2, width / 2);
        sprite.pushRotated(270, bgColor); // Only angle and transparent color
        sprite.pushSprite(x, y);          // Draw at correct position
        sprite.deleteSprite();
    }

    int getMinWidth() const override { return 20; }
    int getMinHeight() const override { return 30; }
};
