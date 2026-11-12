#pragma once
#include <Arduino.h>
#include "Notification.h"      // struct Notif { const char *app, *title, *text; }

// Mémoire des notifications reçues : taille fixe, aucune allocation dynamique.
// Les écrans lisent ce magasin ; BleLink y dépose les paquets reçus.
//
// Format du paquet (écriture BLE, entiers en little-endian) :
//   [0]      op      0 = nouvelle notification, 1 = supprimée sur le téléphone
//   [1..4]   id      uint32, identifiant choisi par l'appli
//   -- seulement pour op = 0 --
//   [5..8]   time    uint32, epoch UTC
//   [9]      vibe    0 = court, 1 = double, 2 = long, 255 = aucune
//   [10]     appLen, puis app    (UTF-8)
//            titleLen, puis title
//            textLen, puis text
class NotificationStore {
    public:
        static constexpr int kCapacity = 20;

        static constexpr uint8_t kVibeShort  = 0;
        static constexpr uint8_t kVibeDouble = 1;
        static constexpr uint8_t kVibeLong   = 2;
        static constexpr uint8_t kVibeNone   = 255;

        enum class Result : uint8_t { Ignored, Added, Removed };

        NotificationStore();

        // Décode un paquet reçu. Pour Result::Added, vibe reçoit le motif demandé.
        Result handlePacket(const uint8_t *data, size_t len, uint8_t &vibe);

        int      count() const { return _count; }
        const Notif &at(int i) const;       // 0 = la plus récente
        uint32_t timeAt(int i) const;       // epoch UTC (0 si i invalide)

        // Augmente à chaque changement : permet à l'interface de savoir s'il faut redessiner.
        uint32_t version() const { return _version; }

    private:
        static constexpr size_t kAppMax   = 16;
        static constexpr size_t kTitleMax = 48;
        static constexpr size_t kTextMax  = 160;

        struct Slot {
            bool     used = false;
            uint32_t id   = 0;
            uint32_t time = 0;
            char     app[kAppMax + 1];
            char     title[kTitleMax + 1];
            char     text[kTextMax + 1];
            Notif    view;              // pointe vers les tableaux ci-dessus : renvoyé par at()
        };

        int  findById(uint32_t id) const;            // position dans _order, ou -1
        int  freeSlot() const;
        void moveToFront(int orderPos);
        void removeAt(int orderPos);

        Slot    _slots[kCapacity];
        uint8_t _order[kCapacity];                   // indices de slots, du plus récent au plus ancien
        int     _count   = 0;
        uint32_t _version = 0;
};