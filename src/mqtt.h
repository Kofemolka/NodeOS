#ifndef MQTT_H
#define MQTT_H

#include <functional>
#include <map>
#include <config.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

class MQTT
{
public:
  typedef std::function<void(const String&)> MsgCallback;

  MQTT() : client(espClient)
  {

  }

  void Init()
  {
    client.setServer(mqtt_server, 1883);
    //std::function<void(char*, uint8_t*, unsigned int)> fn = std::bind(&MQTT::callback, this, std::placeholders::_1);
    client.setCallback([=](char* topic, byte* payload, unsigned int length)
    {
      return this->callback(topic, payload, length);
    });
  }

  void Loop()
  {
    if (!client.connected()) {
        reconnect();
      }
      client.loop();
  }

  void Subscribe(const char* topic, MsgCallback callback)
  {
    _callbacks.insert(std::pair<String,MsgCallback>(topic,callback));
  }

  void Publish(const char* topic, const char* data)
  {
    client.publish(topic, data);
  }

protected:
  void  callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    String d;
    for (int i = 0; i < length; i++) {
      d += (char)payload[i];
    }
    Serial.println(d);

    std::pair <std::multimap<String,MsgCallback>::iterator, std::multimap<String,MsgCallback>::iterator> ret;
    ret = _callbacks.equal_range(topic);

    for (std::multimap<String,MsgCallback>::iterator it=ret.first; it!=ret.second; ++it)
      {
        it->second(d);
      }
  }

  void reconnect() {
    // Loop until we're reconnected
    if (!client.connected()) {
      Serial.print("Attempting MQTT connection...");
      // Create a random client ID
      String clientId = "ESP8266Client-";
      clientId += String(random(0xffff), HEX);
      // Attempt to connect
      if (client.connect(clientId.c_str(),mqtt_user, mqtt_pwd)) {
        Serial.println("connected");
        // Once connected, publish an announcement...

        for (auto it=_callbacks.begin(); it!=_callbacks.end(); ++it)
        {
          client.subscribe((*it).first.c_str());
        }
        // ... and resubscribe

      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
      }
    }
  }

private:
    WiFiClient espClient;
    PubSubClient client;

    std::multimap<String,MsgCallback> _callbacks;
};

#endif