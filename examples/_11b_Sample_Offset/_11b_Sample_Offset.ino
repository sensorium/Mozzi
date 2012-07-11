/*  Example of "scraping" through a sampled sound
 *  using Mozzi sonification library.
 *
 *  Demonstrates starting a sample with an offset.
 *
 *  Circuit: Audio output on digital pin 9.
 *
 *  Mozzi help/discussion/announcements:
 *  https://groups.google.com/forum/#!forum/mozzi-users
 *
 *  Tim Barrass 2012.
 *  This example code is in the public domain.
 */

#include <MozziGuts.h>
#include <Sample.h> // Sample template
#include <tables/bamboo1_1024_int8.h> // table for Sample
#include <EventDelay.h>

#define CONTROL_RATE 64

// use: Sample <table_size, update_rate> SampleName (wavetable)
Sample <BAMBOO1_1024_NUM_CELLS, AUDIO_RATE> aBamboo1(BAMBOO1_1024_DATA);

// for scheduling audio gain changes
EventDelay kTriggerDelay(CONTROL_RATE);

char gain = 1;
unsigned int offset =0;

void setup(){
  startMozzi(CONTROL_RATE);
  aBamboo1.setFreq((float) BAMBOO1_1024_SAMPLERATE / (float) BAMBOO1_1024_NUM_CELLS); // play at the speed it was recorded at
  kTriggerDelay.set(1000); // 1 second countdown, within resolution of CONTROL_RATE
}

void updateControl(){
  if(kTriggerDelay.ready()){
    //aBamboo1.start();
    kTriggerDelay.start();
    offset = 0;
  }
  offset += 4;
  aBamboo1.start(offset);
}


int updateAudio(){
  return (int) aBamboo1.next()*gain;
}


void loop(){
  audioHook();
}



