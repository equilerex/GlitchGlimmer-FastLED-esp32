#pragma once
#include "Widget.h"
#include "WidgetColorTheme.h"
#include "ThemeManager.h"

class AcronymValueWidget : public Widget {
private:
    String acronym;
    int value;
    const WidgetColorTheme& theme;
public:
    AcronymValueWidget(const String& acr, int val, const WidgetColorTheme& themeRef = ThemeManager::get())
        : acronym(acr), value(val), theme(themeRef) {}
    void draw(TFT_eSPI& tft, int x, int y, int width, int height) override {
        Serial.printf("[AcronymValueWidget] draw: %s=%d at (%d,%d,%d,%d)\n", acronym.c_str(), value, x, y, width, height);
        if (acronym.length() == 0) {
            Serial.println("[AcronymValueWidget] ERROR: acronym is empty!");
            return;
        }
        tft.fillRect(x, y, width, height, theme.background);
        tft.setTextSize(2);
        tft.setTextColor(theme.text, theme.background);
        tft.setCursor(x, y);
        tft.printf("%s: %d", acronym.c_str(), value);
    }
    int getMinWidth() const override { return 80; }
    int getMinHeight() const override { return 20; }
    const WidgetColorTheme& getTheme() const override { return theme; }
    int getTypeId() const override { return 2; }
};
