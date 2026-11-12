#pragma once
#include <Arduino.h>
#include <NimBLEDevice.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "Clock.h"
#include "PhoneStatus.h"

class NotificationStore;
class Haptics;

// Message reçu par BLE, mis en file par les callbacks (qui tournent dans la tâche BLE)
// puis traité dans loop() par BleLink::update().
struct BleEvent {
    enum class Type : uint8_t { Time, PhoneStatus, Notification, Connected, Disconnected };   // on ajoutera : Config...
    Type    type;
    uint8_t len;
    uint8_t data[250];                    // une notification tient dans un paquet (MTU 247 -> 244 octets utiles)
};

// Le pager est le « périphérique » BLE : il s'annonce, le téléphone se connecte.
//
//   Service pager      (UUID personnalisé)
//     ├ Heure          écriture : uint32 epoch UTC + int16 décalage en minutes (6 octets, little-endian)
//     └ État téléphone écriture : 3 octets = batterie 0..100 (255 = inconnue),
//                                 drapeaux (bit0 = en charge), signal 0..4 (255 = inconnu)
//     └ Notification  écriture : un paquet par notification (format : voir NotificationStore.h)
//   Service batterie   (standard 0x180F)
//     └ Niveau         lecture + notification : 1 octet, 0..100 (255 = inconnu)
class BleLink {
public:
    void begin();                    // dans setup()
    void update(Clock &clock, PhoneStatus &phone, NotificationStore &notifs, Haptics &haptics);   // dans loop() : traite les messages reçus
    void setBattery(int percent);    // batterie du pager (0..100, -1 = inconnue)

    bool isConnected() const { return _connected; }

private:
    // --- callbacks : exécutés dans la tâche BLE, jamais d'accès direct à l'interface ici ---
    class ServerCb : public NimBLEServerCallbacks {
    public:
        explicit ServerCb(BleLink &owner) : _o(owner) {}
        void onConnect(NimBLEServer *server, NimBLEConnInfo &connInfo) override;
        void onDisconnect(NimBLEServer *server, NimBLEConnInfo &connInfo, int reason) override;
    private:
        BleLink &_o;
    };

    // Une même classe sert pour toutes les caractéristiques en écriture : on lui dit quel type
    // de message elle produit.
    class WriteCb : public NimBLECharacteristicCallbacks {
    public:
        WriteCb(BleLink &owner, BleEvent::Type type) : _o(owner), _type(type) {}
        void onWrite(NimBLECharacteristic *chr, NimBLEConnInfo &connInfo) override;
    private:
        BleLink        &_o;
        BleEvent::Type  _type;
    };

    void push(BleEvent::Type type, const uint8_t *data, size_t len);

    // Les callbacks gardent un pointeur vers ces objets : BleLink doit donc rester un objet
    // global (comme dans main.cpp) et ne jamais être copié ni déplacé.
    ServerCb _serverCb{*this};
    WriteCb  _timeCb{*this, BleEvent::Type::Time};
    WriteCb  _statusCb{*this, BleEvent::Type::PhoneStatus};
    WriteCb  _notifCb{*this, BleEvent::Type::Notification};

    QueueHandle_t         _queue = nullptr;
    NimBLECharacteristic *_batteryChar = nullptr;
    int                   _lastBattery = -2;
    volatile bool         _connected = false;
};