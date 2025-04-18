#pragma once
#include <TFT_eSPI.h>

// Utility for color debug
inline void debugColor(const char* label, uint16_t color) {
    Serial.printf("[ColorDebug] %s: 0x%04X (R:%d G:%d B:%d)\n", label, color,
        (color >> 11) & 0x1F, (color >> 5) & 0x3F, color & 0x1F);
}

struct WidgetColorTheme {
    uint16_t primary;
    uint16_t secondary;
    uint16_t accent;
    uint16_t background;
    uint16_t text;

    static WidgetColorTheme cyberpunk() {
        WidgetColorTheme t = {
            .primary = TFT_PURPLE,
            .secondary = TFT_PINK,
            .accent = TFT_YELLOW,
            .background = TFT_BLACK,
            .text = TFT_WHITE
        };
        debugColor("cyberpunk.primary", t.primary);
        debugColor("cyberpunk.secondary", t.secondary);
        debugColor("cyberpunk.accent", t.accent);
        debugColor("cyberpunk.background", t.background);
        debugColor("cyberpunk.text", t.text);
        return t;
    }

    static WidgetColorTheme bladeRunner() {
        WidgetColorTheme t = {
            .primary = TFT_SILVER,
            .secondary = TFT_BLUE,
            .accent = TFT_ORANGE,
            .background = TFT_BLACK,
            .text = TFT_WHITE
        };
        debugColor("bladeRunner.primary", t.primary);
        debugColor("bladeRunner.secondary", t.secondary);
        debugColor("bladeRunner.accent", t.accent);
        debugColor("bladeRunner.background", t.background);
        debugColor("bladeRunner.text", t.text);
        return t;
    }
};
