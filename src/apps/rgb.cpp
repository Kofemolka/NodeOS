#ifdef APP_RGB

#include <functional>
#include <math.h>
#include <app.h>
#include <mqtt.h>
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
    Clr,
    Off
  } Mode;

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

public:
  RgbApp(MQTT* mqtt) :
   App(mqtt)
  {
      _this = this;
      _color = { 0, 0, 0 };
      _mode = Mode::Off;
  }

  void Init()
  {
    pinMode(R, OUTPUT);
    pinMode(G, OUTPUT);
    pinMode(B, OUTPUT);

    mqtt->Subscribe("rgb", [=](const String& d) { this->onRgbMsg(d); });
    mqtt->Subscribe("hsb", [=](const String& d) { this->onHsbMsg(d); });
    mqtt->Subscribe("mode", [=](const String& d) { this->onModeMsg(d); });
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

          _v -= 0.3;
          if( _v < 0)
          {
            _v = 100.0;
          }

          HSV hsv = rgb2hsv(_color);
          hsv.v = _v;
          setRGB(hsv2rgb(hsv));
      }
  }

  void Shutdown()
  {
      setRGB( {0,0,0} );
  }

private:
  void onRgbMsg(const String& d)
  {
    if(d.length() < 7)
      return;

      unsigned char r = strtol(d.substring(1,3).c_str(), 0, 16);
      unsigned char g = strtol(d.substring(3,5).c_str(), 0, 16);
      unsigned char b = strtol(d.substring(5,7).c_str(), 0, 16);

      _color = { r, g, b};

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

      _color = rgb;
      setRGB(rgb);
  }

  void onModeMsg(const String& d)
  {
      if(d == "clr")
      {
         _mode = Mode::Clr;
         runner.detach();
          setRGB(_color);
      }
      else if(d == "run")
      {
          _mode = Mode::Run;
          updateRunner = false;
          runner.detach();
          runner.attach_ms(30, RgbApp::runStep);
          _h = 0.0;
      }
      else if(d == "off")
      {
         _mode = Mode::Off;
         runner.detach();
          setRGB( {0,0,0});
      }
      else if(d == "pulse")
      {
           _mode = Mode::Pulse;
           updatePulser = false;
           runner.detach();
           runner.attach_ms(30, RgbApp::pulseStep);
           _v = 100.0;
      }
  }

  void setRGB(RGB rgb)
  {
    Serial.printf("RGB: %i %i %i\n", rgb.r, rgb.g, rgb.b);

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
    float Max;
	  float Min;
	  float Chroma;
	  HSV hsv;

	  Min = min(min(in.r, in.g), in.b);
	  Max = max(max(in.r, in.g), in.b);
	  Chroma = (Max - Min)/255;

  	//If Chroma is 0, then S is 0 by definition, and H is undefined but 0 by convention.
  	if(Chroma != 0)
  	{
  		if(in.r == Max)
  		{
  			hsv.h = (in.g - in.b) / Chroma / 255;

  			if(hsv.h < 0.0)
  			{
  				hsv.h += 6.0;
  			}
  		}
  		else if(in.g == Max)
  		{
  			hsv.h = ((in.b - in.r) / Chroma / 255) + 2.0;
  		}
  		else //in.b == Max
  		{
  			hsv.h = ((in.r - in.g) / Chroma / 255) + 4.0;
  		}

  		hsv.h *= 60.0;
  		hsv.s = 100*Chroma / Max / 255;
  	}

  	hsv.v = 100*Max/255;

    Serial.print("HSV: ");
    Serial.print(hsv.h);
    Serial.print(" ");
    Serial.print(hsv.s);
    Serial.print(" ");
    Serial.print(hsv.v);
    Serial.println("");

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

      return { (r+m)*255, (g+m)*255, (b+m)*255 };
  }

  RGB _color;
  Mode _mode;
  Ticker runner;
  float _h;
  bool updateRunner;
  float _v;
  bool updatePulser;
};

App* CreateApp(MQTT* mqtt)
{
  return new RgbApp(mqtt);
}

RgbApp* RgbApp::_this = 0;

#endif
