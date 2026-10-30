#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "Buttons.h"
#include "Battery.h"
#include "Icons.h"
#include <U8g2_for_Adafruit_GFX.h>

// Interface de test : liste de fausses notifications + écran de détail.
// Plus tard, les données viendront d'un NotificationStore alimenté en BLE.
class Ui {
public:
    Ui();
    void begin();
    void handleInput(Buttons &btns);
    void updateBattery(Battery &b);
    void draw();

private:
    enum class Screen { List, Detail };

    void drawHeader();
    void drawList();
    void drawDetail();
    void drawFooter();
    void setEvent(const char *name);
    void drawIcon(int x, int y, const uint16_t *icon, int w, int h);
    void setFont(const uint8_t * font, uint16_t color);

    Adafruit_ST7789 _tft;
    GFXcanvas16     _canvas;  // dessin hors écran, puis envoi d'un bloc : pas de scintillement

    Screen   _screen = Screen::List;
    int      _sel = 0;
    int      _batteryPercentage = -1;
    bool     _dirty = true;
    uint32_t _lastSecond = 0xFFFFFFFF;
    char     _event[24] = "-";
    const uint16_t* _batteryIcon = ICON_BATTERY_UNKNOWN;
    U8G2_FOR_ADAFRUIT_GFX _u8g2;
};
