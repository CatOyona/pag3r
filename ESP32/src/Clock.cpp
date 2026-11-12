#include "Clock.h"
#include <sys/time.h>

void Clock::setUtc(uint32_t epoch, int16_t tzMinutes) {
    struct timeval tv;
    tv.tv_sec  = static_cast<time_t>(epoch);
    tv.tv_usec = 0;
    settimeofday(&tv, nullptr);          // met à jour l'horloge système de l'ESP32

    _tzSeconds = static_cast<int32_t>(tzMinutes) * 60;
    _synced    = true;
}

bool Clock::getLocal(struct tm &out) const {
    if (!_synced) return false;

    const time_t local = time(nullptr) + _tzSeconds;
    gmtime_r(&local, &out);              // le décalage est déjà appliqué : on décompose sans fuseau
    return true;
}