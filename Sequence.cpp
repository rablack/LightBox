#include "Sequence.h"

void Sequence::addEffect(Effect* effect) {
  EffectNode* newNode = new EffectNode;
  newNode->effect = effect;
  newNode->next = 0;
  
  if (effectList == 0) {
    state = STOP;
    effectList = newNode;
    newNode->startTime = 0;
  } else {
    EffectNode* node = effectList;
    while (node->next != 0) {
      node = node->next;
    }
    node->next = newNode;
    newNode->startTime = node->startTime +
                         node->effect->getDuration();
  }
  Serial.print(F("Added node with start at: "));
  Serial.println(newNode->startTime);
}

void Sequence::releaseEffects(void) {
  state = INIT;
  while (effectList != 0) {
    EffectNode* node = effectList;
    effectList = effectList->next;
    delete node;
  }
  currentEffect = 0;
}

void Sequence::update(Adafruit_NeoPixel& strip) {
  switch (state) {
    case INIT:
    case STOP:
      for (int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, 0x000606);
      }
      strip.show();
      break;

    case PLAY:
      updatePlay(strip);
      break;

    case PAUSE:
      updatePause(strip);
  }
}

void Sequence::updatePlay(Adafruit_NeoPixel& strip) {
  if (currentEffect == 0) {
    finished();
    return;
  }
  
  if (currentEffect->effect->getDuration() == 0) {
    timeCounter = currentEffect->startTime * 1000UL;
    state = PAUSE;
    return;
  }
  
  // Now is the time in ms into the sequence. In play mode the
  // timeCounter gives the effective start time of the sequence.
  unsigned long now = millis() - timeCounter;

  // effectTime is the ms time relative to the start of the effect.
  // Note that effect duration and startTime are in seconds
  unsigned long effectTime = now - currentEffect->startTime * 1000UL;

  // Check whether it is time to start the next effect in sequence
  if (effectTime >= currentEffect->effect->getDuration() * 1000UL) {
    Serial.print(F("PLAY: Effect time is: "));
    Serial.println(effectTime);
    Serial.print(F("Duration is: "));
    Serial.println(currentEffect->effect->getDuration());
    currentEffect = currentEffect->next;
    // Make sure that we always call each effect at least once.
    effectTime = 0;
  }

  if (currentEffect != 0) {
    currentEffect->effect->show(strip, effectTime);
  }
}

void Sequence::updatePause(Adafruit_NeoPixel& strip) {
  if (currentEffect == 0) {
    finished();
    return;
  }
  
  // Now is the time in ms into the sequence. In pause mode
  // timeCounter gives the pause time.
  unsigned long now = timeCounter;

  // effectTime is the ms time relative to the start of the effect.
  // Note that effect duration and startTime are in seconds
  unsigned long effectTime = now - currentEffect->startTime * 1000UL;

  // Check whether it is time to start the next effect in sequence
  // This can happen in pause mode because each effect gets called
  // at least once. This is the clock catching up to the pause
  // time.
  if (effectTime > currentEffect->effect->getDuration() * 1000) {
    Serial.print(F("PAUSE: Effect time is: "));
    Serial.println(effectTime);
    currentEffect = currentEffect->next;
    // Make sure that we always call each effect at least once.
    effectTime = 0;
  }

  if (currentEffect != 0) {
    currentEffect->effect->show(strip, effectTime);
    if (currentEffect->effect->getDuration() == 0) {
      timeCounter = currentEffect->startTime * 1000;
    }
  }
}

void Sequence::forward(unsigned long ms) {
  switch (state) {
    case PLAY:
      if (currentEffect != 0) {
        timeCounter -= ms;
        unsigned long now = millis() - timeCounter;
        unsigned long effectTime = now - currentEffect->startTime * 1000UL;
        if (effectTime > currentEffect->effect->getDuration() * 1000UL) {
          if (currentEffect->next != 0) {
            currentEffect = currentEffect->next;
            if (currentEffect->effect->getDuration() == 0) {
              state = PAUSE;
              timeCounter = currentEffect->startTime * 1000UL;
            } else {
              timeCounter = millis() - currentEffect->startTime * 1000UL;
            }
          } else {
            finished();
          }
        }
      }
      break;
      
    case PAUSE:
      if (currentEffect != 0) {
        timeCounter += ms;
        unsigned long now = timeCounter;
        unsigned long effectTime = now - currentEffect->startTime * 1000UL;
        if (effectTime > currentEffect->effect->getDuration() * 1000UL) {
          if (currentEffect->next != 0) {
            currentEffect = currentEffect->next;
            timeCounter = currentEffect->startTime * 1000UL;
          } else {
            finished();
          }
        }
      }
      break;
  }
}

void Sequence::rewind(unsigned long ms) {
  switch (state) {
    case PLAY:
      if (currentEffect != 0) {
        if (millis() - timeCounter < ms) {
          timeCounter = millis();
          currentEffect = effectList;
        } else {
          timeCounter += ms;
        }
        unsigned long now = millis() - timeCounter;
        if (now < currentEffect->startTime * 1000L) {
          if (currentEffect == effectList) {
            // Can't rewind before first effect
            timeCounter = millis();
          } else {
            EffectNode* lastNode = effectList;
            while (lastNode->next != currentEffect && lastNode->next != 0) {
              lastNode = lastNode->next;
            }
            currentEffect = lastNode;
            if (now < currentEffect->startTime * 1000L) {
              // We don't allow rewinding the clock by more than one effect.
              // Set the clock to the start of the previous effect
              timeCounter = millis() - currentEffect->startTime;
            }
          }
        }
        if (currentEffect->effect->getDuration() == 0) {
          timeCounter = currentEffect->startTime * 1000UL;
          state = PAUSE;
        }
      }
      break;
      
    case PAUSE:
      if (currentEffect != 0) {
        if (timeCounter > ms) {
          timeCounter -= ms;
        } else {
          timeCounter = 0;
          currentEffect = effectList;
        }
        unsigned long now = timeCounter;
        if (now < currentEffect->startTime * 1000L) {
          if (currentEffect == effectList) {
            // Can't rewind before first effect
            timeCounter = 0;
          } else {
            EffectNode* lastNode = effectList;
            while (lastNode->next != currentEffect && lastNode->next != 0) {
              lastNode = lastNode->next;
            }
            currentEffect = lastNode;
            if (now < currentEffect->startTime * 1000L) {
              // We don't allow rewinding the clock by more than one effect.
              // Set the clock to the start of the previous effect
              timeCounter = currentEffect->startTime * 1000UL;
            }
          }
        }
      }
      Serial.print(F("Time counter is : "));
      Serial.println(timeCounter);
      break;
  }
}

void Sequence::stop(void) {
  if (state == INIT) {
    return;
  }
  state = STOP;
  currentEffect = 0;
}

void Sequence::play(void) {
  switch (state) {
    case INIT:
      initEffects();
    case STOP:
      currentEffect = effectList;
      timeCounter = millis();
      state = PLAY;
      break;

    case PAUSE:
      if (currentEffect != 0) {
        if (currentEffect->effect->getDuration() == 0) {
          currentEffect = currentEffect->next;
        }
      }
      timeCounter = millis() - timeCounter;
      state = PLAY;
  }
}

