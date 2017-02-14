#ifdef APP_CATOMAT

#include <app.h>
#include <mqtt.h>
#include <Wire.h>
#include <Ticker.h>

class CatomatApp : public App
{
    static CatomatApp* _this;

    static const unsigned char SLAVE_ID = 9;
    static const unsigned char C_MODE = 1;
    static const unsigned char C_CMD = 2;
    static const unsigned char C_ADJFOOD = 3;
    static const unsigned char C_ADJWATER = 4;
public:
    CatomatApp(MQTT* mqtt) :
     App(mqtt)
    {
        _this = this;
    }

    void Init()
    {
        mqtt->Subscribe("mode", [=](const String& d) { this->onMode(d); });
        mqtt->Subscribe("cmd", [=](const String& d) { this->onCmd(d); });
        mqtt->Subscribe("adj/food", [=](const String& d) { this->onAdjFood(d); });
        mqtt->Subscribe("adj/water", [=](const String& d) { this->onAdjWater(d); });

        timeToRead = false;
        ticker.attach(2, CatomatApp::markTimeToRead);

        Wire.begin(D1, D2);
        Wire.setClock(100000);
        Wire.setClockStretchLimit(1500);
    }

    void Loop()
    {
        if(timeToRead)
        {
            timeToRead = false;

            Wire.requestFrom(SLAVE_ID, (byte)6);
            //Serial.println("timeToRead");
            byte data[6] = { 0 };
            if(Wire.available())
            {
                //Serial.println("Reading...");
                int i = 0;
                while (Wire.available()) {
                    byte b = Wire.read();
                    if(i<6)
                    {
                        data[i++] = b;
                    }
                }

                processMsg(data);
            }
        }
    }

    void Shutdown()
    {

    }
private:
    static void markTimeToRead()
    {
      _this->timeToRead = true;
    }

    void onMode(const String& p)
    {
        byte d = p.toInt();
        if(d < 1 || d > 2)
        {
            return;
        }

        send(C_MODE, d);
    }

    void onCmd(const String& p)
    {
        byte d = p.toInt();
        if(d < 1 || d > 2)
        {
            return;
        }

        send(C_CMD, d);
    }

    void onAdjFood(const String& p)
    {
        byte d = p.toInt();

        send(C_ADJFOOD, d);
    }

    void onAdjWater(const String& p)
    {
        byte d = p.toInt();

        send(C_ADJWATER, d);
    }

    void send(byte code, byte data)
    {
        Wire.beginTransmission(SLAVE_ID);
        for(int i=0;i<3;i++)
        {
            Wire.write(code);
        }
        for(int i=0;i<3;i++)
        {
            Wire.write(data);
        }
        Serial.printf("End transmit: %d\n", Wire.endTransmission());
    }

    void processMsg(byte data[6])
    {
        //Serial.printf("Read: %02X%02X%02X:%02X%02X%02X\n", data[0],data[1],data[2],data[3],data[4],data[5]);
        if(data[0] == 0)
        {
            return; //nothing
        }

        if(data[0] != data[1] || data[0] != data[2] ||
           data[3] != data[4] || data[3] != data[5])
         {
            Serial.printf("Invalid message: %02X%02X%02X:%02X%02X%02X\n", data[0],data[1],data[2],data[3],data[4],data[5]);
            return;
         }

        Serial.printf("Valid msg: %d:%d\n", data[0], data[3]);
        switch (data[0]) {
          case 1: //mode
            mqtt->Publish("status/mode", String(data[3]), true);
            break;
          case 2: //visit
            mqtt->Publish("status/visit", String(data[3]), true);
            break;
          case 3: //ack
            mqtt->Publish("status/ack", String(data[3]));
            break;
          case 4: //ack
            mqtt->Publish("status/food", String(data[3]));
            break;
          case 5: //ack
            mqtt->Publish("status/water", String(data[3]));
            break;
          default:
            Serial.printf("Unknown code: %d\n", data[0]);
            return;
        }
    }

private:
    Ticker ticker;
    volatile bool timeToRead;
};

App* CreateApp(MQTT* mqtt)
{
  return new CatomatApp(mqtt);
}

CatomatApp* CatomatApp::_this = 0;

#endif
