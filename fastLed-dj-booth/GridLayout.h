// GridLayout.h
#pragma once

#include <vector>
#include <TFT_eSPI.h>
#include "Widget.h" // Include the header file where Widget is defined

class GridLayout {
private:
    int _width, _height;
    std::vector<Widget*> widgets;

public:
    GridLayout(int screenWidth, int screenHeight) : _width(screenWidth), _height(screenHeight) {}

    ~GridLayout() {
        clear();
    }

    void clear() {
        for (auto widget : widgets) {
            delete widget;
        }
        widgets.clear();
    }

    void addWidget(Widget* widget) {
        widgets.push_back(widget);
    }

    void draw(TFT_eSPI& tft) {
        int x = 0, y = 0;
        int rowHeight = 0;
        for (auto widget : widgets) {
            int w = widget->getMinWidth();
            int h = widget->getMinHeight();
            if (x + w > _width) {
                x = 0;
                y += rowHeight;
                rowHeight = 0;
            }
            widget->draw(tft, x, y, w, h);
            x += w;
            if (h > rowHeight) rowHeight = h;
        }
    }
};
