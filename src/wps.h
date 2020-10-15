#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DNSServer.h>
#include <EasyButton.h>
#include <jled.h>

#include "config.h"

class Wps
{
  private:
    //String wifiHostname = "ESPKlingel-" + String(ESP.getChipId(), HEX);
    String wifiHostname = "klingel";
    DNSServer dnsServer;
    uint8_t wpsButtonPin;
    uint8_t wpsLedPin;
    EasyButton wpsButton;
    static JLed wpsLed;
    static bool bellSignalDetected;

    static void startWpsConfiguration();
    static void eraseWpsConfiguration();

  public:
    Wps(uint8_t buttonPin, uint8_t ledPin);
    void setup();
    void loop();
};
