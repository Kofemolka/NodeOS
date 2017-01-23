#ifndef APP_H
#define APP_H

#include <mqtt.h>

class App
{
public:
  App(MQTT* mqtt) : mqtt(mqtt)
  {

  }

  virtual void Init() {};
  virtual void Loop() {};
  virtual void Shutdown() {};

protected:
  MQTT* mqtt;
};

extern App* CreateApp(MQTT* mqtt);

#endif
