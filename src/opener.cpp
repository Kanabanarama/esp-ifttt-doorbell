#include <opener.h>

Oeffner::Oeffner(uint8_t pin) {
  this->pin = pin;
}

void Oeffner::latchFor(unsigned int duration) {
  Serial.println("Opening door.");
  digitalWrite(this->pin, LOW);
  delay(duration);
  Serial.println("Closing door.");
  digitalWrite(this->pin, HIGH);
}

void Oeffner::setup() {
  pinMode(this->pin, OUTPUT);
  digitalWrite(this->pin, HIGH);
}

void Oeffner::loop() { }
