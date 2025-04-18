#include "DisplayManager.h"
#include "HybridController.h"
#include "Config.h"
#include "AcronymValueWidget.h"
#include "WaveformWidget.h"
#include "VerticalBarWidget.h"
#include <Arduino.h>

// Cyberpunk theme colors
#define COLOR_PURPLE 0x780F
#define COLOR_PINK   0xF81F
#define COLOR_YELLOW 0xFFE0

// Define custom WidgetColorTheme instances for color variety
static WidgetColorTheme purpleTheme { TFT_PURPLE, TFT_WHITE, TFT_PINK, TFT_BLACK };
static WidgetColorTheme pinkTheme   { TFT_PINK,   TFT_WHITE, TFT_YELLOW, TFT_BLACK };
static WidgetColorTheme yellowTheme { TFT_YELLOW, TFT_BLACK, TFT_PINK,   TFT_BLACK };
static WidgetColorTheme redTheme    { TFT_RED,    TFT_WHITE, TFT_YELLOW, TFT_BLACK };
static WidgetColorTheme blueTheme   { TFT_BLUE,   TFT_WHITE, TFT_CYAN,   TFT_BLACK };
static WidgetColorTheme orangeTheme { TFT_ORANGE, TFT_BLACK, TFT_YELLOW, TFT_BLACK };
static WidgetColorTheme cyanTheme   { TFT_CYAN,   TFT_BLACK, TFT_WHITE,  TFT_BLACK };
static WidgetColorTheme magentaTheme{ TFT_MAGENTA,TFT_WHITE, TFT_YELLOW, TFT_BLACK };

// ReasonTextWidget with required methods
class ReasonTextWidget : public Widget {
    String label;
    String reason;
    const WidgetColorTheme& theme;
public:
    ReasonTextWidget(const String& lbl, const String& reasonText, const WidgetColorTheme& themeRef = ThemeManager::get())
        : label(lbl), reason(reasonText), theme(themeRef) {}
    void draw(TFT_eSPI& tft, int x, int y, int w, int h) override {
        tft.fillRect(x, y, w, h, theme.background);
        tft.setTextColor(theme.text, theme.background);
        tft.setTextSize(1);
        tft.setCursor(x, y);
        tft.print(label + ": " + reason);
    }
    int getMinWidth() const override { return 120; }
    int getMinHeight() const override { return 18; }
    const WidgetColorTheme& getTheme() const override { return theme; }
    int getTypeId() const override { return 5; }
};

DisplayManager::DisplayManager(TFT_eSPI &display)
    : _tft(display), layout(display.width(), display.height()) {}

void DisplayManager::showStartupScreen() {
    _tft.fillScreen(TFT_BLACK);
    _tft.setTextColor(TFT_WHITE);
    _tft.setTextSize(2);
    _tft.setCursor(20, 30);
    _tft.print("LED DJ BOOTH");
    _tft.setTextSize(1);
    _tft.setCursor(20, 60);
    _tft.print("Initializing...");
}

void DisplayManager::updateAudioVisualization(const AudioFeatures& features, HybridController* hybrid) {
    Serial.println("[DisplayManager] Drawing new frame");
    _tft.fillScreen(TFT_BLACK);
    layout.clear();

    Serial.printf("[DisplayManager] features: vol=%.3f, bass=%.3f, mid=%.3f, treb=%.3f, beat=%d, bpm=%.2f, loud=%d\n",
        features.volume, features.bass, features.mid, features.treble, features.beatDetected, features.bpm, features.loudness);

    const WidgetColorTheme& pulseTheme = features.beatDetected ? pinkTheme : purpleTheme;

    layout.addWidget(std::unique_ptr<VerticalBarWidget>(new VerticalBarWidget("BASS", features.bass, purpleTheme, true)));
    layout.addWidget(std::unique_ptr<VerticalBarWidget>(new VerticalBarWidget("MID", features.mid, yellowTheme, true)));
    layout.addWidget(std::unique_ptr<VerticalBarWidget>(new VerticalBarWidget("TREB", features.treble, pinkTheme, true)));
    layout.addWidget(std::unique_ptr<VerticalBarWidget>(new VerticalBarWidget("PWR", features.loudness / 100.0f, redTheme, true)));
    layout.addWidget(std::unique_ptr<AcronymValueWidget>(new AcronymValueWidget("BPM", static_cast<int>(features.bpm), pulseTheme)));
    layout.addWidget(std::unique_ptr<AcronymValueWidget>(new AcronymValueWidget("PWR", static_cast<int>(features.loudness), purpleTheme)));
    
    // Explicitly check the waveform pointer
    Serial.printf("[DisplayManager] Waveform pointer: %p\n", features.waveform);
    if (!features.waveform) {
        Serial.println("[DisplayManager] WARNING: Null waveform pointer in features!");
    } else {
        try {
            layout.addWidget(std::unique_ptr<WaveformWidget>(new WaveformWidget(features.waveform, NUM_SAMPLES, magentaTheme, features.beatDetected)));
            Serial.println("[DisplayManager] Added waveform widget successfully");
        } catch (const std::exception& e) {
            Serial.printf("[DisplayManager] Exception creating waveform widget: %s\n", e.what());
        } catch (...) {
            Serial.println("[DisplayManager] Unknown exception creating waveform widget");
        }
    }

    if (hybrid) {
        layout.addWidget(std::unique_ptr<AcronymValueWidget>(new AcronymValueWidget("IDX", hybrid->getCurrentIndex() + 1, yellowTheme)));
        layout.addWidget(std::unique_ptr<AcronymValueWidget>(new AcronymValueWidget("TOT", hybrid->getAnimationCount(), pinkTheme)));
        layout.addWidget(std::unique_ptr<AcronymValueWidget>(new AcronymValueWidget(hybrid->isAutoSwitchEnabled() ? "AUTO" : "MAN", 1, blueTheme)));
        layout.addWidget(std::unique_ptr<AcronymValueWidget>(new AcronymValueWidget("KEEP", 1, orangeTheme)));
        layout.addWidget(std::unique_ptr<ReasonTextWidget>(new ReasonTextWidget("KEEP REASON", hybrid->getModeKeepReason(), cyanTheme)));
    }

    // Draw all widgets in a vertical stack (no direct access to widgets)
    layout.drawVerticalStack(_tft);
}