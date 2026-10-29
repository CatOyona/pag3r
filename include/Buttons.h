#pragma once
#include <Arduino.h>

enum class Btn : uint8_t { Up, Down, Action, Action2, Count };

// Lecture des boutons avec anti-rebond.
//  - pressed()     : vrai une fois, au relâchement d'un appui COURT
//  - longPressed() : vrai une fois, dès que le maintien dépasse LONG_PRESS_MS
// Les deux méthodes « consomment » l'événement.
class Buttons {
public:
    void begin();
    void update();  // à appeler à chaque tour de loop()

    bool pressed(Btn b);
    bool longPressed(Btn b);
    bool isDown(Btn b) const;

private:
    struct State {
        int8_t   pin = -1;
        bool     activeLow = true;
        bool     stable = false;   // état debouncé (true = enfoncé)
        bool     lastRaw = false;
        uint32_t lastChange = 0;
        uint32_t downSince = 0;
        bool     pressedFlag = false;
        bool     longFlag = false;
        bool     longSent = false;
    };

    State _s[static_cast<size_t>(Btn::Count)];
};
