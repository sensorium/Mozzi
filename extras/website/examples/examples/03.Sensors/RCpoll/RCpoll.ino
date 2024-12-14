/*  Sets the frequency of an oscillator with a resistive sensor on a digital pin,
 *  using Mozzi sonification library.
 *
 *  Demonstrates RCpoll to read a resistive sensor on a digital pin,
 *  without blocking audio output.
 *
 *  Control circuit: http://arduino.cc/en/Tutorial/RCtime
 *  Values for components which work with this sketch are:
 *  Sensing Capacitor .1uf (104)
 *  Variable resistor 1 MegaOhm
 *  Input resistor 470 Ohms
 *
                           +5V
                            |
                            |
                           ___
                           ___    Sensing Cap
                            |      .1 uf
                            |
sPin ---\/\/\/-----.
       220 - 1K       |
                            |
                            \
                            /     Variable Resistive Sensor
                            \     Photocell, phototransistor, FSR etc.
                            /
                            |
                            |
                          _____
                           ___
                            _

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2013-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#define MOZZI_CONTROL_RATE 128 // Hz, powers of 2 are most reliable
#include <Mozzi.h>
#include <Oscil.h>
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <RCpoll.h>

#define SENSOR_PIN 4            // digital pin for sensor input

Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin(SIN2048_DATA);
RCpoll <SENSOR_PIN> sensor;


void setup(){
  //Serial.begin(9600); // for Teensy 3.1, beware printout can cause glitches
  Serial.begin(115200);
  startMozzi();
}



void updateControl(){
  int freq = 60 + 8*sensor.next(); // sensor ranges about 0 to 13
  Serial.println(freq);
  aSin.setFreq(freq);
}


AudioOutput updateAudio(){
  return MonoOutput::from8Bit(aSin.next());
}


void loop(){
  audioHook();
}
