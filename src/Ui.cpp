#include "Ui.h"
#include "Config.h"

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
    {"Systeme",  "Batterie",  "Batterie du telephone faible"},
};
constexpr int kDemoCount = sizeof(kDemo) / sizeof(kDemo[0]);
constexpr int kVisibleRows = 4;
constexpr int kHeaderH = 20;
constexpr int kFooterH = 12;
constexpr int kRowH = 25;

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
        setEvent("Haut");
    }
    if (b.pressed(Btn::Down)) {
        if (_sel < kDemoCount - 1) _sel++;
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

void Ui::drawHeader() {
    // Heure provisoire = temps écoulé depuis le démarrage.
    // Elle sera remplacée par une horloge synchronisée par le téléphone.
    const uint32_t s = millis() / 1000;
    char buf[16];
    snprintf(buf, sizeof(buf), "%02lu:%02lu:%02lu",
             (unsigned long)((s / 3600) % 100),
             (unsigned long)((s / 60) % 60),
             (unsigned long)(s % 60));

    _canvas.fillRect(0, 0, cfg::SCREEN_W, kHeaderH, ST77XX_BLUE);
    _canvas.setTextColor(ST77XX_WHITE);
    _canvas.setTextSize(2);
    _canvas.setCursor(6, 3);
    _canvas.print(buf);

    _canvas.setCursor(cfg::SCREEN_W - 70, 6);
    _canvas.print("Bat --%");
}

void Ui::drawList() {
    int first = 0;
    if (_sel >= kVisibleRows) first = _sel - kVisibleRows + 1;

    _canvas.setTextSize(2);
    for (int row = 0; row < kVisibleRows; row++) {
        const int i = first + row;
        if (i >= kDemoCount) break;

        const int y = kHeaderH + 2 + row * kRowH;
        const bool selected = (i == _sel);

        if (selected) {
            _canvas.fillRect(0, y, cfg::SCREEN_W, kRowH - 2, ST77XX_WHITE);
            _canvas.setTextColor(ST77XX_BLACK);
        } else {
            _canvas.setTextColor(ST77XX_WHITE);
        }

        char line[40];
        snprintf(line, sizeof(line), "%s: %s", kDemo[i].app, kDemo[i].title);
        _canvas.setCursor(6, y + 4);
        _canvas.print(line);
    }
}

void Ui::drawDetail() {
    const Notif &n = kDemo[_sel];

    _canvas.setTextWrap(true);
    _canvas.setTextColor(ST77XX_YELLOW);
    _canvas.setTextSize(2);
    _canvas.setCursor(6, kHeaderH + 6);
    _canvas.print(n.app);

    _canvas.setTextColor(ST77XX_WHITE);
    _canvas.setCursor(6, kHeaderH + 28);
    _canvas.print(n.title);

    _canvas.setTextSize(1);
    _canvas.setTextColor(ST77XX_CYAN);
    _canvas.setCursor(6, kHeaderH + 54);
    _canvas.print(n.text);
    _canvas.setTextWrap(false);
}

void Ui::drawFooter() {
    const int y = cfg::SCREEN_H - kFooterH;
    _canvas.fillRect(0, y, cfg::SCREEN_W, kFooterH, ST77XX_BLACK);
    _canvas.setTextSize(1);
    _canvas.setTextColor(ST77XX_GREEN);
    _canvas.setCursor(4, y + 2);
    _canvas.print("Dernier bouton: ");
    _canvas.print(_event);
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
