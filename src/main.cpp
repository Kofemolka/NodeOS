#include <config.h>
#include <wifi.h>
#include <ota.h>
#include <mqtt.h>
#include <dev.h>
#include <app.h>

Wifi wifi;
OTA ota;
MQTT mqtt;
App* app = CreateApp(&mqtt);

Device* Device::_inst;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Booting...");

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
