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

const char* wifi_ssid     = "---YOUR SSID---";
const char* wifi_password = "---YOUR WIFI PASSWORD---";

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

void setup()
{
    pinMode(sensorPin, INPUT);

    Serial.begin(9600);
    delay(1000);

    Serial.println();
    Serial.print("Connecting to: ");
    Serial.println(wifi_ssid);

    WiFi.begin(wifi_ssid, wifi_password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    attachInterrupt(digitalPinToInterrupt(sensorPin), ISRoutine, RISING);
}

void loop() {
  checkIfCallHasToBeMade();
  delay(50);
}
