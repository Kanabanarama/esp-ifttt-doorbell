#include <wps.h>

JLed Wps::wpsLed(WPS_LED);

bool Wps::startWpsConfiguration() {
    Serial.println("WPS-Konfiguration gestartet.");
    wpsLed.On().Update();
    bool wpsSuccess = WiFi.beginWPSConfig();
    if(wpsSuccess) {
        String newSSID = WiFi.SSID();
        if(newSSID.length() > 0) {
            Serial.printf("WPS-Konfiguration erfolgreich. SSID: '%s'\n", newSSID.c_str());
        }
    } else {
        Serial.println("WPS-Konfiguration fehlgeschlagen.");
    }
    wpsLed.Off().Update();
    return wpsSuccess;
}

bool Wps::eraseWpsConfiguration() {
    Serial.println("WPS-Daten zurückgesetzt.");
    wpsLed.On().DelayAfter(500).Update();
    ESP.eraseConfig();
    wpsLed.Off().Update();
    ESP.reset();
    return true;
}

Wps::Wps(uint8_t buttonPin, uint8_t ledPin)
: wpsButton(buttonPin) {
  this->wpsButtonPin = buttonPin;
  this->wpsLedPin = ledPin;
  Wps::wpsButton = wpsButton;
  //Wps::wpsLed = wpsLed;
}

void Wps::setup() {
  pinMode(this->wpsButtonPin, INPUT_PULLUP);

  this->wpsButton.onPressed(startWpsConfiguration);
  this->wpsButton.onPressedFor(3000, eraseWpsConfiguration);

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
      Serial.printf("IP: '%s'\n", WiFi.localIP().toString().c_str());
      Serial.printf("HOSTNAME '%s'\n", wifiHostname.c_str());
      wpsLed.FadeOn(1000);
  }
}

void Wps::loop() {
  this->wpsButton.read();
  wpsLed.Update();
}
