#ifndef SOLIDEFFECT_H
#define SOLIDEFFECT_H

#include "Effect.h"
#include <Adafruit_NeoPixel.h>

class SolidEffect : public Effect {
 public:
  SolidEffect(unsigned int duration_, uint32_t color_)
    : Effect(duration_), color(color_){
    // Nothing else to do
  }

  void show(Adafruit_NeoPixel& strip, unsigned long) override {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, color);
    }
    strip.show();
  }

 private:
  uint32_t color;
};

#endif
