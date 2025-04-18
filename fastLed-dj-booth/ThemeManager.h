#pragma once
#include "WidgetColorTheme.h"

class ThemeManager {
public:
    static void setTheme(const WidgetColorTheme& theme) {
        currentTheme = theme;
    }

    static const WidgetColorTheme& get() {
        static WidgetColorTheme instance; // Persistent static instance
        return instance;
    }

private:
    static WidgetColorTheme currentTheme;
};
