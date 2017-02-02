#ifdef APP_ENTRANCE

#include <app.h>
#include <mqtt.h>
#include <persist.h>
#include <Ticker.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define MovePin D5
#define DhtPin D6
#define DoorPin D7

class EntranceApp : public App
{
  static EntranceApp* _this;

public:
  EntranceApp(MQTT* mqtt) :
   App(mqtt),
   dht(DhtPin, DHT22)
  {
      _this = this;
  }

  void Init()
  {
      pinMode(DoorPin, INPUT);
      pinMode(MovePin, INPUT);

      dht.begin();
      timeToReadDht = false;
      tickerDht.attach(30, EntranceApp::markReadDht);

      doorState = false;
      wasMovement = false;
      timeToReadState = false;
      tickerState.attach(1, EntranceApp::markReadState);
  }

  void Loop()
  {
      if( timeToReadDht)
      {
          timeToReadDht = false;
          readDht();
      }

      if( timeToReadState )
      {
          timeToReadState = false;

          bool movement = digitalRead(MovePin);
          if(wasMovement != movement)
          {
              mqtt->Publish("move", movement ? "1" : "0");
              wasMovement = movement;
          }

          bool door = digitalRead(DoorPin);
          Serial.println(door);
          if(doorState != door)
          {
              mqtt->Publish("door", door ? "1" : "0");
              doorState = door;
          }
      }
  }

  void Shutdown()
  {

  }

protected:
  static void markReadDht()
  {
    _this->timeToReadDht = true;
  }

  static void markReadState()
  {
    _this->timeToReadState = true;
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
    DHT dht;
    bool timeToReadDht;

    Ticker tickerState;
    bool wasMovement;
    bool doorState;
    bool timeToReadState;
};

App* CreateApp(MQTT* mqtt)
{
  return new EntranceApp(mqtt);
}

EntranceApp* EntranceApp::_this = 0;

#endif
