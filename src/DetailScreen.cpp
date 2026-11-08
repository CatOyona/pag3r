#include "DetailScreen.h"
#include <Adafruit_ST7789.h> 
#include "Config.h"
#include "TextUtils.h"

namespace {
    constexpr int      kMarginX  = 6;
    constexpr uint16_t kGrey     = 0x7BEF;   // séparateur
    constexpr uint16_t kDarkGrey = 0x4228;   // nom de l'application
}  // namespace

DetailScreen::DetailScreen(NotifFn get) : _get(get) {}

void DetailScreen::onEnter(int arg) {
    _index = arg;      // index de la notification, envoyé par la liste
    _dirty = true;
}

Transition DetailScreen::handleInput(Buttons &b) {
    if (b.pressed(Btn::Action)) return Transition::pop();
    return Transition::none();
}

void DetailScreen::draw(GFXcanvas16 &c, U8G2_FOR_ADAFRUIT_GFX &g) {
    const Notif &n = _get(_index);
    const int maxW = cfg::SCREEN_W - 2 * kMarginX;
    char buf[48];

    // Application (petit, gris foncé)
    setFont(g, u8g2_font_helvR10_tf, kDarkGrey);
    textutils::fitText(g, n.app, buf, sizeof(buf), maxW);
    g.drawUTF8(kMarginX, cfg::HEADER_H + 16, buf);

    // Titre (gras, noir)
    setFont(g, u8g2_font_helvB14_tf, ST77XX_BLACK);
    textutils::fitText(g, n.title, buf, sizeof(buf), maxW);
    g.drawUTF8(kMarginX, cfg::HEADER_H + 40, buf);

    // Séparateur
    c.drawFastHLine(kMarginX, cfg::HEADER_H + 46, maxW, kGrey);

    // Texte, sur 3 lignes maximum (bleu)
    setFont(g, u8g2_font_helvR10_tf, ST77XX_BLUE);
    textutils::drawWrapped(g, n.text, kMarginX, cfg::HEADER_H + 64, 14, maxW, 3);
}

FooterHints DetailScreen::footerHints() const {
    FooterHints h;
    h.press  = "Retour";
    return h;
}