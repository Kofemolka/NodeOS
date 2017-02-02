#ifdef APP_DHT_IR

#include <functional>
#include <app.h>
#include <mqtt.h>
#include <IRremoteESP8266.h>
#include <Ticker.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define LedPin D7
#define DhtPin D5

class DhtIrApp : public App
{
  static DhtIrApp* _this;
public:
  DhtIrApp(MQTT* mqtt) :
   App(mqtt),
   irsend(LedPin),
   dht(DhtPin, DHT22)
  {
      _this = this;
  }

  void Init()
  {
     dht.begin();
     timeToRead = false;
     tickerDht.attach(30, DhtIrApp::markTimeToRead);

     irsend.begin();

     mqtt->Subscribe("tv", [=](const String& d) { this->onTvMsg(d); });
     mqtt->Subscribe("audio", [=](const String& d) { this->onAudioMsg(d); });
  }

  void Loop()
  {
    if( timeToRead)
    {
      timeToRead = false;
      readDht();
    }
  }

  void Shutdown()
  {

  }
protected:
  static void markTimeToRead()
  {
    _this->timeToRead = true;
  }

  void readDht()
  {
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t) ) {
      Serial.println("Failed to read from DHT sensor!");
      mqtt->Publish("temp", "error");
      //return;
    }

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" *C");

    mqtt->Publish("temp", String(t).c_str());
    mqtt->Publish("hum", String(h).c_str());
  }

  void onTvMsg(const String& d)
  {
    if(d == "on")
    {
      for(int i=0; i<15;i++)
      {
        irsend.sendPanasonic(0x4004, 0x0100BCBD);
        delayMicroseconds(30000);
      }
    }
    else
    {
      for(int i=0; i<3;i++)
      {
        irsend.sendPanasonic(0x4004, 0x0100BCBD);
        delayMicroseconds(30000);
      }
    }
  }

  void onAudioMsg(const String& d)
  {
    unsigned long code = strtol(d.c_str(), 0, 16);

    irsend.sendNEC(code, 32);
  }

private:
    IRsend irsend;
    Ticker tickerDht;
    DHT dht;
    bool timeToRead;
};

App* CreateApp(MQTT* mqtt)
{
  return new DhtIrApp(mqtt);
}

DhtIrApp* DhtIrApp::_this = 0;

#endif
