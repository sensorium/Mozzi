/*  Example of playing a sampled sound,
 *  using Mozzi sonification library.
 *
 *  Demonstrates one-shot samples scheduled
 *  with EventDelay.
 *
 *  Circuit: Audio output on digital pin 9.
 *
 *  Tim Barrass 2012.
 *  This example code is in the public domain.
 */

#include <MozziGuts.h>
#include <Sample.h> // Samplelator template
#include <tables/bamboo1_1024_int8.h> // sine table for Samplelator
#include <EventDelay.h>

#define CONTROL_RATE 64

// use: Sample <table_size, update_rate> SampleName (wavetable)
Sample <BAMBOO1_1024_NUM_CELLS, AUDIO_RATE> aBamboo1(BAMBOO1_1024_DATA);

// for scheduling audio gain changes
EventDelay kTriggerDelay(CONTROL_RATE);

char gain = 1;

void setup(){
  startMozzi(CONTROL_RATE);
  aBamboo1.setFreq((float) BAMBOO1_1024_SAMPLERATE / (float) BAMBOO1_1024_NUM_CELLS); // play at the speed it was recorded at
  kTriggerDelay.set(1000); // 1 second countdown, within resolution of CONTROL_RATE
}


void updateControl(){
  if(kTriggerDelay.ready()){
    aBamboo1.start();
    kTriggerDelay.start();
  }
}


int updateAudio(){
  return (int) aBamboo1.next()*gain;
}


void loop(){
  audioHook();
}



