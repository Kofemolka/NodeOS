#ifndef OTA_H
#define OTA_H

#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <dev.h>

class OTA
{
  public:
    void Init()
    {
      ArduinoOTA.setPassword("admin");
      ArduinoOTA.onStart([]() {
        Serial.println("Start updating");
        Device::Inst().BlinkLed(3);
      });
      ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
        Device::Inst().BlinkLed(5);
      });
      ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        Serial.println();      
      });
      ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
      });
      ArduinoOTA.begin();
    }

    void Loop()
    {
      ArduinoOTA.handle();
    }
};


#endif
