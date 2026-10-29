#pragma once
#include <Arduino.h>
#include <Adafruit_MAX1704X.h>

class Battery {
    public:
        enum class State {
            Critical,
            Low,
            Mid,
            High,
            Full,
            Charging,
            Unknown
        };
        void begin();
        void update();

        State getState() const;
        const uint16_t * getIcon() const;
        int getPercentage() const;


    private:
        int _percentage = -1;
        Adafruit_MAX17048 _maxlipo;
        bool _gaugeOk = false;
        uint32_t _lastBatteryRead = 0;
        State _state = State::Unknown;

        void checkBattery();
};