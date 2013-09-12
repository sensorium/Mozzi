/*
  Example of simple "scrubbing" through a sampled sound with a knob
  using Mozzi sonification library.
 
  Demonstrates getting audio samples from a table using a Line to
  slide between different indexes.
  Also demonstrates mozziAnalogRead(pin), a non-blocking replacement for mozziAnalogRead
 
  This example goes with a tutorial on the Mozzi site:
  http://sensorium.github.io/Mozzi/learn/Mozzi_Introductory_Tutorial.pdf
  
  The circuit:
    Audio output on digital pin 9 (on a Uno or similar), or 
    check the README or http://sensorium.github.com/Mozzi/
 
  Piezo on analog pin A3:
    + connection of the piezo attached to the analog pin
    - connection of the piezo attached to ground
    1-megOhm resistor attached from the analog pin to ground
 
  Mozzi help/discussion/announcements:
  https://groups.google.com/forum/#!forum/mozzi-users
 
  Tim Barrass 2013.
  This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Sample.h> // Sample template
#include "blahblah4b_int8.h"
#include <Line.h>
#include <Smooth.h>


// use this smooth out the wandering/jumping rate of scrubbing, gives more convincing reverses
Smooth <int> kSmoothOffset(0.85f);


// use: Sample <table_size, update_rate, interpolation > SampleName (wavetable)
Sample <BLAHBLAH4B_NUM_CELLS, AUDIO_RATE, INTERP_LINEAR> aSample(BLAHBLAH4B_DATA);

//Line to scrub through sample at audio rate, Line target is set at control rate
Line <Q16n16> scrub; // Q16n16 fixed point for high precision

// the number of audio steps the line has to take to reach the next offset
const unsigned int AUDIO_STEPS_PER_CONTROL = AUDIO_RATE / CONTROL_RATE;


#define INPUT_PIN 3 


void setup(){
  aSample.setLoopingOn();
  startMozzi();
}


void updateControl(){

  // read the pot
  int sensor_value = mozziAnalogRead(INPUT_PIN); // value is 0-1023

  // map it to an 8 bit range for efficient calculations in updateAudio
  int target = ((long) sensor_value * BLAHBLAH4B_NUM_CELLS) >> 10; // >> 10 is / 1024
  int smooth_offset = kSmoothOffset.next(target);

  // set new target for interpolating line to scrub to
  scrub.set(Q16n0_to_Q16n16(smooth_offset), AUDIO_STEPS_PER_CONTROL);
}


int updateAudio(){
  return aSample.atIndex(Q16n16_to_Q16n0(scrub.next()));
}


void loop(){
  audioHook();
}
