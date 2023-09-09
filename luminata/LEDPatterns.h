#ifndef LED_PATTERNS_H
#define LED_PATTERNS_H

#include <FastLED.h>

#define LED_PIN 27
#define LEDS_PER_LOOP 8

class LEDPattern {

  public:
  void Update(uint32_t millis);

  private:
  void SetPixel(int loopID, int idx);  
};

#endif