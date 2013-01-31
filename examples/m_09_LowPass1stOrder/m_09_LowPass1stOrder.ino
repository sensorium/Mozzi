/*  Example changing the volume of a sound smoothly
 *  using Mozzi sonification library.
 *
 *  Demonstrates using LowPass1stOrder to filter a control signal.
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
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <EventDelay.h>
#include <LowPass1stOrder.h>
#include <mozzi_midi.h>

#define CONTROL_RATE 512

// use: Oscil <table_size, update_rate> oscilName (wavetable)
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

// for scheduling audio gain changes
EventDelay <CONTROL_RATE>  kGainChangeDelay;

LowPass1stOrder <int, 128> kSmoothGain;
int smoothed_gain; // for conveying kSmoothGain to updateAudio
int target_gain = 0;

void setup(){
  aSin.setFreq(330u); // set the frequency with an unsigned int
  kGainChangeDelay.set(500); // 500ms countdown, within resolution of CONTROL_RATE
  startMozzi(CONTROL_RATE);
}


void updateControl(){
  if(kGainChangeDelay.ready()){
    if (target_gain == 0) {
      target_gain = 255;
    }else{
      target_gain = 0;
    }
    kGainChangeDelay.start();
  }
  // >>7 chosen through experimentation, to move to useful range
  smoothed_gain = kSmoothGain.next(target_gain)>>7;
}


int updateAudio(){
  return ((int)aSin.next() * smoothed_gain)>>8;
}


void loop(){
  audioHook();
}




