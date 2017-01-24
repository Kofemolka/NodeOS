#ifndef DEV_H
#define DEV_H

#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <mqtt.h>
#include <app.h>

class Device
{
  static const int led = D0;
  static Device* _inst;

public:
  static Device& Inst()
  {
    if(_inst == 0)
    {
      _inst = new Device();
    }

    return *_inst;
  }

  void Init(MQTT* mqtt, App* app)
  {
    _mqtt = mqtt;
    _app = app;
    _mqtt->Subscribe("dev/reset", Device::onReset);

    pinMode(led, OUTPUT);

    devInfoTicker.attach(60, Device::devInfoUpdate);
  }

  void BlinkLed(int times)
  {
    for(int i=0;i<times*2;i++)
    {
      digitalWrite(led, i & 1);
      delay(50);
    }

    digitalWrite(led, 1);
  }

  void Led(bool off = false)
  {
    if(off)
    {
      digitalWrite(led, 1);
    }
    else
    {
      digitalWrite(led, digitalRead(led));
    }
  }

private:
  static void devInfoUpdate()
  {
    Inst()._mqtt->Publish("dev/ip", WiFi.localIP().toString());
    Inst()._mqtt->Publish("dev/heap", String(ESP.getFreeHeap()));
  }

  static void onReset(const String& m)
  {
    Inst()._app->Shutdown();
    ESP.restart();
  }

  Ticker devInfoTicker;
  MQTT* _mqtt;
  App* _app;
};

#endif
