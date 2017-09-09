#ifndef EFFECT_H
#define EFFECT_H

#include <Adafruit_NeoPixel.h>

class Effect {
 public:
  Effect(unsigned int duration_) : duration(duration_) {
    // Nothing to do
  }
  virtual ~Effect(void) {}
  
  virtual void show(Adafruit_NeoPixel& strip, unsigned long ms) = 0;

  inline unsigned long getDuration(void) const {
    return duration;
  }

 private:
  unsigned int duration;
};

#endif
