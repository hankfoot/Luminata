// Luminata Project 2023, Leo, Hank, Jom

#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#include "LEDManager.h"
#include "WebApp.h"

const char *ssid = "Luminator";
const char *password = "password";
  
LEDManager leds;
WebApp webApp;

void setup() {
  // hardware peripherals
  pinMode(LED_BUILTIN, OUTPUT);

  // software peripherals
  // filesystem for web server (/data directory)
  SPIFFS.begin();
  WiFi.softAP(ssid, password);
  // web server
  webApp.begin();
  // debug via serial
  Serial.begin(115200);
  // LEDs
  leds.begin();

  // boot up sucess, light up blue LED
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  leds.update();
  // FastLED.delay(10); // TODO: is this neccessary?
}
