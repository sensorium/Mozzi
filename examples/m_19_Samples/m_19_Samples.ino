/*  Example of playing sampled sounds,
 *  using Mozzi sonification library.
 *
 *  Demonstrates one-shot samples scheduled
 *  with EventDelay(), and fast random numbers with 
 *  xorshift96() and rand(), a more friendly wrapper for xorshift96().
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
#include <samples/bamboo1/bamboo1_1024_int8.h> // wavetable data
#include <samples/bamboo1/bamboo2_1024_int8.h> // wavetable data
#include <samples/bamboo1/bamboo3_2048_int8.h> // wavetable data
#include <EventDelay.h>
#include <mozzi_rand.h>

#define CONTROL_RATE 64

// use: Sample <table_size, update_rate> SampleName (wavetable)
Sample <BAMBOO1_1024_NUM_CELLS, AUDIO_RATE> aBamboo1(BAMBOO1_1024_DATA);
Sample <BAMBOO2_1024_NUM_CELLS, AUDIO_RATE> aBamboo2(BAMBOO2_1024_DATA);
Sample <BAMBOO3_2048_NUM_CELLS, AUDIO_RATE> aBamboo3(BAMBOO3_2048_DATA);

// for scheduling audio gain changes
EventDelay <CONTROL_RATE>  kTriggerDelay;

void setup(){
  startMozzi(CONTROL_RATE);
  aBamboo1.setFreq((float) BAMBOO1_1024_SAMPLERATE / (float) BAMBOO1_1024_NUM_CELLS); // play at the speed it was recorded at
  aBamboo2.setFreq((float) BAMBOO2_1024_SAMPLERATE / (float) BAMBOO2_1024_NUM_CELLS);
  aBamboo3.setFreq((float) BAMBOO3_2048_SAMPLERATE / (float) BAMBOO3_2048_NUM_CELLS);
  kTriggerDelay.set(111); // countdown ms, within resolution of CONTROL_RATE
}


unsigned char randomByte(){
  return highByte(xorshift96());
}

unsigned char randomGain(){
  //return lowByte(xorshift96())<<1;
  return rand(200) + 55;
}

// referencing members from a struct is meant to be a bit faster than seperately
// ....haven't actually tested it here...
struct gainstruct{
  unsigned char gain1;
  unsigned char gain2;
  unsigned char gain3;
}
gains;


void updateControl(){
  if(kTriggerDelay.ready()){
    switch(rand(0, 3)) {
    case 0:
      gains.gain1 = randomGain();
      aBamboo1.start();
      break;
    case 1:
      gains.gain2 = randomGain();
      aBamboo2.start();
      break;
    case 2:
      gains.gain3 = randomGain();
      aBamboo3.start();
      break;
    }
    kTriggerDelay.start();
  }
}


int updateAudio(){
  int asig= (int) ((long) aBamboo1.next()*gains.gain1 +
    aBamboo2.next()*gains.gain2 +
    aBamboo3.next()*gains.gain3)/16;
  //clip to keep audio loud but still in range
  if (asig > 243) asig = 243;
  if (asig < -244) asig = -244;
  return asig;
}


void loop(){
  audioHook();
}











