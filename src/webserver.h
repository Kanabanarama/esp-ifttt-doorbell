#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <vector>
#include <string>

#include "config.h"

using namespace std;

class Webserver {
  public:
    void setup();
    void loop();
    boolean received(string signalName);
};
