#ifndef LED_MANAGER_H
#define LED_MANAGER_H
#include <FastLED.h>
#include <NeoPixelBus.h>
#include <NPBColorLib.h>

#define LED_PIN 27
#define NUM_LEDS 24
#define BRIGHTNESS 64
#define LEDS_PER_LOOP NUM_LEDS/2
#define NEOPIXEL_FEATURE NeoRgbwFeature
#define NEOPIXEL_METHOD Neo800KbpsMethod


class LEDManager {
   
  /*
      VARIABLES
  */
  private:
  //FastLED
  CRGB leds[NUM_LEDS];
  CRGB innerLeds[NUM_LEDS/2];
  CRGB outerLeds[NUM_LEDS/2];

  //NeoPixelBus
  NeoPixelBus<NEOPIXEL_FEATURE, NEOPIXEL_METHOD>* strip;

  //Internal state
  int currentPattern = 0;

  /*
      METHODS
  */
  public:
  LEDManager() {}
  
  void begin(NeoPixelBus<NEOPIXEL_FEATURE, NEOPIXEL_METHOD>* stripRef) {
    strip = stripRef;
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    strip->Dirty();
  }

  void update() {
    
    switch (currentPattern)
    {
      case 0:
        RainbowSweep();
        break;
      case 1:
        WaveringColor();
        break;
      case 2:
        DualRotation();
        break;
      case 3:
        HeatmapPalette();
        break;
      default:
        // default to something nice
        RainbowSweep();
        break;
    }
    strip->Dirty();
    
    if(strip->IsDirty() == true){
      for (uint8_t i=0; i<NUM_LEDS; i++) {
        strip->SetPixelColor(i, CRGBToRgbwColor(leds[i]).Dim(BRIGHTNESS));
      }
    }
    strip->Show();
  }

  RgbwColor CRGBToRgbwColor(CRGB color) {

    static Colorspace colorspace(4000, 1, true, false);

    const float maxFastLedValue = 255.0;

    const float color_r_float = static_cast<float>(color.r);
    const float color_g_float = static_cast<float>(color.g);
    const float color_b_float = static_cast<float>(color.b);
    
    return colorspace.toRgbw(
        RgbFColor(
          color_r_float / maxFastLedValue,
          color_g_float / maxFastLedValue,
          color_b_float / maxFastLedValue)
      );
}

  void changePattern(int newPattern)
  {
    // TODO: handle tweening?
    currentPattern = newPattern;
  }
  void setBrightness(int ring, int brightness)
  {
    // TODO: implement this so that each ring can have its
    // brightness controlled individually.
    // the hs(v) value of each pattern should have some multiplier 
    // that is updated here.
  }
  
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

    strip->Dirty();
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
  uint8_t paletteIndex = 0;

  void HeatmapPalette(){
    //fill_palette_circular(leds, NUM_LEDS, paletteIndex, OceanColors_p, 50, LINEARBLEND);
    FastLED.show();
  }

/*
  public:
  void Update(uint32_t millis);

  private:
  void SetPixel(int loopID, int idx); 
  */
};

#endif
