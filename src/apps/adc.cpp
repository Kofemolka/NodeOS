#ifdef APP_ADC

#include <app.h>
#include <mqtt.h>
#include <Ticker.h>

class AdcApp : public App
{
  Ticker ticker;
public:
  AdcApp(MQTT* mqtt) :
   App(mqtt)
  {

  }

  void Init()
  {
    ticker.attach(1, AdcApp::readAdc);
      pinMode(D1, INPUT);
      pinMode(A0, INPUT);
  }

  void Loop()
  {

  }

  void Shutdown()
  {

  }

protected:
  static void readAdc()
  {
    int pwm_value = 0;//pulseIn(D1, HIGH);
    int a = analogRead(A0);
    int d = digitalRead(D1);
    Serial.printf("D:%d A:%d\n", d, a);

  }
private:

};

App* CreateApp(MQTT* mqtt)
{
  return new AdcApp(mqtt);
}

#endif
