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
  CRGBPalette16 palette;
  CRGBPalette16 targetPalette;

  uint8_t numPalettes = 5;
 
  TBlendType blendType;

  //NeoPixelBus
  NeoPixelBus<NEOPIXEL_FEATURE, NEOPIXEL_METHOD>* strip;

  //Internal state
  int currentPattern = 5;
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
        RainbowSwirl();
        break;
      case 1:
        DuelingDragons();
        break;
      case 2:
        RevItUp();
        break;
      case 3:
        Orbital();
        break;
      case 4:
        CyclePaletteSwirl();
        break;
      case 5:
        EbbAndFlow();
        break;
      default:
        // default to something nice
        CyclePaletteSwirl();
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

  void changePattern(int newPattern)
  {
    // TODO: handle tweening?
    currentPattern = newPattern;
  }
  void setBrightness(int brightness)
  {
    // TODO: implement this so that each ring can have its
    // brightness controlled individually.
    // the hs(v) value of each pattern should have some multiplier 
    // that is updated here.

    //TODO
  }

  void setBlendType(int blendType){

  }

  void setPalette(int paletteIndex){
    //TODO
  }

  private:

  void UpdatePalettes(){

    EVERY_N_MILLISECONDS(2){
      nblendPaletteTowardPalette(palette, targetPalette, 1);
    }

    EVERY_N_SECONDS(30){
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
  void EbbAndFlow(){

    static uint8_t paletteIndex = 0;

    uint8_t brightBeat1 = beatsin8(8, 0, 255, 0, 0);
    uint8_t brightBeat2 = beatsin8(8, 0, 255, 0, 128);

    int threshold = 96;
    brightBeat1 = (brightBeat1 < threshold) ? 0 : map(brightBeat1, threshold, 255, 0, 255);
    brightBeat2 = (brightBeat2 < threshold) ? 0 : map(brightBeat2, threshold, 255, 0, 255);

    fill_palette(innerLeds, LEDS_PER_LOOP, paletteIndex, 0, palette, brightBeat1, blendType);
    fill_palette(outerLeds, LEDS_PER_LOOP, paletteIndex + 128, 0, palette, brightBeat2, blendType);
    
    EVERY_N_MILLISECONDS(150){
      paletteIndex++;
    }

    DualToSingle();
  }
  
  void RevItUp(){

    int loopLeds = int(LEDS_PER_LOOP);
    uint8_t posBeat = beatsin8(7, 0, loopLeds * 6, 0, 0) % loopLeds;


    uint8_t posBeat2 = (posBeat + (loopLeds)/2) % loopLeds;
    uint8_t brightBeat = beatsin8(14, 0, 255, 0, 64);

    static uint8_t paletteIndex = 0;
    innerLeds[posBeat] = ColorFromPalette(palette, paletteIndex, brightBeat, blendType);

    EVERY_N_MILLISECONDS(100){
      paletteIndex++;
    }

    EVERY_N_MILLISECONDS(10){
      blur1d(outerLeds, LEDS_PER_LOOP, 50);
      fadeToBlackBy(innerLeds, LEDS_PER_LOOP, 1);
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
  
  void RainbowSwirl() {
    targetPalette = RainbowColors_p;

    static uint8_t paletteIndex = 0;
    
    fill_palette(leds, NUM_LEDS, paletteIndex, 3, palette, 255, blendType);

    EVERY_N_MILLISECONDS(5){
      paletteIndex++;
    }
  }

  void DuelingDragons(){
    static int innerIndex = 0;
    static int outerIndex = 0;

    static uint8_t paletteIndex = 0;
    int subdivisions = 1;
    
    //Inner
    EVERY_N_MILLISECONDS(100){
      innerIndex -= 1;
      if(innerIndex < 0) innerIndex = (LEDS_PER_LOOP / subdivisions) - 1;
    }

    //Outer
    EVERY_N_MILLISECONDS(135){
      outerIndex = (outerIndex + 1) % (LEDS_PER_LOOP / subdivisions);
    }
    
    for(int i = innerIndex; i < LEDS_PER_LOOP; i += LEDS_PER_LOOP/subdivisions){
      innerLeds[i] = ColorFromPalette(palette, paletteIndex + 32, 255, blendType);
    }
    for(int i = outerIndex; i < LEDS_PER_LOOP; i += LEDS_PER_LOOP/subdivisions){
        outerLeds[i] = ColorFromPalette(palette, paletteIndex, 255, blendType);
    }

    EVERY_N_MILLISECONDS(2){
      fadeToBlackBy(innerLeds, LEDS_PER_LOOP, 1);
      fadeToBlackBy(outerLeds, LEDS_PER_LOOP, 1);
    }

    EVERY_N_MILLISECONDS(100){
      paletteIndex++;
    }

    DualToSingle();

    
  }
};

#endif
