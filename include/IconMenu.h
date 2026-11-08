#pragma once
#include "Screen.h"

// Menu à une entrée à la fois : icône centrée, texte dessous.
// Haut / Bas changent d'entrée, Action ouvre l'écran associé, appui long = retour.
class IconMenu : public Screen {
public:
    struct Item {
        const char     *label;    // texte affiché sous l'icône
        const uint16_t *icon;     // image RGB565 (0x0000 = transparent)
        int             w, h;     // taille de l'icône en pixels
        ScreenId        target;   // écran ouvert quand on valide
    };

    // items : tableau qui doit exister pendant toute la vie du programme
    // (global ou static const), car IconMenu n'en garde qu'un pointeur.
    IconMenu(const Item *items, int count);

    void onEnter(int arg) override;
    Transition handleInput(Buttons &b) override;
    void draw(GFXcanvas16 &c, U8G2_FOR_ADAFRUIT_GFX &g) override;
    FooterHints footerHints() const override;

private:
    void select(int index);

    const Item *_items;
    int         _count;
    int         _sel = 0;
};