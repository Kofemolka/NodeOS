#ifdef APP_IRSCAN

#include <app.h>
#include <mqtt.h>
#include <IRremoteESP8266.h>

#define IRPin D5

IRrecv irrecv(IRPin);

class IRScanApp : public App
{
public:
  IRScanApp(MQTT* mqtt) : App(mqtt)
  {

  }

  void Init()
  {
     irrecv.enableIRIn();
  }

  void Loop()
  {
    decode_results results;
    if (irrecv.decode(&results)) {
      Serial.println(results.value, HEX);
      dump(&results); 
      irrecv.resume(); // Receive the next value
    }

    delay(100);
  }

  void Shutdown()
  {

  }
protected:
  void dump(decode_results *results) {
    // Dumps out the decode_results structure.
    // Call this after IRrecv::decode()
    int count = results->rawlen;
    if (results->decode_type == UNKNOWN) {
      Serial.print("Unknown encoding: ");
    }
    else if (results->decode_type == NEC) {
      Serial.print("Decoded NEC: ");

    }
    else if (results->decode_type == SONY) {
      Serial.print("Decoded SONY: ");
    }
    else if (results->decode_type == RC5) {
      Serial.print("Decoded RC5: ");
    }
    else if (results->decode_type == RC6) {
      Serial.print("Decoded RC6: ");
    }
    else if (results->decode_type == PANASONIC) {
      Serial.print("Decoded PANASONIC - Address: ");
      Serial.print(results->panasonicAddress, HEX);
      Serial.print(" Value: ");
    }
    else if (results->decode_type == LG) {
      Serial.print("Decoded LG: ");
    }
    else if (results->decode_type == JVC) {
      Serial.print("Decoded JVC: ");
    }
    else if (results->decode_type == AIWA_RC_T501) {
      Serial.print("Decoded AIWA RC T501: ");
    }
    else if (results->decode_type == WHYNTER) {
      Serial.print("Decoded Whynter: ");
    }
    Serial.print(results->value, HEX);
    Serial.print(" (");
    Serial.print(results->bits, DEC);
    Serial.println(" bits)");
    Serial.print("Raw (");
    Serial.print(count, DEC);
    Serial.print("): ");

    for (int i = 1; i < count; i++) {
      if (i & 1) {
        Serial.print(results->rawbuf[i]*USECPERTICK, DEC);
      }
      else {
        Serial.write('-');
        Serial.print((unsigned long) results->rawbuf[i]*USECPERTICK, DEC);
      }
      Serial.print(" ");
    }
    Serial.println();
  }
private:

};

App* CreateApp(MQTT* mqtt)
{
  return new IRScanApp(mqtt);
}

#endif
