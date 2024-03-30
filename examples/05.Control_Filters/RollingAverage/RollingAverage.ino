/*  Example changing the frequency of 2 sinewaves with 1 analog input,
    using RollingAverage() to filter one of the control signals
    and the other one unfiltered,
    using Mozzi sonification library.

    Demonstrates the difference between a raw control
    signal and one smoothed with RollingAverage().

    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.io/Mozzi/

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2013-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#include <Mozzi.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <RollingAverage.h>

#define INPUT_PIN 0 // analog control input

// oscils to compare bumpy to averaged control input
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin0(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin1(SIN2048_DATA);

// use: RollingAverage <number_type, how_many_to_average> myThing
RollingAverage <int, 32> kAverage; // how_many_to_average has to be power of 2
int averaged;

void setup(){
  //Serial.begin(9600); // for Teensy 3.1, beware printout can cause glitches
  Serial.begin(115200);
  startMozzi();
}


void updateControl(){
  int bumpy_input = mozziAnalogRead<10>(INPUT_PIN);
  averaged = kAverage.next(bumpy_input);

  Serial.print("bumpy \t");
  Serial.print(bumpy_input);
  Serial.print("\t averaged \t");
  Serial.println(averaged);

  aSin0.setFreq(bumpy_input);
  aSin1.setFreq(averaged);
}


AudioOutput updateAudio(){
  return MonoOutput::fromAlmostNBit(10, 3*(aSin0.next()+aSin1.next()));
}


void loop(){
  audioHook();
}
