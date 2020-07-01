/*
 * This program sends a request to IFTTT to call your phone on GPIO input events.
 *
 * You need a ifttt account with Webhooks service enabled. In your Webhooks
 * settings you will find the url to trigger your hooks. You can create an
 * event for a hook, for example, call your phone via the VoIP Calls service.
 *
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <EasyButton.h>

#include "config.h"

EasyButton wpsButton(WPS_PIN);

String wifiHostname = "ESPKlingel-" + String(ESP.getChipId(), HEX);

const char* host = "http://maker.ifttt.com";
const char* path = "%s/trigger/%s/with/key/%s";

bool makeACall = false;

// Interrupt routine that mustn't do much so it is not blocking
void ICACHE_RAM_ATTR ISRoutine () {
  makeACall = true;
}

void checkIfCallHasToBeMade() {
  if(!makeACall || WiFi.status() != WL_CONNECTED) return;

  makeACall = false;

  WiFiClient client;
  HTTPClient http;

  char url[128];
  sprintf(url, path, host, IFTTT_EVENT, IFTTT_KEY);
  Serial.printf("Request: '%s'\n", url);

  http.begin(client, url);
  http.addHeader("Content-Type", "text/plain");
  int httpCode = http.POST("Message from ESP8266");
  String payload = http.getString();

  Serial.println("---ANTWORT:---");
  Serial.println(httpCode);
  Serial.println("--------------");
  Serial.println(payload);
  Serial.println("--------------");

  http.end();
}

bool startWpsConfiguration() {
  Serial.println("WPS-Konfiguration gestartet.");
  digitalWrite(WPS_LED, LOW);
  bool wpsSuccess = WiFi.beginWPSConfig();
  if(wpsSuccess) {
      String newSSID = WiFi.SSID();
      if(newSSID.length() > 0) {
        Serial.printf("WPS-Konfiguration erfolgreich. SSID: '%s'\n", newSSID.c_str());
      } else {
        wpsSuccess = false;
      }
  } else {
    Serial.println("WPS-Konfiguration fehlgeschlagen.");
  }
  digitalWrite(WPS_LED, HIGH);
  Serial.println(wpsSuccess);
  return wpsSuccess;
}

bool eraseWpsConfiguration() {
  Serial.println("Resetting WPS data.");
  digitalWrite(WPS_LED, LOW);
  ESP.eraseConfig();
  delay(1000);
  digitalWrite(WPS_LED, HIGH);
  ESP.reset();

  return true;
}

void setup()
{
    pinMode(WPS_PIN, INPUT_PULLUP);
    pinMode(WPS_LED, OUTPUT);
    digitalWrite(WPS_LED, HIGH);
    pinMode(SENSOR_PIN, INPUT);
    pinMode(SENSOR_PIN_INVERTED, INPUT);

    delay(1000);
    Serial.begin(9600);
    delay(1000);

    Serial.printf("Ready (%s)\n", wifiHostname.c_str());

    wpsButton.onPressed(startWpsConfiguration);
    wpsButton.onPressedFor(3000, eraseWpsConfiguration);

    WiFi.mode(WIFI_STA);

    String wifiSSID = WiFi.SSID().c_str();
    String wifiPSK = WiFi.psk().c_str();

    if(wifiSSID.length() <= 0) {
      Serial.println("Keine WiFi Verbindung möglich.");
      Serial.println("Bitte WPS Taste am Router und am Gerät drücken.");
      return;
    }

    Serial.printf("Verbindung mit '%s' wird hergestellt.\n", wifiSSID.c_str());
    WiFi.begin(wifiSSID, wifiPSK);
    WiFi.hostname(wifiHostname);
    int retry = 0;
    while ((WiFi.status() == WL_DISCONNECTED) && (retry < 15)) {
      delay(500);
      Serial.print(".");
      retry++;
    }

    wl_status_t status = WiFi.status();
    if(status == WL_CONNECTED) {
      Serial.println("verbunden.");
      Serial.printf("SSID: '%s'\n", WiFi.SSID().c_str());
      Serial.printf("IP address: '%s'\n", WiFi.localIP().toString().c_str());
    }

    attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), ISRoutine, RISING);
    attachInterrupt(digitalPinToInterrupt(SENSOR_PIN_INVERTED), ISRoutine, FALLING);
}

void loop() {
  wpsButton.read();
  checkIfCallHasToBeMade();
}
