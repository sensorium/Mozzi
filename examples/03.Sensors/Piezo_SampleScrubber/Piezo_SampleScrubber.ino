/*
  Example of simple "scrubbing" through a sampled sound with a knob
  using Mozzi sonification library.

  Demonstrates getting audio samples from a table using a Line to
  slide between different indexes.
  Also demonstrates mozziAnalogRead(pin), a non-blocking replacement for mozziAnalogRead

  This example goes with a tutorial on the Mozzi site:
  http://sensorium.github.io/Mozzi/learn/introductory-tutorial/

  The circuit:
    Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.io/Mozzi/

  Piezo on analog pin A3:
    + connection of the piezo attached to the analog pin
    - connection of the piezo attached to ground
    1-megOhm resistor attached from the analog pin to ground

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2013-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#include <Mozzi.h>
#include <Sample.h> // Sample template
#include "blahblah4b_int8.h"
#include <Line.h>
#include <Smooth.h>

#define INPUT_PIN 3

// use this smooth out the wandering/jumping rate of scrubbing, gives more convincing reverses
Smooth <int> kSmoothOffset(0.85f);


// use: Sample <table_size, update_rate, interpolation > SampleName (wavetable)
Sample <BLAHBLAH4B_NUM_CELLS, MOZZI_AUDIO_RATE, INTERP_LINEAR> aSample(BLAHBLAH4B_DATA);

//Line to scrub through sample at audio rate, Line target is set at control rate
Line <Q16n16> scrub; // Q16n16 fixed point for high precision

// the number of audio steps the line has to take to reach the next offset
const unsigned int AUDIO_STEPS_PER_CONTROL = MOZZI_AUDIO_RATE / MOZZI_CONTROL_RATE;


void setup(){
  aSample.setLoopingOn();
  startMozzi();
}


void updateControl(){

  // read the pot
  int sensor_value = mozziAnalogRead<10>(INPUT_PIN); // value is 0-1023

  // map it to an 8 bit range for efficient calculations in updateAudio
  int target = ((long) sensor_value * BLAHBLAH4B_NUM_CELLS) >> 10; // >> 10 is / 1024
  int smooth_offset = kSmoothOffset.next(target);

  // set new target for interpolating line to scrub to
  scrub.set(Q16n0_to_Q16n16(smooth_offset), AUDIO_STEPS_PER_CONTROL);
}


AudioOutput updateAudio(){
  return MonoOutput::from8Bit(aSample.atIndex(Q16n16_to_Q16n0(scrub.next())));
}


void loop(){
  audioHook();
}
