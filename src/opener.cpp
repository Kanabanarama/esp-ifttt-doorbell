#include <opener.h>

Oeffner::Oeffner(uint8_t pin, int duration) {
  this->pin = pin;
  this->duration = duration;
}

void Oeffner::pulse() {
  this->open = true;
  this->remaining = this->duration;
}

void Oeffner::setup() {
   pinMode(this->pin, OUTPUT);
}

void Oeffner::loop() {
  if(this->open && this->remaining > 0) {
    digitalWrite(this->pin, LOW);
    this->remaining--;
  } else {
    digitalWrite(this->pin, HIGH);
    this->remaining = 0;
  }
}
