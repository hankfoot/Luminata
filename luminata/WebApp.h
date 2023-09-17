#ifndef WEB_APP_H
#define WEB_APP_H

#include <Arduino.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>

class WebApp
{
  public:
  WebApp();
  void begin();
};

#endif
