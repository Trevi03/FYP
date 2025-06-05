#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include <Arduino.h>

void setupLEDs();
void toggleBlinkLED(String color); // toggle implementation the blink (blink or not blinking)
void startPulse(String color); // pulse once
void checkLED(); // Calculates and implement the blink or pulse

#endif
