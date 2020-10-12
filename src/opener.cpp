#include <opener.h>

Oeffner::Oeffner(uint8_t pin, int duration) {
  this->pin = pin;
  this->duration = duration;
  this->open = false;
  this->latched = true;
}

void Oeffner::pulse() {
  this->open = true;
  this->latched = false;
  this->remaining = this->duration;
}

void Oeffner::setup() {
  digitalWrite(this->pin, HIGH);
  pinMode(this->pin, OUTPUT);
}

void Oeffner::loop() {
  if(this->open && this->remaining > 0) {
    if(!this->latched) {
      Serial.println("Opening door.");
      digitalWrite(this->pin, LOW);
      this->latched = true;
    }
    this->remaining--;
  }

  if(this->open && this->remaining <= 0) {
    this->latched = false;
    if(!this->latched) {
      Serial.println("Closing door.");
      digitalWrite(this->pin, HIGH);
      this->open = false;
      this->latched = true;
    }
    this->remaining = 0;
  }
}
