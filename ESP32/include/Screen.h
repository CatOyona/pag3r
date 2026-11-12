#pragma once
#include <Adafruit_GFX.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "Buttons.h"
#include "Transition.h"

struct FooterHints {
        const char *up    = nullptr;   // bouton Haut
        const char *down  = nullptr;   // bouton Bas
        const char *press = nullptr;   // Action, appui court
        const char *hold  = nullptr;   // Action, appui long
};

class Screen {
    public:
        virtual ~Screen() = default;
        virtual void onEnter(int /*arg*/) {}
        virtual Transition handleInput(Buttons &b) = 0;
        virtual void draw(GFXcanvas16 &c, U8G2_FOR_ADAFRUIT_GFX &g) = 0;
        virtual FooterHints footerHints() const { return FooterHints(); }
        bool isDirty() const { return _dirty; }
        void clearDirty() { _dirty = false; }
        void invalidate() { _dirty = true; }
        void setFont(U8G2_FOR_ADAFRUIT_GFX &g, const uint8_t * font, uint16_t color);
        static void drawIcon(GFXcanvas16 &c, int x, int y, const uint16_t *icon, int w, int h);
    protected:
        bool _dirty = true;
};