#include "Ui.h"
#include "Config.h"
#include "TextUtils.h"

namespace {

struct Notif {
    const char *app;
    const char *title;
    const char *text;
};

const Notif kDemo[] = {
    {"Messages", "Maman",     "Tu rentres ce soir ?"},
    {"Mail",     "Polytech",  "Rappel : rendu du projet vendredi"},
    {"Agenda",   "TP RCA",    "Demain 8h, salle 204"},
    {"Chat",     "Groupe",    "On se retrouve a 18h ?"},
    {"Livraison", "Colis",    "Votre colis arrive aujourd'hui"},
    {"Systeme",  "Battérie",  "Batterie du telephone faible"},
};
constexpr int kDemoCount = sizeof(kDemo) / sizeof(kDemo[0]);
constexpr int kVisibleRows = 4;
constexpr int kHeaderH = 20;
constexpr int kFooterH = 15;
constexpr int kRowH = 25;
constexpr uint16_t kGrey = 0x7BEF;

}  // namespace

Ui::Ui()
    : _tft(TFT_CS, TFT_DC, TFT_RST),
      _canvas(cfg::SCREEN_W, cfg::SCREEN_H) {}

void Ui::begin() {
    // Rétroéclairage et alimentation de l'écran
    pinMode(TFT_BACKLITE, OUTPUT);
    digitalWrite(TFT_BACKLITE, HIGH);
    pinMode(TFT_I2C_POWER, OUTPUT);
    digitalWrite(TFT_I2C_POWER, HIGH);
    delay(10);

    _tft.init(135, 240);
    _tft.setRotation(3);  // paysage 240x135
    _tft.fillScreen(ST77XX_BLACK);

    _u8g2.begin(_canvas);

    _canvas.setTextWrap(false);
    _dirty = true;
}

void Ui::setEvent(const char *name) {
    snprintf(_event, sizeof(_event), "%s", name);
    _dirty = true;
}

void Ui::handleInput(Buttons &b) {
    if (b.pressed(Btn::Up)) {
        if (_sel > 0) _sel--;
        else _sel = kDemoCount - 1;
        setEvent("Haut");
    }
    if (b.pressed(Btn::Down)) {
        if (_sel < kDemoCount - 1) _sel++;
        else _sel = 0;
        setEvent("Bas");
    }
    if (b.pressed(Btn::Action)) {
        _screen = (_screen == Screen::List) ? Screen::Detail : Screen::List;
        setEvent("Action 1");
    }
    if (b.longPressed(Btn::Action)) {
        setEvent("Action 1 (long)");
    }
    if (b.pressed(Btn::Action2)) {
        _screen = Screen::List;
        setEvent("Action 2");
    }
    if (b.longPressed(Btn::Action2)) {
        setEvent("Action 2 (long)");
    }
}

void Ui::updateBattery(Battery &b) {
    int perc = b.getPercentage();
    const uint16_t* ico = b.getIcon();
    if (perc != _batteryPercentage) { _batteryPercentage = perc; _dirty = true; }
    if (ico != _batteryIcon) {_batteryIcon = ico; _dirty = true;}
}

void Ui::drawHeader() {
    // Heure provisoire = temps écoulé depuis le démarrage.
    // Elle sera remplacée par une horloge synchronisée par le téléphone.
    const uint32_t s = millis() / 1000;
    char buf[16];
    snprintf(buf, sizeof(buf), "%02lu:%02lu:%02lu",
             (unsigned long)((s / 3600) % 100),
             (unsigned long)((s / 60) % 60),
             (unsigned long)(s % 60));

    _canvas.fillRect(0, 0, cfg::SCREEN_W, kHeaderH, ST77XX_ORANGE);
    setFont(u8g2_font_helvR14_tf, ST77XX_WHITE);
    _u8g2.drawUTF8(6, 17, buf);

    drawIcon(cfg::SCREEN_W - 84, 2, _batteryIcon, 32, 16);
    char batt[8];
    if (_batteryPercentage < 0) snprintf(batt, sizeof(batt), "--%%");
    else snprintf(batt, sizeof(batt), "%d%%", _batteryPercentage);
    setFont(u8g2_font_helvR14_tf, ST77XX_WHITE);
    _u8g2.drawUTF8(cfg::SCREEN_W - 50, 17, batt);
}

void Ui::drawList() {
    int first = 0;
    if (_sel >= kVisibleRows) first = _sel - kVisibleRows + 1;

    for (int row = 0; row < kVisibleRows; row++) {
        const int i = first + row;
        if (i >= kDemoCount) break;

        const int y = kHeaderH + 2 + row * kRowH;
        const bool selected = (i == _sel);

        if (selected) {
            _canvas.fillRect(0, y, cfg::SCREEN_W, kRowH - 2, ST77XX_WHITE);
        }

        char line[40];
        snprintf(line, sizeof(line), "%s: %s", kDemo[i].app, kDemo[i].title);
        setFont(selected ? u8g2_font_helvB12_tf : u8g2_font_helvR12_tf, selected ? ST77XX_BLACK : ST77XX_WHITE);
        _u8g2.drawUTF8(6, y + 16, line);
    }
}

void Ui::drawDetail() {
    const Notif &n = kDemo[_sel];
    const int x = 6;
    const int maxW = cfg::SCREEN_W - 2 * x;
    char buf[48];

    // Application (petit, jaune)
    setFont(u8g2_font_helvR10_tf, ST77XX_YELLOW);
    textutils::fitText(_u8g2, n.app, buf, sizeof(buf), maxW);
    _u8g2.drawUTF8(x, kHeaderH + 16, buf);

    // Titre (gras, blanc)
    setFont(u8g2_font_helvB14_tf, ST77XX_WHITE);
    textutils::fitText(_u8g2, n.title, buf, sizeof(buf), maxW);
    _u8g2.drawUTF8(x, kHeaderH + 40, buf);

    // Séparateur
    _canvas.drawFastHLine(x, kHeaderH + 46, maxW, kGrey);

    // Texte, sur 3 lignes maximum
    setFont(u8g2_font_helvR10_tf, ST77XX_CYAN);
    textutils::drawWrapped(_u8g2, n.text, x, kHeaderH + 64, 14, maxW, 3);
}

void Ui::drawFooter() {
    const int y = cfg::SCREEN_H - kFooterH;
    _canvas.fillRect(0, y, cfg::SCREEN_W, kFooterH, ST77XX_ORANGE);

    char line[64];
    snprintf(line, sizeof(line), "Dernier bouton: %s", _event);
    setFont(u8g2_font_helvR10_tf, ST77XX_WHITE);
    _u8g2.drawUTF8(4, y + 13, line);
}

void Ui::drawIcon(int x, int y, const uint16_t *icon, int w, int h) {
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            const uint16_t c = icon[j * w + i];
            if (c != 0x000) _canvas.drawPixel(x + i, y + j, c);
        }
    }
}

void Ui::setFont(const uint8_t *font, uint16_t color) {
    _u8g2.setFont(font);
    _u8g2.setForegroundColor(color);
    _u8g2.setFontMode(1);
}

void Ui::draw() {
    // Redessine si un événement est arrivé ou si la seconde a changé
    const uint32_t sec = millis() / 1000;
    if (!_dirty && sec == _lastSecond) return;
    _lastSecond = sec;
    _dirty = false;

    _canvas.fillScreen(ST77XX_BLACK);
    drawHeader();
    if (_screen == Screen::List) drawList();
    else                         drawDetail();
    drawFooter();

    _tft.drawRGBBitmap(0, 0, _canvas.getBuffer(), cfg::SCREEN_W, cfg::SCREEN_H);
}
