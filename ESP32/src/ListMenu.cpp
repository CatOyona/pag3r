#include "ListMenu.h"
#include <Adafruit_ST7789.h>   // couleurs ST77XX_*
#include "Config.h"
#include "TextUtils.h"

namespace {
    constexpr int kRowH        = 25;
    constexpr int kVisibleRows = 4;
    constexpr int kMarginX     = 6;
}  // namespace

ListMenu::ListMenu(LabelFn label, int count, ScreenId onSelect)
    : _label(label), _count(count), _onSelect(onSelect) {}

void ListMenu::setCount(int count) {
    _count = count;
    select(_sel);               // recale la sélection si la liste a rétréci
}

void ListMenu::onEnter(int /*arg*/) {
    select(_sel);               // on garde la sélection d'avant, en la recadrant si besoin
}

void ListMenu::select(int index) {
    if (_count <= 0) { _sel = 0; _first = 0; _dirty = true; return; }

    if (index < 0) index = 0;
    if (index > _count - 1) index = _count - 1;
    _sel = index;

    // La fenêtre ne bouge que si la sélection en sort
    if (_sel < _first) _first = _sel;
    if (_sel >= _first + kVisibleRows) _first = _sel - kVisibleRows + 1;
    if (_first < 0) _first = 0;

    _dirty = true;
}

Transition ListMenu::handleInput(Buttons &b) {
    if (_count > 0) {
        if (b.pressed(Btn::Up))     select(_sel > 0 ? _sel - 1 : _count - 1);
        if (b.pressed(Btn::Down))   select(_sel < _count - 1 ? _sel + 1 : 0);
        if (b.pressed(Btn::Action)) return Transition::push(_onSelect, _sel);
    }
    if (b.longPressed(Btn::Action))    return Transition::pop();

    return Transition::none();
}

void ListMenu::draw(GFXcanvas16 &c, U8G2_FOR_ADAFRUIT_GFX &g) {
    g.setFontMode(1);   // fond transparent

    if (_count <= 0) {
        //g.setFont(u8g2_font_helvR12_tf);
        //g.setForegroundColor(ST77XX_BLACK);
        setFont(g, u8g2_font_helvR12_tf, ST77XX_BLACK);
        g.drawUTF8(kMarginX, cfg::HEADER_H + 24, "(vide)");
        return;
    }

    const int maxW = cfg::SCREEN_W - 2 * kMarginX;

    for (int row = 0; row < kVisibleRows; row++) {
        const int i = _first + row;
        if (i >= _count) break;

        const int  y = cfg::HEADER_H + row * kRowH;
        const bool selected = (i == _sel);

        if (selected) {
            c.fillRect(0, y, cfg::SCREEN_W, kRowH, 0xbdf7);
        }

        char raw[48], line[48];
        _label(i, raw, sizeof(raw));
        //g.setFont(selected ? u8g2_font_helvB12_tf : u8g2_font_helvR12_tf);
        //g.setForegroundColor(selected ? ST77XX_WHITE : ST77XX_BLACK);
        setFont(g, selected ? u8g2_font_helvB12_tf : u8g2_font_helvR12_tf, ST77XX_BLACK);
        textutils::fitText(g, raw, line, sizeof(line), maxW);   // coupe avec "..." si trop long
        g.drawUTF8(kMarginX, y + 17, line);
    }
}

FooterHints ListMenu::footerHints() const {
    FooterHints h;
    h.up    = "Haut";
    h.down  = "Bas";
    h.press = (_count > 0) ? "Ouvrir" : nullptr;   // dépend de l'état de l'écran
    h.hold  = "Retour";
    return h;
}