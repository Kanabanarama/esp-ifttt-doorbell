#include <opener.h>

Oeffner::Oeffner(uint8_t pin) {
  this->pin = pin;
}

void Oeffner::latchFor(unsigned long duration) {
  Serial.println("Opening door.");
  digitalWrite(this->pin, HIGH);
  this->start = millis();
  this->duration = duration;
  this->latched = false;
}

void Oeffner::setup() {
  pinMode(this->pin, OUTPUT);
  digitalWrite(this->pin, LOW);
  this->latched = true;
}

void Oeffner::loop() {
  unsigned long current = millis();
  if (!this->latched && current - this->start >= this->duration) {
    Serial.println("Closing door.");
    this->latched = true;
    digitalWrite(this->pin, LOW);
  }
}
