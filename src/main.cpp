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

#define sensorPin D8
#define sensorPinInverted D9
#define wpsPin D3
#define wpsLed LED_BUILTIN

EasyButton wpsButton(wpsPin);

String wifiHostname = "ESPKlingel-" + String(ESP.getChipId(), HEX);

const char* ifttt_event = "---YOUR IFTTT EVENT---";
const char* ifttt_key = "---YOUR IFTT KEY---";

const char* host = "https://maker.ifttt.com";
const char* path = "%s/trigger/%s/with/key/%s";

bool makeACall = false;

// Interrupt routine that mustn't do much so it is not blocking
void ICACHE_RAM_ATTR ISRoutine () {
  makeACall = true;
}

void checkIfCallHasToBeMade() {
  if(!makeACall || WiFi.status() != WL_CONNECTED) return;

  makeACall = false;

  HTTPClient http;

  char url[32];
  sprintf(url, path, host, ifttt_key, ifttt_key);

  WiFiClient client;
  http.begin(client, url);
  http.addHeader("Content-Type", "text/plain");

  int httpCode = http.POST("Message from ESP8266");
  String payload = http.getString();

  Serial.println(httpCode);
  Serial.println(payload);

  http.end();
}

bool startWpsConfiguration() {
  Serial.println("WPS-Konfiguration gestartet.");
  digitalWrite(wpsLed, LOW);
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
  digitalWrite(wpsLed, HIGH);
  Serial.println(wpsSuccess);
  return wpsSuccess;
}

bool eraseWpsConfiguration() {
  Serial.println("Resetting WPS data.");
  digitalWrite(wpsLed, LOW);
  ESP.eraseConfig();
  delay(1000);
  digitalWrite(wpsLed, HIGH);
  ESP.reset();

  return true;
}

boolean buttonState = false;

boolean buttonActive = false;

long buttonTimer = 0;
unsigned int longPressTime = 1000;

void setup()
{
    pinMode(wpsPin, INPUT_PULLUP);
    pinMode(wpsLed, OUTPUT);
    digitalWrite(wpsLed, HIGH);
    pinMode(sensorPin, INPUT);
    pinMode(sensorPinInverted, INPUT);

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
    WiFi.setAutoConnect(true);
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

    attachInterrupt(digitalPinToInterrupt(sensorPin), ISRoutine, RISING);
    attachInterrupt(digitalPinToInterrupt(sensorPinInverted), ISRoutine, FALLING);
}

void loop() {
  wpsButton.read();
  checkIfCallHasToBeMade();
}
