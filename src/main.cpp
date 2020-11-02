/*
 * This program sends a request to IFTTT to call your phone on GPIO input events.
 *
 * You need a ifttt account with Webhooks service enabled. In your Webhooks
 * settings you will find the url to trigger your hooks. You can create an
 * event for a hook, for example, call your phone via the VoIP Calls service.
 *
 */

#include <EasyButton.h>
#include <jled.h>

#include "config.h"

#include "wps.h"
#include "ota.h"
#include "webserver.h"

#define ENABLE_OTA true
#define USE_AS_BELL true
#define USE_AS_OPENER true

#ifdef USE_AS_BELL
  #include "klingel.h"
#endif

#ifdef USE_AS_OPENER
  #include "opener.h"
#endif

auto espLed = JLed(ESP_LED).LowActive();
Wps wpsWizard = Wps(WPS_PIN, WPS_LED);
Ota otaUpdates = Ota();
Klingel doorbell = Klingel(SENSOR_PIN, SENSOR_PIN_INVERTED);
Oeffner opener = Oeffner(OPENER_PIN);
Webserver webserver = Webserver();

void setup() {
    doorbell.setup();
    opener.setup();
    Serial.begin(9600);
    wpsWizard.setup();
    webserver.setup();
    otaUpdates.setup();
}

void loop() {
    espLed.Update();
    wpsWizard.loop();
    webserver.loop();
    doorbell.loop();
    opener.loop();

    if(webserver.received("connect")) {
      Serial.println("Received request for WPS setup");
      wpsWizard.startWpsConfiguration();
    }
    if(webserver.received("update")) {
      Serial.println("Received request for OTA update");
      otaUpdates.check();
    }
    if(webserver.received("open")) {
      Serial.println("Received request to open door");
      opener.latchFor(CONTACT_DURATION);
    }
}
