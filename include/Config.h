#pragma once
#include <Arduino.h>

// ---------------------------------------------------------------
// Configuration matérielle : Adafruit Feather ESP32-S3 Reverse TFT
// ---------------------------------------------------------------

// Broches de l'écran (normalement déjà définies par la carte ;
// ces valeurs ne servent que de secours)
#ifndef TFT_CS
#define TFT_CS 42
#endif
#ifndef TFT_DC
#define TFT_DC 40
#endif
#ifndef TFT_RST
#define TFT_RST 41
#endif
#ifndef TFT_BACKLITE
#define TFT_BACKLITE 45
#endif
#ifndef TFT_I2C_POWER
#define TFT_I2C_POWER 21
#endif

namespace cfg {

constexpr int SCREEN_W = 240;
constexpr int SCREEN_H = 135;

// Boutons (numéros de GPIO). -1 = bouton absent.
constexpr int PIN_BTN_ACTION = 1;   // D1 : actif à l'état HAUT
constexpr int PIN_BTN_UP      = 0;   // D0 : actif à l'état BAS (aussi bouton BOOT)
constexpr int PIN_BTN_DOWN    = 2;   // D2 : actif à l'état HAUT

constexpr uint32_t DEBOUNCE_MS   = 25;
constexpr uint32_t LONG_PRESS_MS = 600;

}  // namespace cfg
