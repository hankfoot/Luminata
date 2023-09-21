// Luminata Project 2023, Leo, Hank, Jom

#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#define REQUEST_EXAMPLE 0

// include these only if you want to manually add requests
#if REQUEST_EXAMPLE
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#endif

#include "LEDManager.h"
#include "WebApp.h"

const char *ssid = "Luminator";
const char *password = "password";
  
LEDManager leds;

NeoPixelBus<NEOPIXEL_FEATURE, NEOPIXEL_METHOD> strip(NUM_LEDS, LED_PIN);
WebApp webApp;

void setup() {
  // hardware peripherals
  pinMode(LED_BUILTIN, OUTPUT);

#if REQUEST_EXAMPLE
  // must be added before webApp.begin()!
  manuallyAddRequestExample();
#endif

  // lambda callback for global brightness
  webApp.registerCallback("/brightness", "value", [](int value) {
    FastLED.setBrightness(value);
  });

  // lambda callback for pattern
  webApp.registerCallback("/pattern", "value", [](int value) {
    leds.changePattern(value);
  });

  // software peripherals
  // filesystem for web server (/data directory)
  SPIFFS.begin();
  WiFi.softAP(ssid, password);
  // web server
  webApp.begin();
  // debug via serial
  Serial.begin(115200);
  // LEDs
  strip.Begin();
  leds.begin(&strip);

  // boot up success, light up blue LED
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  leds.update();
  // FastLED.delay(10); // TODO: is this neccessary?
}

#if REQUEST_EXAMPLE
void manuallyAddRequestExample()
{
  AsyncWebServer* server = webApp.getServer();
  server->on("/slider", HTTP_GET, [](AsyncWebServerRequest *request) {
    String inputMessage;
    if (request->hasParam("value")) {
      inputMessage = request->getParam("value")->value();
      FastLED.setBrightness(inputMessage.toInt());
    } else {
      inputMessage = "No message sent";
    }
    request->send(200, "text/plain", "OK");
  });
}
#endif
