#include "Ui.h"
#include "Config.h"
#include "TextUtils.h"
#include "Icons.h"
#include "Notification.h"

namespace {
const Notif kDemo[] = {
    {"Messages", "Maman",     "Tu rentres ce soir ?"},
    {"Mail",     "Polytech",  "Rappel : rendu du projet vendredi"},
    {"Agenda",   "TP RCA",    "Demain 8h, salle 204"},
    {"Chat",     "Groupe",    "On se retrouve a 18h ?"},
    {"Livraison", "Colis",    "Votre colis arrive aujourd'hui"},
    {"Systeme",  "Battérie",  "Batterie du telephone faible"},
};
constexpr int kDemoCount = sizeof(kDemo) / sizeof(kDemo[0]);
const IconMenu::Item kMenuItems[] = {
    {"Notifications", ICON_BELL, 64, 64, ScreenId::NotifList},
    {"Téléphone", ICON_PHONE, 64, 64, ScreenId::PhoneDetail},
    {"Réglages", ICON_SETTINGS, 64, 64, ScreenId::NotifDetail}
};
constexpr int kMenuCount = sizeof(kMenuItems) / sizeof(kMenuItems[0]);
constexpr int kHeaderH = cfg::HEADER_H;
constexpr int kFooterH = cfg::FOOTER_H;
constexpr int kButtonIconW = 16;
constexpr int kButtonIconH = 15;
constexpr int kButtonGap   = 2;

void notifLabel(int i, char *out, size_t n) {
    snprintf(out, n, "%s: %s", kDemo[i].app, kDemo[i].title);
}

const Notif& notifAt(int i) {
    return kDemo[i];
}

}  // namespace

Ui::Ui(const PhoneStatus &status)
    : _tft(TFT_CS, TFT_DC, TFT_RST),
      _canvas(cfg::SCREEN_W, cfg::SCREEN_H),
      _notifList(notifLabel, kDemoCount, ScreenId::NotifDetail),
      _notifDetail(notifAt),
      _mainMenu(kMenuItems, kMenuCount),
      _phoneDetail(status) {}

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

    // Enregistrement des écrans (une seule fois), puis écran de départ
    _manager.registerScreen(ScreenId::NotifList, &_notifList);
    _manager.registerScreen(ScreenId::NotifDetail, &_notifDetail);
    _manager.registerScreen(ScreenId::MainMenu, &_mainMenu);
    _manager.registerScreen(ScreenId::PhoneDetail, &_phoneDetail);
    _manager.begin(ScreenId::MainMenu);

    _dirty = true;
}

void Ui::handleInput(Buttons &b) {
    // Les boutons sont consommés par l'écran actif : c'est lui qui décide quoi en faire.
    _manager.handleInput(b);
    b.clearEvents();
}

void Ui::updateBattery(Battery &b) {
    int perc = b.getPercentage();
    const uint16_t* ico = b.getIcon();
    if (perc != _batteryPercentage) { _batteryPercentage = perc; _dirty = true; }
    if (ico != _batteryIcon) {_batteryIcon = ico; _dirty = true;}
}

void Ui::drawHeader() {
    _canvas.fillRect(0, 0, cfg::SCREEN_W, kHeaderH, ST77XX_ORANGE);
    setFont(u8g2_font_helvR14_tf, ST77XX_WHITE);
    _u8g2.drawUTF8(6, 17, _timeText);

    drawIcon(cfg::SCREEN_W - 84, 2, _batteryIcon, 32, 16);
    char batt[8];
    if (_batteryPercentage < 0) snprintf(batt, sizeof(batt), "--%%");
    else snprintf(batt, sizeof(batt), "%d%%", _batteryPercentage);
    setFont(u8g2_font_helvR14_tf, ST77XX_WHITE);
    _u8g2.drawUTF8(cfg::SCREEN_W - 50, 17, batt);
}

void Ui::drawFooter() {
    const int y = cfg::SCREEN_H - kFooterH;
    _canvas.fillRect(0, y, cfg::SCREEN_W, kFooterH, ST77XX_ORANGE);

    const FooterHints h = _manager.footerHints();
    const char *label[4]    = { h.up, h.down, h.press, h.hold };
    const uint16_t *icon[4] = { ICON_BUTTON_UP, ICON_BUTTON_DOWN,
                                ICON_BUTTON_ACTION, ICON_BUTTON_ACTION_HOLD };

    setFont(u8g2_font_helvR10_tf, ST77XX_WHITE);   // avant toute mesure

    // 1) Largeur réelle du texte de chaque aide
    int textW[4] = {0, 0, 0, 0};
    int n = 0, sumText = 0;
    for (int i = 0; i < 4; i++) {
        if (!label[i]) continue;
        textW[i] = _u8g2.getUTF8Width(label[i]);
        sumText += textW[i];
        n++;
    }
    if (n == 0) return;

    // 2) Si l'ensemble ne tient pas, on plafonne la largeur de chaque texte
    const int fixedPerCell = kButtonIconW + kButtonGap;        // icône + espace avant le texte
    const int minMargin    = 4;
    const int availText    = cfg::SCREEN_W - n * fixedPerCell - (n + 1) * minMargin;
    const int cap          = (sumText > availText) ? availText / n : 1000;

    char fit[4][24];
    int used = 0;
    for (int i = 0; i < 4; i++) {
        if (!label[i]) continue;
        textutils::fitText(_u8g2, label[i], fit[i], sizeof(fit[i]), cap);
        textW[i] = _u8g2.getUTF8Width(fit[i]);
        used += fixedPerCell + textW[i];
    }

    // 3) L'espace restant est réparti en marges égales (aux bords et entre les aides)
    const int margin = (cfg::SCREEN_W - used) / (n + 1);
    int x = margin;
    for (int i = 0; i < 4; i++) {
        if (!label[i]) continue;
        drawIcon(x, y + (kFooterH - kButtonIconH) / 2, icon[i], kButtonIconW, kButtonIconH);
        _u8g2.drawUTF8(x + fixedPerCell, y + 13, fit[i]);
        x += fixedPerCell + textW[i] + margin;
    }
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
    // Redessine si Ui a changé (batterie...), si l'écran actif le demande,
    // ou si la seconde a changé (horloge de l'en-tête)
    const uint32_t sec = millis() / 1000;
    if (!_dirty && !_manager.isDirty() && sec == _lastSecond) return;
    _lastSecond = sec;
    _dirty = false;

    _canvas.fillScreen(0xef5d);
    drawHeader();
    _manager.draw(_canvas, _u8g2);   // zone centrale
    drawFooter();

    _tft.drawRGBBitmap(0, 0, _canvas.getBuffer(), cfg::SCREEN_W, cfg::SCREEN_H);
}

void Ui::updateClock(const Clock &c) {
    struct tm t;
    char buf[9];
    if (c.getLocal(t)) {
        strftime(buf, sizeof(buf), "%H:%M:%S", &t);
    } else {
        strcpy(buf, "--:--:--");
    }
    if (!strcmp(buf, _timeText) == 0) {
        _dirty = true;
        strcpy(_timeText, buf);
    }
}