#pragma once
#include "Screen.h"
#include "PhoneStatus.h"

class PhoneScreen : public Screen {
    public:
        explicit PhoneScreen(const PhoneStatus& status);
        void onEnter(int arg) override;
        Transition handleInput(Buttons &b) override;
        void draw(GFXcanvas16 &c, U8G2_FOR_ADAFRUIT_GFX &g) override;
        FooterHints footerHints() const override;
    private:
        const PhoneStatus &_status;   // la source (remplie par BleLink)
        PhoneStatus        _shown;    // ce qui est à l'écran en ce moment
};