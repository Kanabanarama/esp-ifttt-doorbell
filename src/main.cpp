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
#include <jled.h>

#include "config.h"

EasyButton wpsButton(WPS_PIN);
auto espLed = JLed(ESP_LED).LowActive();

String wifiHostname = "ESPKlingel-" + String(ESP.getChipId(), HEX);

const char* host = "http://maker.ifttt.com";
const char* path = "%s/trigger/%s/with/key/%s";

bool bellSignalDetected = false;

// Interrupt routine that mustn't do much so it is not blocking
void ICACHE_RAM_ATTR ISRoutine () {
  bellSignalDetected = true;
}

void call() {
  if(WiFi.status() != WL_CONNECTED) return;

  bellSignalDetected = false;

  espLed.Breathe(1000).Repeat(3);

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

  espLed.FadeOn(1000).DelayBefore(1000);

  http.end();
}

bool startWpsConfiguration() {
  Serial.println("WPS-Konfiguration gestartet.");
  espLed.On().Update();
  bool wpsSuccess = WiFi.beginWPSConfig();
  if(wpsSuccess) {
      String newSSID = WiFi.SSID();
      if(newSSID.length() > 0) {
        Serial.printf("WPS-Konfiguration erfolgreich. SSID: '%s'\n", newSSID.c_str());
      }
  } else {
    Serial.println("WPS-Konfiguration fehlgeschlagen.");
  }
  espLed.Off().Update();
  return wpsSuccess;
}

bool eraseWpsConfiguration() {
  Serial.println("Resetting WPS data.");
  espLed.On().DelayAfter(500).Update();
  ESP.eraseConfig();
  espLed.Off().Update();
  ESP.reset();

  return true;
}

void setup()
{
    pinMode(WPS_PIN, INPUT_PULLUP);
    pinMode(SENSOR_PIN, INPUT);
    pinMode(SENSOR_PIN_INVERTED, INPUT);

    espLed.Off().Update();

    Serial.begin(9600);
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
      espLed.FadeOn(1000);
    }

    attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), ISRoutine, RISING);
    attachInterrupt(digitalPinToInterrupt(SENSOR_PIN_INVERTED), ISRoutine, FALLING);

    // To check if IFTTT calls work:
    //call();
}

void loop() {
  wpsButton.read();
  espLed.Update();
  if(bellSignalDetected) call();
}
