/*  Example of "scraping" through a sampled sound
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
#include <tables/horse_lips_8192_int8.h> // table for Sample
#include <EventDelay.h>

#define CONTROL_RATE 64

// use: Sample <table_size, update_rate> SampleName (wavetable)
Sample <HORSE_LIPS_8192_NUM_CELLS, AUDIO_RATE> aSample(HORSE_LIPS_8192_DATA);

// for scheduling audio gain changes
EventDelay <CONTROL_RATE>  kTriggerDelay;

char gain = 1;
unsigned int offset =0;

unsigned char scratch = 0;

void setup(){
  startMozzi(CONTROL_RATE);
  aSample.setFreq((float) HORSE_LIPS_8192_SAMPLERATE / (float) HORSE_LIPS_8192_NUM_CELLS); // play at the speed it was recorded at
  kTriggerDelay.set(1000); // 1 second countdown, within resolution of CONTROL_RATE
}

void updateControl(){
  if(kTriggerDelay.ready()){
    if (scratch == 1){
      scratch = 0;
      aSample.start();
      kTriggerDelay.set(500);
    }
    else{
      scratch = 1;
      offset = 2048;
      kTriggerDelay.set(2000);
    }

    kTriggerDelay.start();
  }

  if (scratch == 1) {
    aSample.start(offset);
    offset += 4;
  }

}


int updateAudio(){
  return (int) aSample.next()*gain;
}


void loop(){
  audioHook();
}






