
#include <Arduino.h>
#include "Buttons.h"
#include "Battery.h"
#include "Ui.h"
#include "BleLink.h"
#include "Clock.h"

Buttons buttons;
Battery battery;
Ui ui;
BleLink ble;
Clock   deviceClock;

void setup() {
    Serial.begin(115200);
    buttons.begin();
    battery.begin();
    ui.begin();
    ble.begin();
}

void loop() {
    buttons.update();
    battery.update();
    ui.handleInput(buttons);
    ui.updateBattery(battery);
    ui.draw();
    ble.update(deviceClock);
    ble.setBattery(battery.getPercentage());
    ui.updateClock(deviceClock);
    delay(5);
}