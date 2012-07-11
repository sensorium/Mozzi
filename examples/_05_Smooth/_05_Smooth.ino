/*  Example toggling a sound on an off without obvious clicks,
 *  using Mozzi sonification library.
 *
 *  Demonstrates using Smooth to filter a control signal.
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
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <EventDelay.h>
#include <Smooth.h>
#include <utils.h>

#define CONTROL_RATE 512

// use: Oscil <table_size, update_rate> oscilName (wavetable)
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

// for scheduling audio gain changes
EventDelay kGainChangeDelay(CONTROL_RATE);

Smooth <unsigned char> kSmoothGain(0.975f);
unsigned char smoothed_gain; // for conveying kSmoothGain to updateAudio
unsigned char target_gain = 0;

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
  smoothed_gain = kSmoothGain.next(target_gain);
}


int updateAudio(){
  return ((int)aSin.next() * smoothed_gain)>>8;
}


void loop(){
  audioHook();
}




