#pragma once
#include "Screen.h"
#include "Notification.h"

class DetailScreen : public Screen {
public:
    using NotifFn = const Notif& (*)(int index);   // donne la notification numéro index

    explicit DetailScreen(NotifFn get);

    void onEnter(int arg) override;                // arg = index envoyé par la liste
    Transition handleInput(Buttons &b) override;
    void draw(GFXcanvas16 &c, U8G2_FOR_ADAFRUIT_GFX &g) override;
    FooterHints footerHints() const override;

private:
    NotifFn _get;
    int     _index = 0;
};