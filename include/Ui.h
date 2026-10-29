#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "Buttons.h"

// Interface de test : liste de fausses notifications + écran de détail.
// Plus tard, les données viendront d'un NotificationStore alimenté en BLE.
class Ui {
public:
    Ui();
    void begin();
    void handleInput(Buttons &btns);
    void draw();

private:
    enum class Screen { List, Detail };

    void drawHeader();
    void drawList();
    void drawDetail();
    void drawFooter();
    void setEvent(const char *name);

    Adafruit_ST7789 _tft;
    GFXcanvas16     _canvas;  // dessin hors écran, puis envoi d'un bloc : pas de scintillement

    Screen   _screen = Screen::List;
    int      _sel = 0;
    bool     _dirty = true;
    uint32_t _lastSecond = 0xFFFFFFFF;
    char     _event[24] = "-";
};
