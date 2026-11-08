#include "IconMenu.h"
#include <Adafruit_ST7789.h>   // couleurs ST77XX_*
#include "Config.h"
#include "TextUtils.h"

namespace {
    constexpr int      kMarginX = 6;
    constexpr int      kGap     = 6;        // espace entre l'icône et le texte
    constexpr int      kTextH   = 15;       // hauteur du texte (helvB12) : 12 au-dessus de la ligne de base + 3 en dessous
    constexpr int      kDotStep = 10;       // écart entre deux points de l'indicateur
    constexpr uint16_t kDotOff  = 0xbdf7;   // gris clair
}  // namespace

IconMenu::IconMenu(const Item *items, int count)
    : _items(items), _count(count) {}

void IconMenu::onEnter(int /*arg*/) {
    select(_sel);               // on garde l'entrée d'avant
}

void IconMenu::select(int index) {
    if (_count <= 0) { _sel = 0; _dirty = true; return; }

    if (index < 0) index = 0;
    if (index > _count - 1) index = _count - 1;
    _sel = index;
    _dirty = true;
}

Transition IconMenu::handleInput(Buttons &b) {
    if (_count > 0) {
        if (b.pressed(Btn::Up))     select(_sel > 0 ? _sel - 1 : _count - 1);
        if (b.pressed(Btn::Down))   select(_sel < _count - 1 ? _sel + 1 : 0);
        if (b.pressed(Btn::Action)) return Transition::push(_items[_sel].target);
    }
    if (b.longPressed(Btn::Action)) return Transition::pop();

    return Transition::none();
}

void IconMenu::draw(GFXcanvas16 &c, U8G2_FOR_ADAFRUIT_GFX &g) {
    if (_count <= 0) return;
    const Item &it = _items[_sel];

    // Zone centrale : entre l'en-tête et le pied de page
    const int areaTop = cfg::HEADER_H;
    const int areaH   = cfg::SCREEN_H - cfg::HEADER_H - cfg::FOOTER_H;

    // Le bloc « icône + texte » est centré verticalement
    const int blockH = it.h + kGap + kTextH;
    const int top    = areaTop + (areaH - blockH) / 2;

    drawIcon(c, (cfg::SCREEN_W - it.w) / 2, top, it.icon, it.w, it.h);

    char fit[32];
    setFont(g, u8g2_font_helvB12_tf, ST77XX_BLACK);
    textutils::fitText(g, it.label, fit, sizeof(fit), cfg::SCREEN_W - 2 * kMarginX);
    const int textW = g.getUTF8Width(fit);
    g.drawUTF8((cfg::SCREEN_W - textW) / 2, top + it.h + kGap + 12, fit);

    // Indicateur de position : un point par entrée, celui de l'entrée active est plus gros
    if (_count > 1) {
        const int step  = min(kDotStep, (areaH - 12) / (_count - 1));   // se resserre s'il y a beaucoup d'entrées
        const int total = (_count - 1) * step;
        const int x     = 10;                                           // distance au bord gauche
        const int y0    = areaTop + (areaH - total) / 2;                // colonne centrée verticalement
        for (int i = 0; i < _count; i++) {
            const bool on = (i == _sel);
            c.fillCircle(x, y0 + i * step, on ? 3 : 2, on ? ST77XX_ORANGE : kDotOff);
        }
    }
}

FooterHints IconMenu::footerHints() const {
    FooterHints h;
    h.up    = "Préc.";
    h.down  = "Suiv.";
    h.press = "Ouvrir";
    h.hold  = "Retour";
    return h;
}