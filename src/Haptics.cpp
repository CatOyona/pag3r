#include "Haptics.h"
#include "Config.h"

void Haptics::begin() {
    pinMode(cfg::PIN_VIB, OUTPUT);
    apply(false);
}

void Haptics::apply(bool on) {
    digitalWrite(cfg::PIN_VIB, on ? HIGH : LOW);
}

void Haptics::start(const Segment *pattern, int count) {
    if (count > kMaxSegments) count = kMaxSegments;
    for (int i = 0; i < count; i++) _pattern[i] = pattern[i];

    _count    = count;
    _index    = 0;
    _segStart = millis();
    _active   = (count > 0);
    if (_active) apply(_pattern[0].on);
}

void Haptics::update() {
    if (!_active) return;

    // Soustraction non signée : reste correcte même quand millis() repasse par zéro
    if (millis() - _segStart < _pattern[_index].ms) return;

    _index++;
    if (_index >= _count) {       // motif terminé
        stop();
        return;
    }
    _segStart = millis();
    apply(_pattern[_index].on);
}

void Haptics::stop() {
    _active = false;
    apply(false);
}

void Haptics::shortVibration() {
    const Segment p[] = { {true, 200} };
    start(p, 1);
}

void Haptics::doubleVibration() {
    const Segment p[] = { {true, 150}, {false, 80}, {true, 150} };
    start(p, 3);
}

void Haptics::longVibration() {
    const Segment p[] = { {true, 800} };
    start(p, 1);
}