#pragma once
#include "Widget.h"
#include "WidgetColorTheme.h"
#include "ThemeManager.h"

class VerticalBarWidget : public Widget {
private:
    String label;
    float value; // Expected to be between 0.0 and 1.0
    const WidgetColorTheme& theme;
    bool beatPulse;

public:
    VerticalBarWidget(const String& lbl, float val, const WidgetColorTheme& themeRef = ThemeManager::get(), bool pulse = false)
        : label(lbl), value(val), theme(themeRef), beatPulse(pulse) {}

    void draw(TFT_eSPI& tft, int x, int y, int width, int height) override {
        Serial.printf("[VerticalBarWidget] draw: %s=%.2f at (%d,%d,%d,%d)\n", label.c_str(), value, x, y, width, height);
        // Draw background
        tft.fillRect(x, y, width, height, theme.background);

        // Clamp value to [0.0, 1.0]
        float clampedValue = value;
        if (clampedValue < 0.0f) clampedValue = 0.0f;
        if (clampedValue > 1.0f) clampedValue = 1.0f;

        // Calculate bar height
        int barHeight = static_cast<int>(clampedValue * height);
        int barY = y + height - barHeight;

        // Beat pulse effect (optional, can be triggered externally)
        uint16_t barCol = theme.primary;
        if (beatPulse) {
            // Simple pulse: brighten the bar color
            barCol = theme.accent;
        }

        // Draw the bar
        tft.fillRect(x, barY, width, barHeight, barCol);

        // Draw rotated label using a sprite
        TFT_eSprite sprite = TFT_eSprite(&tft);
        sprite.setColorDepth(8);
        sprite.createSprite(height, width);
        sprite.fillSprite(theme.background);
        sprite.setTextColor(theme.text, theme.background);
        sprite.setTextDatum(MC_DATUM);
        sprite.setTextSize(1);
        sprite.drawString(label, height / 2, width / 2);
        sprite.pushRotated(270, theme.background); // Only angle and transparent color
        sprite.pushSprite(x, y);          // Draw at correct position
        sprite.deleteSprite();
    }

    int getMinWidth() const override { return 20; }
    int getMinHeight() const override { return 30; }
    const WidgetColorTheme& getTheme() const override { return theme; }
    int getTypeId() const override { return 1; }
};
