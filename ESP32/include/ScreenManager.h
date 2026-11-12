#pragma once
#include "Screen.h"
#include "ScreenStack.h"
#include "Transition.h"

class ScreenManager {
    public:
        void registerScreen(ScreenId id, Screen* screen);
        void begin(ScreenId root);
        void handleInput(Buttons &b);
        void draw(GFXcanvas16 &c, U8G2_FOR_ADAFRUIT_GFX &g);
        bool isDirty() const;
        FooterHints footerHints() const;
    private:
        void apply(const Transition &t);
        Screen* find(ScreenId id) const;

        static constexpr int kCount = static_cast<int>(ScreenId::Count);
        Screen* _screens[kCount] = {};
        ScreenStack _stack;
};