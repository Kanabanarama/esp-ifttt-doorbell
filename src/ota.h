#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#include "config.h"

class Ota
{
  private:
    String FirmwareVersion={"1.0.0"}; 

  public:
    void setup();
    void loop();
    void check();
};
