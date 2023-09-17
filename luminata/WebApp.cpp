#include "WebApp.h"

AsyncWebServer server(80);

WebApp::WebApp()
{

}

void WebApp::begin()
{
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.onNotFound([](AsyncWebServerRequest *request)
  {
    request->send(404);
  });

  server.begin();
}
