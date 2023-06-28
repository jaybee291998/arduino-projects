#include <FastLED.h>

// How many leds in your strip?
#define NUM_LEDS 64
#define DATA_PIN 27
#define BRIGHTNESS 50

// Define the array of leds
CRGB leds[NUM_LEDS];

void setup() { 
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
    FastLED.setBrightness(BRIGHTNESS);
//    Serial.begin(115200);
}

void loop() { 
  // Turn the LED on, then pause
  for(int i  = 0; i < NUM_LEDS; i ++){
    leds[i] = 0x7fff00;
    FastLED.show();
    delay(1);
    leds[i] = CRGB::Black;
    FastLED.show();
    delay(1);
  }
//  leds[0] = CRGB::Red;
//  FastLED.show();
//  delay(500);
//  // Now turn the LED off, then pause
//  leds[0] = CRGB::Black;
//  FastLED.show();
//  delay(500);
}
