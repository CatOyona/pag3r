#include "Buttons.h"
#include "Config.h"

static inline size_t idx(Btn b) { return static_cast<size_t>(b); }

void Buttons::begin() {
    _s[idx(Btn::Action)].pin = cfg::PIN_BTN_ACTION;
    _s[idx(Btn::Action)].activeLow = false;

    _s[idx(Btn::Up)].pin = cfg::PIN_BTN_UP;
    _s[idx(Btn::Up)].activeLow = true;

    _s[idx(Btn::Down)].pin = cfg::PIN_BTN_DOWN;
    _s[idx(Btn::Down)].activeLow = false;

    for (auto &s : _s) {
        if (s.pin < 0) continue;
        pinMode(s.pin, s.activeLow ? INPUT_PULLUP : INPUT_PULLDOWN);
    }
}

void Buttons::update() {
    const uint32_t now = millis();

    for (auto &s : _s) {
        if (s.pin < 0) continue;

        const bool raw = (digitalRead(s.pin) == (s.activeLow ? LOW : HIGH));

        if (raw != s.lastRaw) {
            s.lastRaw = raw;
            s.lastChange = now;
        }

        // L'état n'est accepté que s'il est stable depuis DEBOUNCE_MS
        if (raw != s.stable && (now - s.lastChange) >= cfg::DEBOUNCE_MS) {
            s.stable = raw;
            if (raw) {
                s.downSince = now;
                s.longSent = false;
            } else if (!s.longSent) {
                s.pressedFlag = true;  // relâché avant le seuil : appui court
            }
        }

        // Appui long : déclenché pendant le maintien
        if (s.stable && !s.longSent && (now - s.downSince) >= cfg::LONG_PRESS_MS) {
            s.longSent = true;
            s.longFlag = true;
        }
    }
}

bool Buttons::pressed(Btn b) {
    State &s = _s[idx(b)];
    const bool r = s.pressedFlag;
    s.pressedFlag = false;
    return r;
}

bool Buttons::longPressed(Btn b) {
    State &s = _s[idx(b)];
    const bool r = s.longFlag;
    s.longFlag = false;
    return r;
}

bool Buttons::isDown(Btn b) const {
    return _s[idx(b)].stable;
}
