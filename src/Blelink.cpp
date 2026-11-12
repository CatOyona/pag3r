#include "BleLink.h"

namespace {
    // UUID du service pager. Les caractéristiques ne changent que les 4 premiers chiffres.
    constexpr const char *kServiceUuid = "4111f001-810e-4953-9fb2-393ca444e99c";
    constexpr const char *kTimeUuid    = "4111f002-810e-4953-9fb2-393ca444e99c";
    constexpr const char *kStatusUuid  = "4111f003-810e-4953-9fb2-393ca444e99c";

    constexpr int kQueueLen = 8;

    // Taille attendue du paquet « heure » : uint32 + int16
    constexpr size_t kTimePacketLen = 6;

    // Paquet « état du téléphone » : batterie, drapeaux, signal
    constexpr size_t  kStatusPacketLen = 3;
    constexpr uint8_t kUnknown         = 0xFF;

    // Une heure antérieure à 2020 est forcément une erreur d'envoi : on l'ignore.
    constexpr uint32_t kMinPlausibleEpoch = 1577836800UL;
}  // namespace

// ---------------------------------------------------------------- callbacks (tâche BLE)

void BleLink::ServerCb::onConnect(NimBLEServer *, NimBLEConnInfo &) {
    _o._connected = true;
    _o.push(BleEvent::Type::Connected, nullptr, 0);
}

void BleLink::ServerCb::onDisconnect(NimBLEServer *, NimBLEConnInfo &, int) {
    _o._connected = false;           // l'annonce redémarre toute seule (advertiseOnDisconnect)
    _o.push(BleEvent::Type::Disconnected, nullptr, 0);   // loop() invalidera l'état du téléphone
}

void BleLink::WriteCb::onWrite(NimBLECharacteristic *chr, NimBLEConnInfo &) {
    const NimBLEAttValue value = chr->getValue();    // copie des octets reçus
    _o.push(_type, value.data(), value.length());
}

// Dépose un message dans la file ; loop() le lira. On ne bloque jamais (timeout 0).
void BleLink::push(BleEvent::Type type, const uint8_t *data, size_t len) {
    if (!_queue) return;

    BleEvent e;
    e.type = type;
    e.len  = static_cast<uint8_t>(len < sizeof(e.data) ? len : sizeof(e.data));
    if (e.len > 0) memcpy(e.data, data, e.len);
    xQueueSend(_queue, &e, 0);
}

// ---------------------------------------------------------------- initialisation

void BleLink::begin() {
    _queue = xQueueCreate(kQueueLen, sizeof(BleEvent));

    NimBLEDevice::init("Pager");

    // Sécurité (appariement / chiffrement) : à ajouter plus tard, voir
    // NimBLEDevice::setSecurityAuth() et les propriétés WRITE_ENC / READ_ENC.

    NimBLEServer *server = NimBLEDevice::createServer();
    server->setCallbacks(&_serverCb, false);   // false : la bibliothèque ne supprime pas notre objet
    server->advertiseOnDisconnect(true);       // se remet à s'annoncer après une déconnexion

    // Service pager : caractéristique « heure », en écriture
    NimBLEService        *pager   = server->createService(kServiceUuid);
    NimBLECharacteristic *timeChr = pager->createCharacteristic(kTimeUuid, NIMBLE_PROPERTY::WRITE);
    timeChr->setCallbacks(&_timeCb);

    // Caractéristique « état du téléphone », en écriture
    NimBLECharacteristic *statusChr = pager->createCharacteristic(kStatusUuid, NIMBLE_PROPERTY::WRITE);
    statusChr->setCallbacks(&_statusCb);

    // Service batterie standard (0x180F), niveau (0x2A19) : lisible et notifiable
    NimBLEService *battery = server->createService("180F");
    _batteryChar = battery->createCharacteristic("2A19", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    const uint8_t unknown = 0xFF;
    _batteryChar->setValue(&unknown, 1);

    NimBLEAdvertising *adv = NimBLEDevice::getAdvertising();
    adv->setName("Pager");
    adv->addServiceUUID(pager->getUUID());
    adv->enableScanResponse(true);
    adv->start();
}

// ---------------------------------------------------------------- loop()

void BleLink::update(Clock &clock, PhoneStatus &phone) {
    if (!_queue) return;

    BleEvent e;
    while (xQueueReceive(_queue, &e, 0) == pdTRUE) {
        switch (e.type) {
            case BleEvent::Type::Time: {
                if (e.len < kTimePacketLen) break;           // paquet incomplet
                uint32_t epoch;
                int16_t  tzMinutes;
                memcpy(&epoch,     &e.data[0], sizeof(epoch));      // little-endian, comme l'ESP32
                memcpy(&tzMinutes, &e.data[4], sizeof(tzMinutes));
                if (epoch < kMinPlausibleEpoch) break;
                clock.setUtc(epoch, tzMinutes);
                break;
            }
            case BleEvent::Type::PhoneStatus: {
                if (e.len < kStatusPacketLen) break;
                phone.connected = true;
                phone.battery   = (e.data[0] == kUnknown || e.data[0] > 100) ? -1 : e.data[0];
                phone.charging  = (e.data[1] & 0x01) != 0;
                phone.signal    = (e.data[2] == kUnknown || e.data[2] > 4) ? -1 : e.data[2];
                break;
            }
            case BleEvent::Type::Disconnected:
                phone = PhoneStatus();       // tout repasse à « inconnu »
                break;
            case BleEvent::Type::Connected:
                phone = PhoneStatus();
                phone.connected = true;
                break;
        }
    }
}

void BleLink::setBattery(int percent) {
    if (!_batteryChar || percent == _lastBattery) return;    // rien à envoyer si ça n'a pas changé
    _lastBattery = percent;

    const uint8_t level = (percent < 0) ? 0xFF : static_cast<uint8_t>(percent > 100 ? 100 : percent);
    _batteryChar->setValue(&level, 1);
    if (_connected) _batteryChar->notify();                  // prévient le téléphone s'il s'est abonné
}