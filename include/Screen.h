#pragma once
#include <Adafruit_GFX.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "Buttons.h"
#include "Transition.h"

class Screen {
    public:
        virtual ~Screen() = default;
        virtual void onEnter(int /*arg*/) {}
        virtual Transition handleInput(Buttons &b) = 0;
        virtual void draw(GFXcanvas16 &c, U8G2_FOR_ADAFRUIT_GFX &g) = 0;
        bool isDirty() const { return _dirty; }
        void clearDirty() { _dirty = false; }
        void invalidate() { _dirty = true; }
    protected:
        bool _dirty = true; 
};