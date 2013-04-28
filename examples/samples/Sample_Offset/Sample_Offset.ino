/*  Example of crude "scraping" through a sampled sound
 *  using Mozzi sonification library.
 *
 *  Demonstrates starting a sample with an offset.
 *  
 *  todo: use a window function to do it smoothly
 *
 *  Circuit: Audio output on digital pin 9 (on a Uno or similar), or 
 *  check the README or http://sensorium.github.com/Mozzi/
 *
 *  Mozzi help/discussion/announcements:
 *  https://groups.google.com/forum/#!forum/mozzi-users
 *
 *  Tim Barrass 2012.
 *  This example code is in the public domain.
 */

#include <MozziGuts.h>
#include <Sample.h> // Sample template
#include <samples/burroughs1_18649_int8.h>
#include <mozzi_rand.h>

#define CONTROL_RATE 512

// use: Sample <table_size, update_rate> SampleName (wavetable)
Sample <BURROUGHS1_18649_NUM_CELLS, AUDIO_RATE> aSample(BURROUGHS1_18649_DATA);

int offset =0;
int offset_advance =4;


void setup(){
  startMozzi(CONTROL_RATE);
  aSample.setFreq((float) BURROUGHS1_18649_SAMPLERATE / (float) BURROUGHS1_18649_NUM_CELLS); // play at the speed it was recorded
  aSample.setLoopingOn();
}


void updateControl(){
  offset += offset_advance;
  // keep offset in range
  if (offset >= BURROUGHS1_18649_NUM_CELLS) offset -= BURROUGHS1_18649_NUM_CELLS;
  if (offset < 0) offset += BURROUGHS1_18649_NUM_CELLS;
  // choose a new offfset_advance amount now and again
  if (!rand(1000)) {
    offset_advance = 2+rand(50);
    if(rand(2)) offset_advance = -offset_advance;
  }
  aSample.start(offset);
}


int updateAudio(){
  return aSample.next();
}


void loop(){
  audioHook();
}








