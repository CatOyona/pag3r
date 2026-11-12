#include "NotificationStore.h"
#include <string.h>

namespace {
    // Lit une chaîne « longueur + octets » à partir de pos. Tronque à outSize-1 octets,
    // sans couper un caractère UTF-8 en deux. Renvoie false si le paquet est trop court.
    bool readString(const uint8_t *d, size_t len, size_t &pos, char *out, size_t outSize) {
        if (pos >= len) return false;
        const size_t n = d[pos++];
        if (pos + n > len) return false;

        size_t copy = (n < outSize - 1) ? n : outSize - 1;
        // si on coupe, le premier octet supprimé ne doit pas être un « octet de suite » UTF-8
        while (copy > 0 && copy < n && (d[pos + copy] & 0xC0) == 0x80) copy--;

        memcpy(out, d + pos, copy);
        out[copy] = '\0';
        pos += n;
        return true;
    }

    uint32_t readU32(const uint8_t *p) {
        uint32_t v;
        memcpy(&v, p, sizeof(v));       // little-endian, comme l'ESP32
        return v;
    }

    const Notif kEmpty = {"", "", ""};
}  // namespace

NotificationStore::NotificationStore() {
    for (int i = 0; i < kCapacity; i++) {
        _slots[i].app[0] = _slots[i].title[0] = _slots[i].text[0] = '\0';
        _slots[i].view.app   = _slots[i].app;
        _slots[i].view.title = _slots[i].title;
        _slots[i].view.text  = _slots[i].text;
        _order[i] = 0;
    }
}

const Notif &NotificationStore::at(int i) const {
    if (i < 0 || i >= _count) return kEmpty;
    return _slots[_order[i]].view;
}

uint32_t NotificationStore::timeAt(int i) const {
    if (i < 0 || i >= _count) return 0;
    return _slots[_order[i]].time;
}

int NotificationStore::findById(uint32_t id) const {
    for (int i = 0; i < _count; i++)
        if (_slots[_order[i]].id == id) return i;
    return -1;
}

int NotificationStore::freeSlot() const {
    for (int i = 0; i < kCapacity; i++)
        if (!_slots[i].used) return i;
    return -1;
}

void NotificationStore::moveToFront(int pos) {
    const uint8_t slot = _order[pos];
    for (int i = pos; i > 0; i--) _order[i] = _order[i - 1];
    _order[0] = slot;
}

void NotificationStore::removeAt(int pos) {
    _slots[_order[pos]].used = false;
    for (int i = pos; i < _count - 1; i++) _order[i] = _order[i + 1];
    _count--;
}

NotificationStore::Result NotificationStore::handlePacket(const uint8_t *d, size_t len, uint8_t &vibe) {
    if (len < 5) return Result::Ignored;
    const uint8_t  op = d[0];
    const uint32_t id = readU32(&d[1]);

    // --- suppression ---
    if (op == 1) {
        const int pos = findById(id);
        if (pos < 0) return Result::Ignored;
        removeAt(pos);
        _version++;
        return Result::Removed;
    }
    if (op != 0 || len < 11) return Result::Ignored;

    // --- nouvelle notification : on décode dans des tampons temporaires, pour ne rien
    //     modifier si le paquet est invalide ---
    char app[kAppMax + 1], title[kTitleMax + 1], text[kTextMax + 1];
    const uint32_t time = readU32(&d[5]);
    const uint8_t  v    = d[9];
    size_t pos = 10;
    if (!readString(d, len, pos, app,   sizeof(app)))   return Result::Ignored;
    if (!readString(d, len, pos, title, sizeof(title))) return Result::Ignored;
    if (!readString(d, len, pos, text,  sizeof(text)))  return Result::Ignored;

    int orderPos = findById(id);
    int slot;
    if (orderPos >= 0) {                         // même id : on met à jour la notification existante
        slot = _order[orderPos];
        moveToFront(orderPos);
    } else {
        if (_count == kCapacity) removeAt(_count - 1);   // plein : on supprime la plus ancienne
        slot = freeSlot();
        if (slot < 0) return Result::Ignored;            // ne devrait pas arriver
        for (int i = _count; i > 0; i--) _order[i] = _order[i - 1];
        _order[0] = slot;
        _count++;
    }

    Slot &s = _slots[slot];
    s.used = true;
    s.id   = id;
    s.time = time;
    strcpy(s.app, app);
    strcpy(s.title, title);
    strcpy(s.text, text);

    vibe = v;
    _version++;
    return Result::Added;
}