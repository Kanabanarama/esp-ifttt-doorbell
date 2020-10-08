#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <EasyButton.h>

#include "config.h"

class Klingel {
  private:
    uint8_t sensorPin;
    uint8_t sensorPinInverted;
    const char* host = "http://maker.ifttt.com";
    const char* path = "%s/trigger/%s/with/key/%s";
    static bool bellSignalDetected;

    static void ICACHE_RAM_ATTR ISRoutine();
    void call();

  public:
    Klingel(uint8_t sensorPin, uint8_t sensorPinInverted);
    void setup();
    void loop();
};
