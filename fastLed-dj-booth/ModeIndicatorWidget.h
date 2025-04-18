#pragma once
#include "Widget.h"
#include "WidgetColorTheme.h"
#include "ThemeManager.h"

class ModeIndicatorWidget : public Widget {
private:
    bool isAuto;
    const WidgetColorTheme& theme;

public:
    ModeIndicatorWidget(bool autoMode, const WidgetColorTheme& themeRef = ThemeManager::get())
        : isAuto(autoMode), theme(themeRef) {}

    void draw(TFT_eSPI& tft, int x, int y, int width, int height) override {
        uint16_t fillColor = isAuto ? theme.primary : theme.accent;
        String modeText = isAuto ? "AUTO" : "MANUAL";

        tft.fillRoundRect(x, y, width, height, 4, fillColor);
        tft.setTextColor(theme.text, fillColor);
        tft.setTextDatum(MC_DATUM);
        tft.setTextSize(1);
        tft.drawString(modeText, x + width / 2, y + height / 2);
    }

    const WidgetColorTheme& getTheme() const override { return theme; }
    int getTypeId() const override { return 4; }

    int getMinWidth() const override { return 70; }
    int getMinHeight() const override { return 20; }
};
