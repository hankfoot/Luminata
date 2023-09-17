#include "WebApp.h"

AsyncWebServer server(80);

WebApp::WebApp() 
{
}

void WebApp::begin() 
{
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404);
  });

  server.begin();
}

AsyncWebServer* WebApp::getServer()
{
  return &server;
}

void WebApp::registerCallback(const char *uri, const char *parameter, void (*callback)(int))
{
  server.on(uri, HTTP_GET, [parameter, callback](AsyncWebServerRequest *request) {
    String inputMessage;
    if (request->hasParam(parameter)) {
      inputMessage = request->getParam(parameter)->value();
      callback(inputMessage.toInt());
    }
    request->send(200, "text/plain", "OK");
  });
}

// TODO: okay, this is kinda embarrassing
void WebApp::registerCallback(const char *uri, const char *parameter1, const char *parameter2, void (*callback)(int, int))
{
  server.on(uri, HTTP_GET, [parameter1, parameter2, callback](AsyncWebServerRequest *request) {
    String parameter1Message;
    String parameter2Message;
    if (request->hasParam(parameter1) && request->hasParam(parameter2)) {
      parameter1Message = request->getParam(parameter1)->value();
      parameter2Message = request->getParam(parameter2)->value();
      callback(parameter1Message.toInt(), parameter2Message.toInt());
    }
    request->send(200, "text/plain", "OK");
  });
}
