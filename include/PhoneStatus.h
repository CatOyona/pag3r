#pragma once
#include <stdint.h>

// État du téléphone, tel que l'appli l'envoie par BLE.
// Valeurs « inconnues » : battery = -1, signal = -1.
struct PhoneStatus {
    bool    connected = false;   // faux dès que la liaison BLE tombe : on n'affiche plus rien de périmé
    int     battery   = -1;      // 0..100
    bool    charging  = false;
    int     signal    = -1;      // barres du réseau mobile, 0..4
};