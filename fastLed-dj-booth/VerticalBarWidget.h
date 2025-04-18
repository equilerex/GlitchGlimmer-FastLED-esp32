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
        Serial.printf("[VerticalBarWidget] draw: %s=%.2f at (%d,%d,%d,%d)\n", 
                     label.c_str(), value, x, y, width, height);

        // Safety checks
        if (width <= 2 || height <= 2) {
            Serial.println("[VerticalBarWidget] Invalid dimensions");
            return;
        }

        // Draw background
        tft.fillRect(x, y, width, height, theme.background);

        // Clamp and calculate bar height
        float clampedValue = (value < 0.0f) ? 0.0f : (value > 1.0f ? 1.0f : value);
        int barHeight = static_cast<int>(clampedValue * (height - 2)); // Account for border

        // Draw the bar with border
        if (barHeight > 0) {
            uint16_t barCol = beatPulse ? theme.accent : theme.primary;
            int barY = y + 1 + (height - 2 - barHeight); // Offset for border
            tft.fillRect(x + 1, barY, width - 2, barHeight, barCol);
        }

        // --- Fixed Sprite Handling ---
        TFT_eSprite sprite(&tft); // Stack-allocated
        
        // Validate dimensions before creating sprite
        if (width <= 0 || height <= 0) {
            Serial.println("[ERROR] Invalid dimensions for sprite");
            return;
        }

        // Create sprite with original orientation dimensions
        if (!sprite.createSprite(width, height)) {
            Serial.println("[ERROR] Failed to create sprite");
            return;
        }

        try {
            // Draw label in original orientation
            sprite.fillSprite(theme.background);
            sprite.setTextColor(theme.text, theme.background);
            sprite.setTextDatum(MC_DATUM);
            sprite.setTextSize(1);
            
            // Center in original dimensions
            sprite.drawString(label, width/2, height/2);

            // Single rotation operation with position
            sprite.pushRotated(270, theme.background);
        } 
        catch (...) {
            Serial.println("[ERROR] Sprite operation failed");
        }

        // Automatic cleanup - no delete needed for stack sprite
    }

    int getMinWidth() const override { return 20; }
    int getMinHeight() const override { return 30; }
    const WidgetColorTheme& getTheme() const override { return theme; }
    int getTypeId() const override { return 1; }
};