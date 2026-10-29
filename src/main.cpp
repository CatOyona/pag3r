
#include <Arduino.h>
#include "Buttons.h"
#include "Battery.h"
#include "Ui.h"

Buttons buttons;
Battery battery;
Ui ui;

void setup() {
    Serial.begin(115200);
    buttons.begin();
    battery.begin();
    ui.begin();

}

void loop() {
    buttons.update();
    battery.update();
    ui.handleInput(buttons);
    ui.updateBattery(battery);
    ui.draw();

    delay(5);
}