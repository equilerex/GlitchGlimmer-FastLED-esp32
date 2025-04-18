// AcronymValueWidget.h
#pragma once
#include "Widget.h"

class AcronymValueWidget : public Widget {
private:
    String acronym;
    int value;
public:
    AcronymValueWidget(const String& acr, int val) : acronym(acr), value(val) {}
    void draw(TFT_eSPI& tft, int x, int y, int width, int height) override {
        tft.setTextSize(2);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setCursor(x, y);
        tft.printf("%s: %d", acronym.c_str(), value);
    }
    int getMinWidth() const override { return 80; }
    int getMinHeight() const override { return 20; }
};
