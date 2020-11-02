#include  "ota.h"

void Ota::setup() {}

void Ota::loop() {}

void Ota::check()
{
  HTTPClient http;
  http.begin(OTA_URL_FW_VERSION, OTA_URL_FW_CERT);
  delay(100);
  int httpCode = http.GET();
  delay(100);
  String payload;

  if (httpCode == HTTP_CODE_OK) {
    payload = http.getString();
    Serial.println("Newest version is:");
    Serial.println(payload);
  } else {
    Serial.print("Error in downloading version file:");
    Serial.println(httpCode);
  }
  
  http.end();

  if (httpCode == HTTP_CODE_OK) {
    if(payload.equals(FirmwareVersion)) {   
      Serial.println("Device already on latest firmware version"); 
  } else {
     Serial.println("New firmware detected:");
     Serial.println(payload);
     WiFiClient client;

    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

    t_httpUpdate_return result = ESPhttpUpdate.update(OTA_URL_FW_BIN, "", OTA_URL_FW_CERT);
    
    switch (result) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    }
  }
 }  
}
