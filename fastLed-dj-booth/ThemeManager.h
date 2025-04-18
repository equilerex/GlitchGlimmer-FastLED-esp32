#pragma once
#include "WidgetColorTheme.h"

class ThemeManager {
public:
    static void setTheme(const WidgetColorTheme& theme) {
        currentTheme = theme;
    }

    static const WidgetColorTheme& get() {
        return currentTheme;
    }

private:
    static WidgetColorTheme currentTheme;
};
