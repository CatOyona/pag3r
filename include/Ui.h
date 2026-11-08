#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "Buttons.h"
#include "Battery.h"
#include "Icons.h"
#include "ScreenManager.h"
#include "ListMenu.h"
#include "DetailScreen.h"
#include "IconMenu.h"

class Ui {
    public:
        Ui();
        void begin();
        void handleInput(Buttons &btns);
        void updateBattery(Battery &b);
        void draw();

    private:
        void drawHeader();
        void drawFooter();
        void drawIcon(int x, int y, const uint16_t *icon, int w, int h);
        void setFont(const uint8_t * font, uint16_t color);

        Adafruit_ST7789 _tft;
        GFXcanvas16     _canvas;

        ScreenManager   _manager;
        ListMenu        _notifList;
        DetailScreen    _notifDetail;
        IconMenu        _mainMenu;

        int      _batteryPercentage = -1;
        bool     _dirty = true;
        uint32_t _lastSecond = 0xFFFFFFFF;
        const uint16_t* _batteryIcon = ICON_BATTERY_UNKNOWN;
        U8G2_FOR_ADAFRUIT_GFX _u8g2;
};