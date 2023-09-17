#ifndef LED_MANAGER_H
#define LED_MANAGER_H

#include <FastLED.h>

#define LED_PIN 10
#define LEDS_PER_LOOP 8
#define NUM_LEDS LEDS_PER_LOOP * 2

class LEDManager {

  public:
  LEDManager() {}
  void begin() {
    FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(50);
    FastLED.setCorrection(UncorrectedColor);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
  }
  void update() {
    RainbowSweep();
  }

  private:
  CRGB leds[LEDS_PER_LOOP * 2];
  
  /*
      RAINBOW SWEEP
  */
  uint8_t _rainbowHue = 0;
  void RainbowSweep() {
    for(int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(_rainbowHue + (i * 10), 255, 255);
    }

    EVERY_N_MILLISECONDS(15){
      _rainbowHue++;
    }

    FastLED.show();
  }

  /*
      WAVERING COLOR
  */
  void WaveringColor(){
    EVERY_N_MILLISECONDS(100) {
      leds[0] = CHSV(160, random8(100,255), random8(100,255));

      for(int i = NUM_LEDS - 1; i > 0; i--){
        leds[i] = leds[i-1];
      }

      FastLED.show();
    }
  }

  /*
      DUAL ROTATION
  */
  int _innerRotIdx = 0;
  int _outerRotIdx = 0;
  void DualRotation(){

    fill_solid(leds, NUM_LEDS, CRGB::Black);
    int subdivisions = 1;

    //Inner loop
    EVERY_N_MILLISECONDS(100){
      _innerRotIdx = (_innerRotIdx + 1) % LEDS_PER_LOOP;
    }
    for(int i = 0; i < subdivisions; i++){
      int rotIdx = (_innerRotIdx +  (i * LEDS_PER_LOOP / subdivisions)) % LEDS_PER_LOOP;
      int ledIdx = rotIdx* 2 + 1;
      leds[ledIdx] = CRGB::Red;
    }
  
    //Outer loop
    EVERY_N_MILLISECONDS(1000){
      _outerRotIdx -= 1;
      if(_outerRotIdx < LEDS_PER_LOOP){
        _outerRotIdx += LEDS_PER_LOOP;
      }
    }
    subdivisions = 4;
    for(int i = 0; i < subdivisions; i++){
      int rotIdx = (_outerRotIdx +  (i * LEDS_PER_LOOP / subdivisions)) % LEDS_PER_LOOP;
      int ledIdx = rotIdx* 2;
      leds[ledIdx] = CRGB::Green;
    }

    FastLED.show();
  }

  /*
      TEST COLOR CORRECTION
  */
  void TestColorCorrection(){
    fill_solid(leds, NUM_LEDS, CRGB::White);

    FastLED.setCorrection(UncorrectedColor);
    leds[0] = CRGB::Red;
    FastLED.show();
    delay(3000);

    FastLED.setCorrection(TypicalLEDStrip);
    leds[0] = CRGB::Green;
    FastLED.show();
    delay(3000);

    FastLED.setCorrection(TypicalPixelString);
    leds[0] = CRGB::Blue;
    FastLED.show();
    delay(3000);
  }
/*
  public:
  void Update(uint32_t millis);

  private:
  void SetPixel(int loopID, int idx); 
  */
};

#endif
