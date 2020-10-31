#include <Arduino.h>
#include <Ticker.h>

#include "config.h"

class Oeffner {
  private:
    uint8_t pin;
    boolean latched;
    unsigned long start;
    unsigned int duration;

  public:
    Oeffner(uint8_t pin);
    void latchFor(unsigned long duration);
    void setup();
    void loop();
};
