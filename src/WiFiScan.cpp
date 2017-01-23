

#include <config.h>
#include <wifi.h>
#include <ota.h>
#include <mqtt.h>
#include <app.h>

Wifi wifi;
OTA ota;
MQTT mqtt;
App* app = CreateApp(&mqtt);

const String DEVID = String(ESP.getChipId(), HEX);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Booting...");

  wifi.init();
  ota.Init();
  mqtt.Init();

  app->Init();

  Serial.println("Ready");
}

void loop()
{
  ota.Loop();
  mqtt.Loop();
  app->Loop();
}
