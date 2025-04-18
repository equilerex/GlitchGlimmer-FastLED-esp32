// GridLayout.h
#pragma once

#include <vector>
#include <memory>
#include <TFT_eSPI.h>
#include "Widget.h" // Include the header file where Widget is defined
#include "AcronymValueWidget.h"
#include "VerticalBarWidget.h"
#include "WaveformWidget.h"
#include "ModeIndicatorWidget.h"

class GridLayout {
private:
    int _width, _height;
    static constexpr size_t MAX_WIDGETS = 16;
    std::vector<std::unique_ptr<Widget>> widgets;

public:
    GridLayout(int screenWidth, int screenHeight) : _width(screenWidth), _height(screenHeight) {}

    void clear() {
        widgets.clear();
    }

    void addWidget(std::unique_ptr<Widget> widget) {
        if (widgets.size() < MAX_WIDGETS) {
            widgets.push_back(std::move(widget));
        }
    }

    void draw(TFT_eSPI& tft) {
        int x = 0, y = 0;
        int rowHeight = 0;
        const int margin = 2;
        for (size_t i = 0; i < widgets.size(); ++i) {
            Widget* widget = widgets[i].get();

            // verify widgey, log error 
            if (!widget) {
                Serial.printf("[GridLayout] ERROR: Widget %zu is null!\n", i);
                continue;
            }
            int w = widget->getMinWidth();
            int h = widget->getMinHeight();
            if (x + w > _width) {
                x = 0;
                y += rowHeight + margin;
                rowHeight = 0;
                // debugging info
                Serial.printf("[GridLayout] Moving to next row at (%d, %d)\n", x, y);
            }
            // debugging info
            Serial.printf("[GridLayout] Drawing widget %zu at (%d, %d) with size (%d, %d)\n", i, x, y, w, h);
           
            widget->draw(tft, x, y, w, h);
            x += w + margin;
            if (h > rowHeight) rowHeight = h;
            // add debugging info
            Serial.printf("[GridLayout] Widget %zu drawn at (%d, %d) with size (%d, %d)\n", i, x, y, w, h);
        }
    }

    void drawVerticalStack(TFT_eSPI& tft) {
        int y = 0;
        int widgetWidth = _width;
        const int margin = 2;
        for (size_t i = 0; i < widgets.size(); ++i) {
            Widget* widget = widgets[i].get();
            if (widget) {
                int widgetHeight = widget->getMinHeight();
                uint16_t bgColor = TFT_BLACK;
                // Use getTypeId() and static_cast (no dynamic_cast, no RTTI)
                switch (widget->getTypeId()) {
                    case 1: // VerticalBarWidget
                    case 2: // AcronymValueWidget
                    case 3: // WaveformWidget
                    case 4: // ModeIndicatorWidget
                        bgColor = widget->getTheme().background;
                        break;
                    default:
                        bgColor = TFT_BLACK;
                        break;
                }
                if (widgetWidth > 0 && widgetHeight > 0) {
                    tft.fillRect(0, y, widgetWidth, widgetHeight, bgColor);
                    widget->draw(tft, 0, y, widgetWidth, widgetHeight);
                    y += widgetHeight + margin;
                } else {
                    Serial.printf("[GridLayout] Skipping widget %zu due to invalid size (%d,%d)\n", i, widgetWidth, widgetHeight);
                }
            }
        }
    }
};
