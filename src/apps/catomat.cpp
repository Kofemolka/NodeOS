#ifdef APP_CATOMAT

#include <app.h>
#include <mqtt.h>

class CatomatApp : public App
{
public:
  CatomatApp(MQTT* mqtt) :
   App(mqtt)
  {

  }

  void Init()
  {
      mqtt->Subscribe("inbox/mode", [=](const String& d) { this->onMsg("inbox/mode",d); });
      mqtt->Subscribe("inbox/cmd", [=](const String& d) { this->onMsg("inbox/cmd",d); });
      mqtt->Subscribe("inbox/adj/food", [=](const String& d) { this->onMsg("inbox/adj/food",d); });
      mqtt->Subscribe("inbox/adj/water", [=](const String& d) { this->onMsg("inbox/adj/water",d); });
  }

  void Loop()
  {
			if(Serial.available() > 0)
      {
					String str = Serial.readString();
		      int ndx = str.indexOf(":");
		      if (ndx != -1)
		      {
		        String topic = str.substring(0, ndx);
		        String value = str.substring(ndx + 1);

		        Serial.print(topic);
		        Serial.print(":");
		        Serial.println(value);

		        mqtt->Publish(topic, value, true);
		      }
			}
  }

  void Shutdown()
  {

  }
private:
  void onMsg(const String& t,const String& d)
  {
      Serial.println(t + ":" + d);
      Serial.println();
  }
};

App* CreateApp(MQTT* mqtt)
{
  return new CatomatApp(mqtt);
}

#endif
