/*  Example of simple "scrubbing" through a sampled sound
     using Mozzi sonification library.

    Demonstrates getting audio samples from a table using a Line to
    slide between different offsets.

   Circuit: Audio output on digital pin 9 (on a Uno or similar), or 
   check the README or http://sensorium.github.com/Mozzi/
 
   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Tim Barrass 2012-13.
   This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Sample.h> // Sample template
#include <samples/burroughs1_18649_int8.h>
#include <mozzi_rand.h>
#include <Line.h>
#include <Smooth.h>

// use: Sample <table_size, update_rate, interpolation > SampleName (wavetable)
Sample <BURROUGHS1_18649_NUM_CELLS, AUDIO_RATE, INTERP_LINEAR> aSample(BURROUGHS1_18649_DATA);

//Line to scrub through sample at audio rate, Line target is set at control rate
Line <Q16n16> scrub; // Q16n16 fixed point for high precision

// the number of audio steps the line has to take to reach the next offset
const unsigned int AUDIO_STEPS_PER_CONTROL = AUDIO_RATE / CONTROL_RATE;

int offset = 0;
int offset_advance = 400; // just a guess

// use this smooth out the wandering/jumping rate of scrubbing, gives more convincing reverses
float smoothness = 0.9f;
Smooth <int> kSmoothOffsetAdvance(smoothness);


void setup(){
  randSeed(); // fresh randomness
  aSample.setLoopingOn();
  startMozzi();
}


void updateControl(){
  
  // wandering advance rate
  offset_advance += (int)rand(20)-rand(20);
  if(!rand(CONTROL_RATE)) offset_advance = -offset_advance; // reverse sometimes
  if(!rand(CONTROL_RATE)) offset_advance = 500-rand(1000); // jump speed sometimes
 
  int smooth_offset_advance = kSmoothOffsetAdvance.next(offset_advance);
  
  offset += smooth_offset_advance;
  
  // keep offset in range
  if (offset >= BURROUGHS1_18649_NUM_CELLS) offset -= BURROUGHS1_18649_NUM_CELLS;
  if (offset < 0) offset += BURROUGHS1_18649_NUM_CELLS;

  // set new target for interpolating line to scrub to
  scrub.set(Q16n0_to_Q16n16(offset), AUDIO_STEPS_PER_CONTROL);
  
}


int updateAudio(){
  unsigned int index = Q16n16_to_Q16n0(scrub.next());
  return aSample.atIndex(index);
}


void loop(){
  audioHook();
}








