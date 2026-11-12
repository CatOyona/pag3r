#pragma once
#include <Arduino.h>

// Identifiants des écrans : c'est ce qu'un écran utilise pour en désigner un autre,
// sans jamais connaître sa classe. Ajoute une ligne ici pour chaque nouvel écran
// (Count doit rester en dernier).
enum class ScreenId : uint8_t {
    Lock,
    MainMenu,
    NotifList,
    NotifDetail,
    PhoneDetail,
    Count
};

// Demande de changement d'écran, renvoyée par Screen::handleInput().
struct Transition {
    enum class Type : uint8_t {
        None,      // rien à faire
        Push,      // ouvrir target par-dessus l'écran actuel
        Pop,       // revenir à l'écran précédent
        Replace    // vider la pile et ne garder que target (ex : déverrouillage)
    };

    Type     type   = Type::None;
    ScreenId target = ScreenId::Lock;
    int      arg    = 0;   // donnée transmise à onEnter() de la cible (ex : index de notification)

    static Transition none() { return Transition(); }

    static Transition push(ScreenId t, int a = 0) {
        Transition r; r.type = Type::Push; r.target = t; r.arg = a; return r;
    }
    static Transition pop() {
        Transition r; r.type = Type::Pop; return r;
    }
    static Transition replace(ScreenId t, int a = 0) {
        Transition r; r.type = Type::Replace; r.target = t; r.arg = a; return r;
    }
};