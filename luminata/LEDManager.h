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
    CRGBPalette16(CRGB::White),
    ForestColors_p,
    OceanColors_p,
    LavaColors_p,
    CloudColors_p,
    PartyColors_p
};

class LEDManager {
   
  /*
      VARIABLES
  */
  private:
  //FastLED Full Arrays
  CRGB source1[NUM_LEDS];
  CRGB source2[NUM_LEDS];
  CRGB output[NUM_LEDS];

  //FastLED Half Arrays
  CRGB source1InnerLeds[LEDS_PER_LOOP];
  CRGB source1OuterLeds[LEDS_PER_LOOP];
  CRGB source2InnerLeds[LEDS_PER_LOOP];
  CRGB source2OuterLeds[LEDS_PER_LOOP];

  //FastLED Palettes
  CRGBPalette16 palette;
  CRGBPalette16 targetPalette;

  //Blending
  uint8_t blendAmount = 0;
  uint8_t currentPattern;
  uint8_t source1Pattern;
  uint8_t source2Pattern;
  bool useSource1 = false;

  uint8_t numPalettes = 6;

  uint8_t brightness;
 
  TBlendType blendType;

  //NeoPixelBus
  NeoPixelBus<NEOPIXEL_FEATURE, NEOPIXEL_METHOD>* strip;

  //Internal state
  uint8_t numPatterns = 6; //First two are boring!
  uint8_t paletteIndex;
  bool wasInBaseState;

  /*
      METHODS
  */
  public:
  LEDManager() {}
  
  void begin(NeoPixelBus<NEOPIXEL_FEATURE, NEOPIXEL_METHOD>* stripRef) {
    Serial.println("Begin");
    strip = stripRef;
    palette = palettes[0];
    setPalette(0);
    changePattern(0);
    blendType = LINEARBLEND;
    brightness = BRIGHTNESS;
    fill_solid(output, NUM_LEDS, CRGB::Black);
    strip->Dirty();
  }

  void update() {

    UpdatePalettes();
    UpdatePatterns();
    AutoAdvance();

    if(strip->IsDirty() == true){
      for (uint8_t i=0; i<NUM_LEDS; i++) {
        strip->SetPixelColor(i, CRGBToRgbwColor(output[i]).Dim(brightness));
      }
    }
    strip->Show();
  }

  void changePattern(int newPattern)
  {
    if(newPattern >= numPatterns) return;

    wasInBaseState = (newPattern == 0);

    currentPattern = newPattern;

    if(useSource1){
      source1Pattern = currentPattern;
      fill_solid(source1, NUM_LEDS, CRGB::Black);
      fill_solid(source1InnerLeds, LEDS_PER_LOOP, CRGB::Black);
      fill_solid(source1OuterLeds, LEDS_PER_LOOP, CRGB::Black);
    }
    else{
      source2Pattern = currentPattern;
      fill_solid(source2, NUM_LEDS, CRGB::Black);
      fill_solid(source2InnerLeds, LEDS_PER_LOOP, CRGB::Black);
      fill_solid(source2OuterLeds, LEDS_PER_LOOP, CRGB::Black);
    }

    useSource1 = !useSource1;

    Serial.print("Change pattern to: ");
    Serial.println(currentPattern);
  }

  void setBrightness(int newBrightness)
  {
    brightness = newBrightness;
  }

  void setBlendType(int blendType){
    //TODO
  }

  void setPalette(int newPaletteIndex){
    if(paletteIndex > numPalettes) return;
    
    paletteIndex = newPaletteIndex;
    targetPalette = palettes[paletteIndex];
    Serial.print("Change palette to: ");
    Serial.println(paletteIndex);
  }

  private:

  void UpdatePalettes(){
    EVERY_N_MILLISECONDS(1){
      nblendPaletteTowardPalette(palette, targetPalette, 1);
    }
  }

  void AutoAdvance(){

    static uint8_t funPatternIndex = 2;

    //Patterns
    EVERY_N_SECONDS(15){

      if(wasInBaseState){
        funPatternIndex = funPatternIndex + 1;
        if(funPatternIndex >= numPatterns){
          funPatternIndex = 2;
        }
        changePattern(funPatternIndex);
      }
      else{
        changePattern(0);
      }
    }

    //Palettes
    EVERY_N_SECONDS(30){
      //Cycle through palettes linearly
      setPalette((paletteIndex + 1) % numPalettes);
    }
  }

  void UpdatePatterns(){ 
    
    EVERY_N_MILLISECONDS(10) {
      blend(source1, source2, output, NUM_LEDS, blendAmount);   // Blend between the two sources

      if (useSource1) {
        if (blendAmount < 255) blendAmount++;                   // blendAmount 'up' to source 2
      } else {
        if (blendAmount > 0) blendAmount--;                     // blendAmount 'down' to source 1
      }
    }

    RunPattern(source1Pattern, source1, source1InnerLeds, source1OuterLeds);
    RunPattern(source2Pattern, source2, source2InnerLeds, source2OuterLeds);

    strip->Dirty();
  }

  void RunPattern(uint8_t pattern, CRGB *leds, CRGB* innerLeds, CRGB* outerLeds){
    switch (pattern)
    {
      case 0:      
        CyclePaletteSwirl(leds);
        break;
      case 1:
        RainbowSwirl(leds);
        break;
      case 2:
        RevItUp(leds, innerLeds, outerLeds);
        break;
      case 3:
        Orbital(leds, innerLeds, outerLeds);
        break;
      case 4:
        EbbAndFlow(leds, innerLeds, outerLeds);
        break;
      case 5:
        DuelingDragons(leds, innerLeds, outerLeds);
        break;
      default:
        //default to something nice
        CyclePaletteSwirl(leds);
        break;
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


  void DualToSingle(CRGB *leds, CRGB* innerLeds, CRGB* outerLeds){
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
  void EbbAndFlow(CRGB *leds, CRGB* innerLeds, CRGB* outerLeds){

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

    DualToSingle(leds, innerLeds, outerLeds);
  }
  
  void RevItUp(CRGB *leds, CRGB* innerLeds, CRGB* outerLeds){

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
    

    DualToSingle(leds, innerLeds, outerLeds);
  }

  void CyclePaletteSwirl(CRGB *leds){

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

  void Orbital(CRGB *leds, CRGB* innerLeds, CRGB* outerLeds){
    EVERY_N_MILLISECONDS(100){
      outerLeds[random8(0, LEDS_PER_LOOP)] = ColorFromPalette(palette, random8(), 255, blendType);
    }

    EVERY_N_MILLISECONDS(3){
      fadeToBlackBy(outerLeds, LEDS_PER_LOOP,1);
    }

    DualToSingle(leds, innerLeds, outerLeds);
  }
  
  void RainbowSwirl(CRGB *leds) {
    static uint8_t paletteIndex = 0;
    
    fill_palette(leds, NUM_LEDS, paletteIndex, 3, RainbowColors_p, 255, blendType);

    EVERY_N_MILLISECONDS(5){
      paletteIndex++;
    }
  }

  void DuelingDragons(CRGB *leds, CRGB* innerLeds, CRGB* outerLeds){
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

    DualToSingle(leds, innerLeds, outerLeds);
  }
};

#endif
