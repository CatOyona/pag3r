#include "PhoneScreen.h"
#include <Adafruit_ST7789.h>   // couleurs ST77XX_*
#include "Config.h"
#include "TextUtils.h"

namespace {
    constexpr int      kMarginX = 10;
    constexpr int      kRowH    = 25;
    constexpr uint16_t kGrey    = 0xbdf7;
    constexpr uint16_t kGreen   = 0x0400;

    bool sameStatus(const PhoneStatus &a, const PhoneStatus &b) {
        return a.connected == b.connected && a.battery == b.battery && a.charging == b.charging && a.signal == b.signal;
    }
}  // namespace

PhoneScreen::PhoneScreen(const PhoneStatus &status) : _status(status) {}

void PhoneScreen::onEnter(int /*arg*/) {
    _shown = _status;
    _dirty = true;
}

Transition PhoneScreen::handleInput(Buttons &b) {
    if (!sameStatus(_shown, _status)) {   // le téléphone a envoyé du nouveau
        _shown = _status;
        _dirty = true;
    }
    if (b.pressed(Btn::Action)) return Transition::pop();
    return Transition::none();
}

void PhoneScreen::draw(GFXcanvas16 &c, U8G2_FOR_ADAFRUIT_GFX &g) {
    const int y0 = cfg::HEADER_H + 20;

    if (!_shown.connected) {
        setFont(g, u8g2_font_helvB12_tf, ST77XX_BLACK);
        const char *msg = "Non connecté";
        const int y = cfg::HEADER_H + ((cfg::SCREEN_H - cfg::HEADER_H - cfg::FOOTER_H) - (g.getFontAscent() - g.getFontDescent())) / 2 + g.getFontAscent();
        g.drawUTF8((cfg::SCREEN_W - g.getUTF8Width(msg)) / 2, y, msg);
        return;
    } else {
        if (_shown.battery < 0 && _shown.signal < 0) {
            setFont(g, u8g2_font_helvB12_tf, ST77XX_BLACK);
            const char *msg = "En attente des infos...";
            const int y = cfg::HEADER_H + ((cfg::SCREEN_H - cfg::HEADER_H - cfg::FOOTER_H) - (g.getFontAscent() - g.getFontDescent())) / 2 + g.getFontAscent();
            g.drawUTF8((cfg::SCREEN_W - g.getUTF8Width(msg)) / 2, y, msg);
            return;
        }
    }

    // Ligne 1 : batterie
    setFont(g, u8g2_font_helvR12_tf, ST77XX_BLACK);
    g.drawUTF8(kMarginX, y0 + 17, "Batterie");

    char txt[16];
    if (_shown.battery < 0) snprintf(txt, sizeof(txt), "--%%");
    else                    snprintf(txt, sizeof(txt), "%d %%", _shown.battery);
    setFont(g, u8g2_font_helvB12_tf, ST77XX_BLACK);
    g.drawUTF8(cfg::SCREEN_W - kMarginX - g.getUTF8Width(txt), y0 + 17, txt);

    // Jauge sous le texte, et mention de charge
    const int gaugeY = y0 + kRowH - 2;
    const int gaugeW = cfg::SCREEN_W - 2 * kMarginX;
    c.drawRect(kMarginX, gaugeY, gaugeW, 8, ST77XX_BLACK);
    if (_shown.battery > 0) {
        c.fillRect(kMarginX + 1, gaugeY + 1, (gaugeW - 2) * _shown.battery / 100, 6, _shown.charging ? ST77XX_ORANGE : kGreen);
    }

    // Ligne 2 : signal (4 barres)
    const int row2 = y0 + kRowH + 14;
    setFont(g, u8g2_font_helvR12_tf, ST77XX_BLACK);
    g.drawUTF8(kMarginX, row2 + 17, "Réseau");

    const int barW = 6, gap = 3;
    const int xBars = cfg::SCREEN_W - kMarginX - 4 * barW - 3 * gap;
    for (int i = 0; i < 4; i++) {
        const int h = 4 + i * 4;
        c.fillRect(xBars + i * (barW + gap), row2 + 20 - h, barW, h, i < _shown.signal ? ST77XX_BLACK : kGrey);
    }
}

FooterHints PhoneScreen::footerHints() const {
    FooterHints h;
    h.press = "Retour";
    return h;
}