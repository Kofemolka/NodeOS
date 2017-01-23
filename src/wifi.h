#ifndef WIFI_H
#define WIFI_H

#include <ESP8266WiFi.h>
#include <config.h>

class Wifi
{
public:
  void init()
  {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Connection Failed! Rebooting...");
      delay(5000);
      ESP.restart();
    }
    WiFi.setAutoReconnect(true);

    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
  }
};

#endif
