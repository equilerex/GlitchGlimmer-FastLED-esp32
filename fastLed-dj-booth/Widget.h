// Widget.h
#pragma once
#include <TFT_eSPI.h>

class Widget {
public:
    virtual void draw(TFT_eSPI& tft, int x, int y, int width, int height) = 0;
    virtual int getMinWidth() const = 0;
    virtual int getMinHeight() const = 0;
    virtual ~Widget() {}
};