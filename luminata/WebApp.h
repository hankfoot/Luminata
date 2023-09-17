#ifndef WEB_APP_H
#define WEB_APP_H

#include <Arduino.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include <FastLED.h>

class WebApp
{
  public:
  WebApp();
  void begin();
  AsyncWebServer* getServer();
  void registerCallback(const char* uri, const char* parameter, void (*callback)(int));
  void registerCallback(const char* uri, const char* parameter1, const char* parameter2, void (*callback)(int, int));
};

#endif
