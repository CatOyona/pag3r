#pragma once

#include "Screen.h"

class ScreenStack {
    public:
        bool push(Screen* s);
        Screen* pop();
        Screen* peek() const;
        int size() const { return _size; }
    private:
        static constexpr int kSize = 6;
        Screen* _stack[kSize] = {};
        int _size = 0;
};