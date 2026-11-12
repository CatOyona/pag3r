#include "Battery.h"
#include "Icons.h"

void Battery::begin() {
    _gaugeOk = _maxlipo.begin();
    if (!_gaugeOk) { Serial.println("Jauge introuvable"); }
    checkBattery();
}

void Battery::update() {
    if (millis() - _lastBatteryRead >= 2000) {
        _lastBatteryRead = millis();
        checkBattery();
    }
    Serial.printf("V=%.2f  %.0f%%  taux=%.1f%%/h\n", _maxlipo.cellVoltage(), _maxlipo.cellPercent(), _maxlipo.chargeRate());
}

void Battery::checkBattery() {
    const float v = _gaugeOk ? _maxlipo.cellVoltage() : 0.0f;
    if (_gaugeOk && v > 2.5f && v < 4.4f) {
        _percentage = constrain((int) (_maxlipo.cellPercent() + 0.5f), 0, 100);
        const float rate = _maxlipo.chargeRate();
        bool charging = rate > 1.0f;
        if (charging)               _state = State::Charging;
        else if (_percentage >= 99) _state = State::Full;
        else if (_percentage >= 75) _state = State::High;
        else if (_percentage >= 50) _state = State::Mid;
        else if (_percentage >= 25) _state = State::Low;
        else                        _state = State::Critical;
    } else {
        _percentage = -1;
        _state = State::Unknown;
    }
}

Battery::State Battery::getState() const {
    return _state;
}

const uint16_t* Battery::getIcon() const {
    switch(_state) {
        case State::Charging: return ICON_BATTERY_CHARGING;
        case State::Full: return ICON_BATTERY_FULL;
        case State::High: return ICON_BATTERY_HIGH;
        case State::Mid: return ICON_BATTERY_MID;
        case State::Low: return ICON_BATTERY_LOW;
        case State::Critical: return ICON_BATTERY_CRITICAL;
        case State::Unknown: return ICON_BATTERY_UNKNOWN;
    }
    return ICON_BATTERY_UNKNOWN;
}

int Battery::getPercentage() const {
    return _percentage;
}