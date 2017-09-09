#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <Arduino.h>
#include "Effect.h"

class Sequence {
 public:
  enum State : uint8_t {
    INIT,
    STOP,
    PLAY,
    PAUSE
  };

  Sequence(void) : effectList(0), currentEffect(0), state(INIT) {
  }
  virtual ~Sequence(void) {
    releaseEffects();
  }
 
  virtual void initEffects(void) = 0;
  void addEffect(Effect* effect);
  void releaseEffects(void);
  void update(Adafruit_NeoPixel& strip);
  void forward(unsigned long ms);
  void rewind(unsigned long ms);
  void stop(void);
  void play(void);

  void finished(void) { stop(); }

 protected:
  void updatePlay(Adafruit_NeoPixel& strip);
  void updatePause(Adafruit_NeoPixel& strip);

 private:
  struct EffectNode {
    Effect* effect;
    EffectNode* next;
    unsigned int startTime;
  };

  EffectNode* effectList;
  EffectNode* currentEffect;
  unsigned long timeCounter;
  State state;
};

#endif
