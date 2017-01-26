#ifdef APP_RGB

#include <functional>
#include <math.h>
#include <app.h>
#include <mqtt.h>
#include <persist.h>
#include <Ticker.h>

#define R D1
#define G D2
#define B D6

class RgbApp : public App
{
  static RgbApp* _this;

  typedef enum {
    Run,
    Pulse,
    Clr
  } Mode;

  typedef enum {
    On,
    Off
  } State;

  typedef struct {
      unsigned char r;
      unsigned char g;
      unsigned char b;
  } RGB;

  typedef struct {
      float h;       // angle in degrees
      float s;       // percent
      float v;       // percent
  } HSV;

  const char* K_MODE = "RgbApp.Mode";
  const char* K_STATE = "RgbApp.State";
  const char* K_COLOR = "RgbApp.Color";

public:
  RgbApp(MQTT* mqtt) :
   App(mqtt)
  {
      _this = this;
      _pulserDelta = 1;

      _color = { 0, 0, 0 };
      _color = Persist::Inst().Get(K_COLOR, _color);

      _state = State::Off;
      _state = Persist::Inst().Get(K_STATE, _state);

      _mode = Mode::Clr;
      _mode = Persist::Inst().Get(K_MODE, _mode);

      setState(_state);
  }

  void Init()
  {
    pinMode(R, OUTPUT);
    pinMode(G, OUTPUT);
    pinMode(B, OUTPUT);

    mqtt->Subscribe("rgb", [=](const String& d) { this->onRgbMsg(d); });
    mqtt->Subscribe("hsb", [=](const String& d) { this->onHsbMsg(d); });
    mqtt->Subscribe("mode", [=](const String& d) { this->onModeMsg(d); });
    mqtt->Subscribe("state", [=](const String& d) { this->onStateMsg(d); });
  }

  void Loop()
  {
      if(updateRunner)
      {
        updateRunner = false;

        _h += 0.5;
        if(_h > 359.0)
        {
          _h = 0.0;
        }

         HSV hsv = { _h, 100, 100 };
         setRGB(hsv2rgb(hsv));
      }
      else if(updatePulser)
      {
          updatePulser = false;

          _pulserClr.v -= _pulserDelta;
          if( _pulserClr.v < 50.0)
          {
              _pulserDelta = -_pulserDelta;
          }
          if( _pulserClr.v > 100.0)
          {
              _pulserDelta = -_pulserDelta;
              _pulserClr.v = 100.0;
          }

          setRGB(hsv2rgb(_pulserClr));
      }
  }

  void Shutdown()
  {
      setRGB( {0,0,0} );
  }

private:
  void onStateMsg(const String& d)
  {
      if(d == "on")
      {
          setState(State::On);
      }
      else if(d == "off")
      {
          setState(State::Off);
      }
  }

  void setState(State state)
  {
     _state = state;
     Persist::Inst().Put(K_STATE, _state);

     if(state == State::On)
     {
       setMode(_mode);
     }
     else if(state == Off)
     {
       runner.detach();
       setRGB( {0,0,0} );
     }
  }

  void onRgbMsg(const String& d)
  {
    if(d.length() < 7)
      return;

      unsigned char r = strtol(d.substring(1,3).c_str(), 0, 16);
      unsigned char g = strtol(d.substring(3,5).c_str(), 0, 16);
      unsigned char b = strtol(d.substring(5,7).c_str(), 0, 16);

      setColor({ r, g, b});
      _pulserClr = rgb2hsv(_color);

      setRGB(_color);
  }

  void onHsbMsg(const String& d)
  {
      //123.0,100.0,100.0
      char* pEnd;
      HSV hsv;

      int c1 = d.indexOf(",");
      int c2 = d.indexOf(",", c1+1);

      if(c1 == -1 || c2 == -1)
        return;

      hsv.h = d.substring(0, c1).toFloat();
      hsv.s = d.substring(c1+1, c2).toFloat();
      hsv.v = d.substring(c2+1).toFloat();

      RGB rgb = hsv2rgb(hsv);

      setColor(rgb);
      _pulserClr = hsv;
      setRGB(rgb);
  }

  void onModeMsg(const String& d)
  {
      if(d == "clr")
      {
         setMode(Mode::Clr);
      }
      else if(d == "run")
      {
          setMode(Mode::Run);
      }
      else if(d == "pulse")
      {
          setMode(Mode::Pulse);
      }
  }

  void setColor(RGB rgb)
  {
      _color = rgb;
      Persist::Inst().Put(K_COLOR, _color);
  }

  void setMode(Mode mode)
  {
      _mode = mode;
      Persist::Inst().Put(K_MODE, _mode);

      if(_state == State::Off)
          return;

      switch(_mode)
      {
        case Mode::Clr:
          runner.detach();
          setRGB(_color);
          break;

        case Mode::Run:
          updateRunner = false;
          runner.detach();
          runner.attach_ms(50, RgbApp::runStep);
          _h = 0.0;
          break;

        case Mode::Pulse:
           updatePulser = false;
           runner.detach();
           runner.attach_ms(30, RgbApp::pulseStep);
           _pulserClr = rgb2hsv(_color);
           break;
      }
  }

  void setRGB(RGB rgb)
  {
      //Serial.printf("RGB: %i %i %i\n", rgb.r, rgb.g, rgb.b);

      analogWrite(R, rgb.r*4);
      analogWrite(G, rgb.g*4);
      analogWrite(B, rgb.b*4);
  }

  static void runStep()
  {
      _this->updateRunner = true;
  }

  static void pulseStep()
  {
      _this->updatePulser = true;
  }

  HSV rgb2hsv(RGB in)
  {
      float rf = ((float)in.r)/255;
      float gf = ((float)in.g)/255;
      float bf = ((float)in.b)/255;

      float max = fmax(rf, fmax(gf, bf));
      float min = fmin(rf, fmin(gf, bf));

      float d = max - min;

      HSV hsv;
      if( d == 0.0 )
      {
          hsv.h = 0;
      }
      else if(max == rf)
      {
          if(gf > bf)
          {
              hsv.h = (gf - bf) / d;
          }
          else
          {
              hsv.h = (gf - bf) / d + 6.0;
          }
      }
      else if(max == gf)
      {
          hsv.h = (bf - rf) / d + 2.0;
      }
      else
      {
          hsv.h = (rf - gf) / d + 4.0;
      }
      hsv.h *= 60;

      if(max == 0)
      {
          hsv.s = 0;
      }
      else
      {
          hsv.s = 100 * (1 - min/max);
      }
      hsv.v = max * 100;

      /*Serial.print("HSV: ");
      Serial.print(hsv.h);
      Serial.print(" ");
      Serial.print(hsv.s);
      Serial.print(" ");
      Serial.print(hsv.v);
      Serial.println("");*/

    	return hsv;
  }

  RGB hsv2rgb(HSV in)
  {
      //h: 0-359
      //s: 0.0 - 100.0
      //v: 0.0 - 100.0
      float h = in.h > 359 ? in.h-359 : in.h;
      float s = in.s / 100.0;
      float v = in.v / 100.0;

      float r,g,b;

      float c = v * s;
      int hh = floor(h/60);
      double fractpart, intpart;
      fractpart = modf (h/60 , &intpart);
      float x = c * (1 - abs(hh%2 - 1 + fractpart));

      switch(hh) {
        case 0: r = c; g = x; b = 0;           break;
        case 1: r = x; g = c; b = 0;           break;
        case 2: r = 0; g = c; b = x;           break;
        case 3: r = 0; g = x; b = c;           break;
        case 4: r = x; g = 0; b = c;           break;
        case 5:
        default: r = c; g = 0; b = x;          break;
      }

      float m = v - c;

      return { (unsigned char)((r+m)*255),
               (unsigned char)((g+m)*255),
               (unsigned char)((b+m)*255) };
  }

  RGB _color;
  Mode _mode;
  State _state;
  Ticker runner;
  float _h;
  bool updateRunner;

  HSV _pulserClr;
  float _pulserDelta;
  bool updatePulser;
};

App* CreateApp(MQTT* mqtt)
{
    return new RgbApp(mqtt);
}

RgbApp* RgbApp::_this = 0;

#endif
