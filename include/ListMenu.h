#pragma once
#include "Screen.h"

class ListMenu: public Screen {
    public:
        void onEnter(int /*arg*/) {}
        Transition handleInput(Buttons &b);
        void draw(GFXcanvas16 &c, U8G2_FOR_ADAFRUIT_GFX &g) = 0;
    private:
        constexpr int kRowH = 25;
        constexpr uint16_t kGrey = 0x7BEF;
        constexpr int kVisibleRows = 4;
};