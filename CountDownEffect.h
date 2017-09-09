#ifndef COUNTDOWNEFFECT_H
#define COUNTDOWNEFFECT_H

#include "Effect.h"
#include <Adafruit_NeoPixel.h>
#include <SimpleTimeout.h>

class CountdownEffect : public Effect {
 public:
  CountdownEffect(unsigned int duration_, uint32_t color1_, uint32_t color2_ = 0)
    : Effect(duration_),
      color1(color1_), color2 (color2_) {
    // Nothing else to do
  }

  void show(Adafruit_NeoPixel& strip, unsigned long ms) override {
    int extinguished = strip.numPixels() * ms / getDuration() / 1000;
    int lit = strip.numPixels() - extinguished;
    for (int i = 0; i < lit; i++) {
      strip.setPixelColor(i, color1);
    }
    for (int i = lit; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, color2);
    }
    strip.show();
  }

 private:
  uint32_t color1;
  uint32_t color2;
};

#endif
