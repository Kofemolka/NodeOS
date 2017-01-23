#ifdef APP_IRSEND

#include <app.h>
#include <mqtt.h>
#include <IRremoteESP8266.h>

#define LedPin D1

IRsend irsend(LedPin);

class IRSendApp : public App
{
public:
  IRSendApp(MQTT* mqtt) : App(mqtt)
  {

  }

  void Init()
  {
  //  pinMode(LedPin, OUTPUT);
    //digitalWrite(LedPin, 0);
     irsend.begin();

    mqtt->Subscribe("tv", [=](const String& d) { this->onTvMsg(d); });
    mqtt->Subscribe("audio", [=](const String& d) { this->onAudioMsg(d); });
  }

  void Loop()
  {

  }

  void Shutdown()
  {

  }
protected:
  void onTvMsg(const String& d)
  {
    int b = 502;
    int h = 1244;
    int l = 400;

    unsigned int buf[] = { 3540, 1673,
      b, l,
      b, h,
      b, l,
      b, l,

      b, l,
      b, l,
      b, l,
      b, l,

      b, l,
      b, l,
      b, l,
      b, l,

      b, l,
      b, h,
      b, l,
      b, l,

      b, l,
      b, l,
      b, l,
      b, l,

      b, l,
      b, l,
      b, l,
      b, h,

      b, l,
      b, l,
      b, l,
      b, l,

      b, l,
      b, l,
      b, l,
      b, l,

      b, h,
      b, l,
      b, h,
      b, h,

      b, h,
      b, h,
      b, l,
      b, l,

      b, h,
      b, l,
      b, h,
      b, h,

      b, h,
      b, h,
      b, l,
      b, h,

      b, 74950
    };

    int bufLen = (sizeof(buf)/sizeof(unsigned int));

    for(int i=0; i<12; i++)
    {
      irsend.sendRaw(buf, bufLen, 38);
    }
  }

  void onAudioMsg(const String& d)
  {
    unsigned long code = strtol(d.c_str(), 0, 16);
    Serial.printf("Audio Code: %X\n", code);

    irsend.sendNEC(code, 32);

    /*unsigned int buf[] = {
      4450, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1650, 650, 550, 650, 550, 650, 550, 700, 1650, 650, 1650, 650, 1650, 650, 550, 650, 550, 650, 1650, 650, 1650,
      700, 1650, 650, 550, 700, 1650, 650, 550, 700, 1650, 700, 500, 650, 550, 650, 550, 650, 550, 700, 1650, 700, 500, 650, 1650, 650, 550, 650, 1650, 650, 1650, 650,
      1650, 700, 1650, 650
    };

    int bufLen = (sizeof(buf)/sizeof(unsigned int));
    irsend.sendRaw(buf, bufLen, 38);*/
  }

private:

};

App* CreateApp(MQTT* mqtt)
{
  return new IRSendApp(mqtt);
}

#endif
