#include <Arduino.h>
#include <Ticker.h>

#include "config.h"

class Oeffner {
  private:
    uint8_t pin;
    int duration;

  public:
    Oeffner(uint8_t pin);
    void latchFor(unsigned int duration);
    void setup();
    void loop();
};
