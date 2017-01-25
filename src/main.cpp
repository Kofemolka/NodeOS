#include <config.h>
#include <wifi.h>
#include <ota.h>
#include <mqtt.h>
#include <dev.h>
#include <persist.h>
#include <app.h>

Wifi wifi;
OTA ota;
MQTT mqtt;
App* app= 0;

Device* Device::_inst;
Persist* Persist::_inst;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("Booting...");

  app = CreateApp(&mqtt);
  
  wifi.init();
  ota.Init();
  mqtt.Init();
  Device::Inst().Init(&mqtt, app);

  app->Init();

  Serial.println("Ready");
  Device::Inst().BlinkLed(3);
}

void loop()
{
  ota.Loop();
  mqtt.Loop();
  app->Loop();
}
