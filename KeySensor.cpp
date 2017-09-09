#include "KeySensor.h"

constexpr unsigned long CALIBRATION_MAX = 0xFFFFFF;
static constexpr bool DEBUG_MESSAGES = false;

KeySensor::KeySensor(byte sendPin_, byte backPin_, byte stopPin_, byte playPin_,
                     byte fwdPin_, byte sampleMax_, byte debounceMin_,
                     unsigned long debounceTimeout_, float threshold_)
           : backSensor(sendPin_, backPin_), stopSensor(sendPin_, stopPin_),
             playSensor(sendPin_, playPin_), fwdSensor(sendPin_, fwdPin_),
             sampleMax(sampleMax_), debounceMin(debounceMin_),
             debounceTimeout(debounceTimeout_), threshold(threshold_),
             debounceCount(0), currentKey(KEY_NONE),
             candidateKey(KEY_NONE), calibrationKey(KEY_NONE),
             candidateLevel(0), calibrationLevel(CALIBRATION_MAX),
             backNoiseLevel(CALIBRATION_MAX), stopNoiseLevel(CALIBRATION_MAX),
             playNoiseLevel(CALIBRATION_MAX), fwdNoiseLevel(CALIBRATION_MAX),
             keyChangedFlag(false) {
  // Nothing to do
}

KeySensor::KeyState KeySensor::getKeyPressed(void) {
  return currentKey;
}

void KeySensor::update(void) {
  long playValue = playSensor.capacitiveSensorRaw(sampleMax);
  long stopValue = stopSensor.capacitiveSensorRaw(sampleMax);
  long backValue = backSensor.capacitiveSensorRaw(sampleMax);
  long fwdValue = fwdSensor.capacitiveSensorRaw(sampleMax);

  switch (calibrationKey) {
    case KEY_PLAY:
      calibrationLevel += playValue;
      break;

    case KEY_STOP:
      calibrationLevel += stopValue;
      break;

    case KEY_BACK:
      calibrationLevel += backValue;
      break;

    case KEY_FWD:
      calibrationLevel += fwdValue;
      break;
  }

  playValue = playValue > playNoiseLevel ? playValue - playNoiseLevel : 0;
  stopValue = stopValue > stopNoiseLevel ? stopValue - stopNoiseLevel : 0;
  backValue = backValue > backNoiseLevel ? backValue - backNoiseLevel : 0;
  fwdValue = fwdValue > fwdNoiseLevel ? fwdValue - fwdNoiseLevel : 0;

  // Find the current capacitance level for the candidate key

  long candidateValue = 0;

  switch (candidateKey) {
    KEY_NONE:
      candidateValue = threshold;
      break;

    KEY_PLAY:
      candidateValue = playValue;
      break;

    KEY_STOP:
      candidateValue = stopValue;
      break;

    KEY_BACK:
      candidateValue = backValue;
      break;

    KEY_FWD:
      candidateValue = fwdValue;
      break;
  }

  // Check whether any other key has a larger value

  KeyState newKey = candidateKey;

  if (threshold > candidateValue) {
    newKey = KEY_NONE;
    //Serial.println(F("Picked KEY_NONE"));
    candidateValue = threshold;
  }
  if (playValue > candidateValue) {
    newKey = KEY_PLAY;
    //Serial.println(F("Picked KEY_PLAY"));
    candidateValue = playValue;
  }
  if (stopValue > candidateValue) {
    newKey = KEY_STOP;
    //Serial.println(F("Picked KEY_STOP"));
    candidateValue = stopValue;
  }
  if (backValue > candidateValue) {
    newKey = KEY_BACK;
    //Serial.println(F("Picked KEY_BACK"));
    candidateValue = backValue;
  }
  if (fwdValue > candidateValue) {
    newKey = KEY_FWD;
    //Serial.println(F("Picked KEY_FWD"));
    candidateValue = fwdValue;
  }

  if (newKey != candidateKey) {
    // Change to a new candidate key
    candidateKey = newKey;
    debounceTimeout.restart();
    debounceCount = 0;
    candidateLevel = 0;
    calibrationLevel = 0;
  } else {
    // The current candidate key is still the best match. Debounce.
    debounceCount++;
    candidateLevel += candidateValue;
    
    if (debounceTimeout.expired() && debounceCount >= debounceMin) {
      // Finished debouncing
      if (currentKey != candidateKey) {
        keyChangedFlag = true;
        currentKey = candidateKey;
        Serial.println("Key changed");
      }
      candidateLevel = candidateLevel / debounceCount;
      if (DEBUG_MESSAGES) {
        Serial.print(F("Candidate level: "));
        Serial.println(candidateLevel);
      }
      calibrationLevel = calibrationLevel / debounceCount;
      if (DEBUG_MESSAGES) {
        Serial.print(F("Calibration level: "));
        Serial.println(calibrationLevel);
      }
      
      switch (calibrationKey) {
        case KEY_NONE:
          if (DEBUG_MESSAGES) {
            Serial.println(F("Cal: KEY_NONE"));
          }
          calibrationKey = KEY_BACK;
          break;
          
        case KEY_BACK:
          if (calibrationLevel < backNoiseLevel) {
            backNoiseLevel = calibrationLevel;
          } else {
            backNoiseLevel++;
          }
          if (DEBUG_MESSAGES) {
            Serial.println(F("Cal: KEY_BACK"));
          }
          calibrationKey = KEY_STOP;
          break;
          
        case KEY_STOP:
          if (calibrationLevel < stopNoiseLevel) {
            stopNoiseLevel = calibrationLevel;
          } else {
            stopNoiseLevel++;
          }
          if (DEBUG_MESSAGES) {
            Serial.println(F("Cal: KEY_STOP"));
          }
          calibrationKey = KEY_PLAY;
          break;
          
        case KEY_PLAY:
          if (calibrationLevel < playNoiseLevel) {
            playNoiseLevel = calibrationLevel;
          } else {
            playNoiseLevel++;
          }
          if (DEBUG_MESSAGES) {
            Serial.println(F("Cal: KEY_PLAY"));
          }
          calibrationKey = KEY_FWD;
          break;
          
        case KEY_FWD:
          if (calibrationLevel < fwdNoiseLevel) {
            fwdNoiseLevel = calibrationLevel;
          } else {
            fwdNoiseLevel++;
          }
          if (DEBUG_MESSAGES) {
            Serial.println(F("Cal: KEY_FWD"));
          }
          calibrationKey = KEY_BACK;
          break;
      }
      debounceTimeout.restart();
      debounceCount = 0;
      candidateLevel = 0;
      calibrationLevel = 0;
    }
  }
}

void KeySensor::dumpStats(void) {
  Serial.println(F("====== KeySensor stats ======="));
  Serial.print(F("Debounce count: "));
  Serial.println(debounceCount);
}

