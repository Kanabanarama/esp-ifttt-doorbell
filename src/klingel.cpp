#include "klingel.h"

bool Klingel::bellSignalDetected;

// Interrupt routine that mustn't do much so it is not blocking
void ICACHE_RAM_ATTR Klingel::ISRoutine() {
  Klingel::bellSignalDetected = true;
}

void Klingel::call() {
  if(WiFi.status() != WL_CONNECTED) return;

  //Klingel::bellSignalDetected = false;

  //espLed.Breathe(1000).Repeat(3);

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

  //espLed.FadeOn(1000).DelayBefore(1000);

  http.end();
}

Klingel::Klingel(uint8_t sensorPin, uint8_t sensorPinInverted) {
  this->sensorPin = sensorPin;
  this->sensorPinInverted = sensorPinInverted;
  Klingel::bellSignalDetected = false;
}

void Klingel::setup() {
  //Klingel::bellSignalDetected = false;

  pinMode(this->sensorPin, INPUT);
  pinMode(this->sensorPinInverted, INPUT);

  attachInterrupt(digitalPinToInterrupt(this->sensorPin), &Klingel::ISRoutine, RISING);
  attachInterrupt(digitalPinToInterrupt(this->sensorPinInverted), Klingel::ISRoutine, FALLING);

  // To check if IFTTT calls work:
  //call();
}

void Klingel::loop() {
  if(Klingel::bellSignalDetected) call();
}
