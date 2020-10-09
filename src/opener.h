#include <Arduino.h>

#include "config.h"

class Oeffner {
  private:
    boolean open;
    uint8_t pin;
    int duration;
    int remaining;
  public:
    Oeffner(uint8_t pin, int duration);
    void pulse();
    void setup();
    void loop();
};
