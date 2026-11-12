#pragma once
#include <Arduino.h>

class Haptics {
    public:
        void begin();
        void update();
        
        void shortVibration();
        void doubleVibration();
        void longVibration();
        void stop();
        bool isActive() const { return _active; }
    private:
        struct Segment {
            bool on;
            uint16_t ms;
        };

        static constexpr int kMaxSegments = 8;

        void start(const Segment* pattern, int count);
        void apply(bool on);

        Segment _pattern[kMaxSegments];
        int _count = 0;
        int _index = 0;
        uint32_t _segStart =0;
        bool _active = false;
};