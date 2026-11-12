#pragma once
#include <Arduino.h>
#include <time.h>

// Horloge du pager. Elle ne connaît pas l'heure toute seule : le téléphone la lui envoie
// (via BleLink) à chaque connexion, ce qui corrige la dérive du quartz de l'ESP32.
// Entre deux synchronisations, c'est l'horloge système de l'ESP32 qui compte le temps.
class Clock {
public:
    // epoch      : secondes depuis le 01/01/1970, en UTC
    // tzMinutes  : décalage du fuseau par rapport à UTC, en minutes (ex : +60 pour Paris en hiver)
    void setUtc(uint32_t epoch, int16_t tzMinutes);

    bool isSynced() const { return _synced; }

    // Heure locale décomposée (tm_hour, tm_min, tm_sec, tm_mday...).
    // Renvoie false tant que l'horloge n'a jamais été synchronisée.
    bool getLocal(struct tm &out) const;

private:
    bool    _synced    = false;
    int32_t _tzSeconds = 0;
};