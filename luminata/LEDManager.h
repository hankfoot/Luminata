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

DEFINE_GRADIENT_PALETTE(alternateBW_gp){
  0,0,0,0, //black
  64,255,255,255, //white
  128,0,0,0, //black
  192,255,255,255 //white
};

CRGBPalette16 palettes[] = {
    PartyColors_p,
    ForestColors_p,
    OceanColors_p,
    LavaColors_p,
    RainbowColors_p
};

class LEDManager {
   
  /*
      VARIABLES
  */
  private:
  //FastLED
  CRGB leds[NUM_LEDS];
  CRGB innerLeds[LEDS_PER_LOOP];
  CRGB outerLeds[LEDS_PER_LOOP];
  CRGB halfMask[LEDS_PER_LOOP];
  CRGBPalette16 palette;
  CRGBPalette16 targetPalette;

  uint8_t numPalettes = 5;
 
  TBlendType blendType;

  //NeoPixelBus
  NeoPixelBus<NEOPIXEL_FEATURE, NEOPIXEL_METHOD>* strip;

  //Internal state
  int currentPattern = 3;
  uint8_t paletteIndex = 0;

  /*
      METHODS
  */
  public:
  LEDManager() {}
  
  void begin(NeoPixelBus<NEOPIXEL_FEATURE, NEOPIXEL_METHOD>* stripRef) {
    strip = stripRef;
    palette = palettes[0];
    targetPalette = palettes[0];
    blendType = LINEARBLEND;
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    strip->Dirty();
  }

  void update() {

    UpdatePalettes();
    
    switch (currentPattern)
    {
      case 0:
        RainbowSweep();
        break;
      case 1:
        WaveringColor();
        break;
      case 2:
        InnerYinYang();
        break;
      case 3:
        Orbital();
        break;
      case 4:
        CyclePaletteSwirl();
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

  void UpdatePalettes(){

    nblendPaletteTowardPalette(palette, targetPalette, 1);

    EVERY_N_SECONDS(20){
      paletteIndex = (paletteIndex + 1) % numPalettes;

      targetPalette = palettes[paletteIndex];
    }
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

  void DualToSingle(){
    //Outer
    for(int i = 0; i < LEDS_PER_LOOP; i++){
      leds[i*2] = outerLeds[i];
    }

    //Inner
    for(int i = 0; i < LEDS_PER_LOOP; i++){
      leds[i*2 + 1] = innerLeds[i];
    }
  }

    /*
      PATTERNS
    */

  void InnerYinYang(){
    CRGBPalette16 mask = alternateBW_gp;

    static uint8_t maskStartIndex = 0;
    EVERY_N_MILLISECONDS(10){
      maskStartIndex++;
    }
    fill_palette_circular(halfMask, LEDS_PER_LOOP, maskStartIndex, mask, 255, LINEARBLEND);

    static uint8_t paletteStartIndex = 0;
    EVERY_N_MILLISECONDS(2){
      //paletteStartIndex++;
    }
    fill_solid(innerLeds, LEDS_PER_LOOP, CRGB::Red);
    //fill_palette_circular(innerLeds, LEDS_PER_LOOP, paletteStartIndex, palette, 255, blendType);
    
    for(int i = 0; i < LEDS_PER_LOOP; i++){
     innerLeds[i] -= halfMask[i];
    }
    

    DualToSingle();
  }

  void CyclePaletteSwirl(){

    static uint8_t startIndex = 0;

    EVERY_N_MILLISECONDS(65){
      startIndex++;
    }

    uint8_t colorIndex = startIndex;

    for(int i = 0; i < NUM_LEDS; i++){
      leds[i] = ColorFromPalette(palette, colorIndex, 255, blendType);
      colorIndex += 1;
    }
  }

  void Orbital(){
    EVERY_N_MILLISECONDS(100){
      outerLeds[random8(0, LEDS_PER_LOOP)] = ColorFromPalette(palette, random8(), 255, blendType);
    }

    EVERY_N_MILLISECONDS(3){
      fadeToBlackBy(outerLeds, LEDS_PER_LOOP,1);
    }

    DualToSingle();
  }
  
  /*
      RAINBOW SWEEP
  */
  uint8_t _rainbowHue = 0;
  void RainbowSweep() {
    for(int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(_rainbowHue + (i * 10), 255, 255);
    }

    EVERY_N_MILLISECONDS(5){
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
