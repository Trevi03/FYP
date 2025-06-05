#include "LEDControl.h"
// LEDR, LEDG, LEDB for Arduino Nano 33 BLE rev2

static unsigned long previousMillis = 0;
static const long blinkInterval = 1000; // 1 second
static bool ledOn = false;
static bool blinking = false;
static bool pulseStarted = false;
static String ledColor = "OFF";

void setLEDColor(String color) {
  bool r, g, b;
  if (color == "WHITE")       { r = g = b = LOW; }
  else if (color == "RED")    { r = LOW;  g = HIGH; b = HIGH; }
  else if (color == "GREEN")  { r = HIGH; g = LOW;  b = HIGH; }
  else if (color == "BLUE")   { r = HIGH; g = HIGH; b = LOW;  }
  else if (color == "YELLOW") { r = LOW;  g = LOW;  b = HIGH; }
  else if (color == "MAGENTA"){ r = LOW;  g = HIGH; b = LOW;  }
  else if (color == "CYAN")   { r = HIGH; g = LOW;  b = LOW;  }
  else                       { r = g = b = HIGH; } // OFF

  digitalWrite(LEDR, r);
  digitalWrite(LEDG, g);
  digitalWrite(LEDB, b);
}

void setupLEDs() {
  // Initialise the pins
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  
  setLEDColor("OFF");
}

void toggleBlinkLED(String color) {
  // toggle implementation the blink (blink or not blinking)
  ledColor = color;
  ledOn = false;  // reset blink state
  setLEDColor("OFF"); // reset blink LED
  previousMillis = millis();
  blinking = !blinking;
}

void startPulse(String color) {
  // pulse once
  if (blinking){
    blinking = !blinking;
  }
  ledColor = color;
  pulseStarted = true;
  previousMillis = millis();
  setLEDColor(color);  // Turn on the color
}

void checkLED() {
  unsigned long  currentMillis = millis();
  // Calculates and implement the blink
  if (blinking && currentMillis - previousMillis >= blinkInterval) {
    // Continuous blinking until toggle off
    previousMillis = currentMillis;
    ledOn = !ledOn;

    if (ledOn) {
      setLEDColor(ledColor);
    } else {
      setLEDColor("OFF");
    }
  }
  else if (pulseStarted && currentMillis - previousMillis >= blinkInterval) {
    // SIngle pulse
    setLEDColor("OFF");
    pulseStarted = false;  // Ensure it doesn't repeat
  }
}