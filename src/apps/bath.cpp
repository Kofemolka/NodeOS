#ifdef APP_BATH

#include <app.h>
#include <mqtt.h>
#include <persist.h>
#include <Ticker.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define MovePin D6
#define DhtPin D5
#define RelayPin D2

class BathApp : public App
{
  static BathApp* _this;
  const char* K_RELAY = "BathApp.Relay";

public:
  BathApp(MQTT* mqtt) :
   App(mqtt),
   dht(DhtPin, DHT22)
  {
      _this = this;
  }

  void Init()
  {
      pinMode(MovePin, INPUT);
      pinMode(RelayPin, OUTPUT);

      bool def = 0;
      digitalWrite(RelayPin, Persist::Inst().Get(K_RELAY, def));

      dht.begin();
      timeToRead = false;
      tickerDht.attach(30, BathApp::markTimeToRead);

      wasMovement = false;
      timeToSendMovement = false;
      tickerMovement.attach(5, BathApp::sendMovement);

      mqtt->Subscribe("fan", [=](const String& d) { this->onFan(d); });
  }

  void Loop()
  {
      if( timeToRead)
      {
          timeToRead = false;
          readDht();
      }

      wasMovement = wasMovement || digitalRead(MovePin);

      if(timeToSendMovement)
      {
          timeToSendMovement = false;
          mqtt->Publish("move", wasMovement ? "1" : "0");
          wasMovement = false;
      }
  }

  void Shutdown()
  {

  }

protected:
  void onFan(const String& d)
  {
      bool on = d == "1";
      digitalWrite(RelayPin, !on);
      Persist::Inst().Put(K_RELAY, !on);
  }

  static void markTimeToRead()
  {
    _this->timeToRead = true;
  }

  static void sendMovement()
  {
      _this->timeToSendMovement = true;
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

private:
    Ticker tickerDht;
    Ticker tickerMovement;
    DHT dht;
    bool timeToRead;
    bool wasMovement;
    bool timeToSendMovement;
};

App* CreateApp(MQTT* mqtt)
{
  return new BathApp(mqtt);
}

BathApp* BathApp::_this = 0;

#endif
