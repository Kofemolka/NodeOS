#ifdef APP_TEMPHUM

#include <Adafruit_Sensor.h>
#include <DHT.h>

#include <app.h>
#include <mqtt.h>

class TempHumApp : public App
{
public:
  TempHumApp(MQTT* mqtt) : App(mqtt)
  {

  }

  void Init()
  {
    dht.begin();
  }

  void Loop()
  {
    float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    //return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");

  //client.publish((DEVID + "/temp").c_str(), String(t).c_str());
  //client.publish((DEVID + "/hum").c_str(), String(h).c_str());
  }

  void Shutdown()
  {

  }
protected:
  DHT dht(D1, DHT22);
};

App* CreateApp(MQTT* mqtt)
{
  return new TempHumApp(mqtt);
}


#endif
