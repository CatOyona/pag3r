#include <Arduino.h>
#include "Buttons.h"
#include "Ui.h"

Buttons buttons;
Ui ui;

void setup() {
    Serial.begin(115200);
    buttons.begin();
    ui.begin();
}

void loop() {
    buttons.update();
    ui.handleInput(buttons);
    ui.draw();
    delay(5);
}
