#include "ScreenManager.h"

void ScreenManager::registerScreen(ScreenId id, Screen *screen) {
    const int i = static_cast<int>(id);
    if (i >= 0 && i < kCount) _screens[i] = screen;
}

Screen *ScreenManager::find(ScreenId id) const {
    const int i = static_cast<int>(id);
    return (i >= 0 && i < kCount) ? _screens[i] : nullptr;
}

void ScreenManager::begin(ScreenId root) {
    Screen *s = find(root);
    if (!s) return;
    _stack.push(s);
    s->onEnter(0);
    s->invalidate();
}

void ScreenManager::handleInput(Buttons &b) {
    Screen *s = _stack.peek();
    if (!s) return;
    apply(s->handleInput(b));
}

void ScreenManager::apply(const Transition &t) {
    switch (t.type) {
        case Transition::Type::None:
            return;

        case Transition::Type::Push: {
            Screen *s = find(t.target);
            if (!s || s == _stack.peek()) return;   // inconnu, ou déjà affiché
            if (!_stack.push(s)) return;            // pile pleine
            s->onEnter(t.arg);
            break;
        }

        case Transition::Type::Pop:
            if (_stack.size() <= 1) return;         // on ne dépile jamais l'écran racine
            _stack.pop();
            break;

        case Transition::Type::Replace: {
            Screen *s = find(t.target);
            if (!s) return;
            while (_stack.pop() != nullptr) {}      // vide la pile
            _stack.push(s);
            s->onEnter(t.arg);
            break;
        }
    }

    // Après tout changement, l'écran qui devient actif doit se redessiner.
    if (Screen *top = _stack.peek()) top->invalidate();
}

void ScreenManager::draw(GFXcanvas16 &c, U8G2_FOR_ADAFRUIT_GFX &g) {
    Screen *s = _stack.peek();
    if (!s) return;
    s->draw(c, g);
    s->clearDirty();
}

bool ScreenManager::isDirty() const {
    const Screen *s = _stack.peek();
    return s && s->isDirty();
}