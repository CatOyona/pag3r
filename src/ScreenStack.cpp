#include "ScreenStack.h"

bool ScreenStack::push(Screen* s) {
    if (_size >= kSize) {
        return false;
    } else {
        _stack[_size] = s;
        _size++;
        return true;
    }
}

Screen* ScreenStack::pop() {
    if (_size == 0) {
        return nullptr;
    } else {
        _size--;
        Screen* toUnstack = _stack[_size];
        _stack[_size] = nullptr;
        return toUnstack;
    }
}

Screen* ScreenStack::peek() const {
    if (_size == 0) {
        return nullptr;
    } else {
        return _stack[_size - 1];
    }
}