
#ifndef SPEAKERSEQUENCE_H
#define SPEAKERSEQUENCE_H

#include "Sequence.h"
#include "FlashEffect.h"
#include "SolidEffect.h"
#include "CountdownEffect.h"

class SpeakerSequence : public Sequence {
 public:
  SpeakerSequence(unsigned int speechLength_, int stopLength_ = 30)
      : speechLength(speechLength_), stopLength(stopLength_) {
    // Nothing else to do  
  }
  
  void initEffects(void) override {
    unsigned int warnTime = 0.8 * speechLength;
    unsigned int warnLength = speechLength - warnTime;
    addEffect(new FlashEffect(0, 1000,
                Adafruit_NeoPixel::Color(0, 255, 0),
                Adafruit_NeoPixel::Color(0, 0, 0)));
    addEffect(new CountdownEffect(warnTime, Adafruit_NeoPixel::Color(0, 255, 0)));
    addEffect(new CountdownEffect(warnLength, Adafruit_NeoPixel::Color(255, 255, 0))); 
    addEffect(new SolidEffect(stopLength,
                Adafruit_NeoPixel::Color(255, 0, 0)));
    addEffect(new FlashEffect(0, 500,
                Adafruit_NeoPixel::Color(255, 0, 0),
                Adafruit_NeoPixel::Color(0, 0, 0)));
  }

 private:
  unsigned int speechLength;
  unsigned int stopLength;
};

#endif
