#pragma once
#include "Widget.h"

class ModeIndicatorWidget : public Widget {
private:
    bool isAuto;

public:
    ModeIndicatorWidget(bool autoMode) : isAuto(autoMode) {}

    void draw(TFT_eSPI& tft, int x, int y, int width, int height) override {
        uint16_t fillColor = isAuto ? TFT_GREEN : TFT_ORANGE;
        String modeText = isAuto ? "AUTO" : "MANUAL";

        tft.fillRoundRect(x, y, width, height, 4, fillColor);
        tft.setTextColor(TFT_WHITE, fillColor);
        tft.setTextDatum(MC_DATUM);
        tft.setTextSize(1);
        tft.drawString(modeText, x + width / 2, y + height / 2);
    }

    int getMinWidth() const override { return 70; }
    int getMinHeight() const override { return 20; }
};
