#include <FastLED.h>

#define NUM_LEDS 64
#define LED_PIN 27

#define WIDTH 8
#define HEIGHT 8

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
}

void loop() {
//  fill_gradient_RGB(leds, NUM_LEDS, CRGB::Magenta, CRGB::Yellow);
//  FastLED.show();
//  delay(500);
//
//  fill_gradient_RGB(leds, NUM_LEDS, CRGB::Red, CRGB::Yellow, CRGB::Green, CRGB::Blue);
//  FastLED.show();
//  delay(500);
//
//  fill_rainbow(leds, NUM_LEDS, 0, 255 / NUM_LEDS);
//  FastLED.show();
//  delay(500);
   glitter_2(10);
   
//   glitter_3(10);
//  for(int i = 0; i < NUM_LEDS; i++){
//    leds[i] = CRGB::Red;
//    FastLED.show();
//    delay(1);
//    leds[i] = CRGB::Black;
//  }
//
//
//  for(int i = NUM_LEDS - 1; i >= 0; i--){
//    leds[i] = CRGB::Red;
//    FastLED.show();
//    delay(1);
//    leds[i] = CRGB::Black;
//  }
}

void glitter_2(int del){
  uint8_t x = random(0, WIDTH);
  uint8_t y = random(0, HEIGHT);
  uint8_t x_1 = random(0, WIDTH);
  uint8_t y_1 = random(0, HEIGHT);
  leds[x + y * WIDTH] = random(0, 0xffffff);
  leds[x_1 + y_1 * WIDTH] = random(0, 0xffffff);
  FastLED.show();
  delay(del);
  leds[x + y * WIDTH] = CRGB::Black;
  leds[x_1 + y_1 * WIDTH] = CRGB::Black;
}


void glitter_3(int del){
  uint8_t x = random(0, WIDTH);
  uint8_t y = random(0, HEIGHT);
  uint8_t x_1 = random(0, WIDTH);
  uint8_t y_1 = random(0, HEIGHT);
  leds[x + y * WIDTH] = random(0, 0xffffff);
  leds[x_1 + y_1 * WIDTH] = random(0, 0xffffff);
  FastLED.show();
  delay(del);
  uint8_t x_2 = random(0, WIDTH);
  uint8_t y_2 = random(0, HEIGHT);
  leds[x_2 + y_2 * WIDTH] = CRGB::Black;
}
