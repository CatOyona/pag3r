
#include <Arduino.h>
#include "Buttons.h"
#include "Battery.h"
#include "Ui.h"
#include "BleLink.h"
#include "Clock.h"
#include "Haptics.h"
#include "PhoneStatus.h"
#include "NotificationStore.h"

Buttons buttons;
Battery battery;
PhoneStatus phone;
Ui ui(phone);
BleLink ble;
Clock   deviceClock;
Haptics haptics;
NotificationStore notifs;

void setup() {
    Serial.begin(115200);
    buttons.begin();
    battery.begin();
    ui.begin();
    ble.begin();
    haptics.begin();
}

void loop() {
    buttons.update();
    battery.update();
    ui.handleInput(buttons);
    ui.updateBattery(battery);
    ui.draw();
    ble.update(deviceClock, notifs, phone);
    ble.setBattery(battery.getPercentage());
    ui.updateClock(deviceClock);
    haptics.update();
    delay(5);
}