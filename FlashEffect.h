#ifndef FLASHEFFECT_H
#define FLASHEFFECT_H

#include "Effect.h"
#include <Adafruit_NeoPixel.h>
#include <SimpleTimeout.h>

class FlashEffect : public Effect {
 public:
  FlashEffect(unsigned int duration_, unsigned long flashPeriod,
              uint32_t color1_, uint32_t color2_)
    : Effect(duration_), timeout(flashPeriod / 2),
      color1(color1_), color2 (color2_), flash(true) {
    // Nothing else to do
  }

  void show(Adafruit_NeoPixel& strip, unsigned long) override {
    if (timeout.expired()) {
      flash = !flash;
      timeout.restart();
    }
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, flash ? color1 : color2);
    }
    strip.show();
  }

 private:
  SimpleTimeout timeout;
  uint32_t color1;
  uint32_t color2;
  bool flash;
};

#endif
