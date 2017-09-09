#ifndef KEYSENSOR_H
#define KEYSENSOR_H

#include <Arduino.h>
#include "SimpleTimeout.h"
#include <CapacitiveSensor.h>

class KeySensor {
 public:
  enum KeyState : uint8_t {
    KEY_NONE,
    KEY_BACK,
    KEY_STOP,
    KEY_PLAY,
    KEY_FWD
  };
  KeySensor(byte sendPin_, byte backPin_, byte stopPin_, byte playPin_,
            byte fwdPin_, byte sampleMax_, byte debounceMin_,
            unsigned long debounceTimeout_, float threshold_);

  KeyState getKeyPressed(void);
  void update(void);
  
  bool keyChanged(void) {
    bool result = keyChangedFlag;
    keyChangedFlag = false;
    return result;
  }
  
  void dumpStats(void);
  
 private:
  CapacitiveSensor backSensor;
  CapacitiveSensor stopSensor;
  CapacitiveSensor playSensor;
  CapacitiveSensor fwdSensor;
  const byte sampleMax;
  const byte debounceMin;
  SimpleTimeout debounceTimeout;
  float threshold;

  uint16_t debounceCount;
  KeyState currentKey;
  KeyState candidateKey;
  KeyState calibrationKey;
  long candidateLevel;
  long calibrationLevel;
  long backNoiseLevel;
  long stopNoiseLevel;
  long playNoiseLevel;
  long fwdNoiseLevel;
  bool keyChangedFlag;
};

#endif
