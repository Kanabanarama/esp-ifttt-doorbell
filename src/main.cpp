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

#define sensorPin D1
#define wpsPin D3

const char* ifttt_event = "---YOUR IFTTT EVENT---";
const char* ifttt_key = "---YOUR IFTT KEY---";

const char* host = "https://maker.ifttt.com";
const char* path = "%s/trigger/%s/with/key/%s";

bool makeTheCall = false;

// Interrupt routine that mustn't do much so it is not blocking
void ICACHE_RAM_ATTR ISRoutine () {
  makeTheCall = true;
}

void checkIfCallHasToBeMade() {
  if(!makeTheCall || WiFi.status() != WL_CONNECTED) return;

  makeTheCall = false;

  HTTPClient http;

  char url[32];
  sprintf(url, path, host, ifttt_key, ifttt_key);

  http.begin(url);
  http.addHeader("Content-Type", "text/plain");

  int httpCode = http.POST("Message from ESP8266");
  String payload = http.getString();

  Serial.println(httpCode);
  Serial.println(payload);

  http.end();
}

bool startWpsConfiguration() {
  Serial.println("WPS-Konfiguration gestartet.");
  bool wpsSuccess = WiFi.beginWPSConfig();
  if(wpsSuccess) {
      String newSSID = WiFi.SSID();
      if(newSSID.length() > 0) {
        Serial.printf("WPS-Konfiguration erfolgreich. SSID: '%s'\n", newSSID.c_str());
      } else {
        wpsSuccess = false;
      }
  }
  return wpsSuccess;
}

void setup()
{
    pinMode(wpsPin, INPUT_PULLUP);
    pinMode(sensorPin, INPUT);

    Serial.begin(9600);
    delay(1000);

    WiFi.mode(WIFI_STA);

    String wifiSSID = WiFi.SSID().c_str();
    String wifiPSK = WiFi.psk().c_str();

    if(wifiSSID.length() > 0) {
      WiFi.begin(wifiSSID, wifiPSK);
      int retry = 0;
      while ((WiFi.status() == WL_DISCONNECTED) && (retry < 10)) {
        delay(500);
        Serial.print(".");
        retry++;
      }

      wl_status_t status = WiFi.status();
      if(status == WL_CONNECTED) {
        Serial.println("verbunden.");
        Serial.printf("SSID: '%s'\n", WiFi.SSID().c_str());
        Serial.printf("IP address: '%s'", WiFi.localIP().toString().c_str());
      }
    } else {
      Serial.println("Keine WiFi Verbindung möglich.");
      Serial.println("Bitte WPS Taste am Router und am Gerät drücken.");

      while(digitalRead(wpsPin)!=0) { yield(); }
      if(!startWpsConfiguration()) {
         Serial.println("WPS-Konfiguration fehlgeschlagen.");
      }
    }

    attachInterrupt(digitalPinToInterrupt(sensorPin), ISRoutine, RISING);
}

void loop() {
  checkIfCallHasToBeMade();
  delay(50);
}
