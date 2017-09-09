// MIT License
//
// Copyright (c) 2017 Robert Black
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <Adafruit_NeoPixel.h>
#include <CapacitiveSensor.h>

#include "SpeakerSequence.h"
#include "KeySensor.h"

constexpr byte speechLength = 5 * 60;
constexpr byte graceSeconds = 30;

constexpr byte sendPin = 8;
constexpr byte receiveBackPin = 4;
constexpr byte receiveStopPin = 5;
constexpr byte receivePlayPin = 6;
constexpr byte receiveFwdPin = 7;

KeySensor keySensor(sendPin, receiveBackPin, receiveStopPin, receivePlayPin,
                    receiveFwdPin, 20, 3, 100, 45.0);
                    
constexpr byte pixelPin = 9;
constexpr byte pixelCount = 8;
constexpr byte pixelType = NEO_GRB + NEO_KHZ800;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(pixelCount, pixelPin, pixelType);

SpeakerSequence sequence(speechLength, graceSeconds);

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(9600);
}

void loop() {
  keySensor.update();

  if (keySensor.keyChanged()) {
    Serial.println(F("Key changed detected"));
    KeySensor::KeyState keyPressed = keySensor.getKeyPressed();

    switch (keyPressed) {
      case KeySensor::KEY_NONE:
        Serial.println(F("NONE"));
        break;
        
      case KeySensor::KEY_PLAY:
        Serial.println(F("PLAY"));
        sequence.play();
        break;
  
      case KeySensor::KEY_STOP:
        Serial.println(F("STOP"));
        sequence.stop();
        sequence.forward(15000);
        break;

      case KeySensor::KEY_BACK:
        Serial.println(F("BACK"));
        sequence.rewind(15000);
        break;
        
      case KeySensor::KEY_FWD:
        Serial.println(F("FWD"));
        sequence.forward(15000);
        break;
    }
  }

  sequence.update(strip);
}

