/*  Example of a sound being toggled on an off,
 *  using Cuttlefish sonification library.
 *
 *  Demonstrates scheduling with DelayCuttlefish.
 *
 *  Circuit: Audio output on digital pin 9.
 *
 *  unbackwards@gmail.com 2012.
 *  This example code is in the public domain.
 */

#include <CuttlefishGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin8192_int8.h> // sine table for oscillator
#include <DelayCuttlefish.h>

#define CONTROL_RATE 64

// use: Oscil <table_size, UPDATE_RATE> oscilName (wavetable)
Oscil <SIN8192_NUM_TABLE_CELLS, AUDIO_RATE> aSin(SIN8192_DATA);

// for scheduling audio gain changes
DelayCuttlefish kBoom(CONTROL_RATE);

char gain = 1;

void setup(){
  startCuttlefish(CONTROL_RATE);
  aSin.setFreq(330u); // set the frequency, using an unsigned int or a float
  kBoom.set(1000); // 1 second countdown, within resolution of CONTROL_RATE
}


void updateControl(){
  if(kBoom.ready()){
    gain = 1-gain; // flip 0/1
    kBoom.start();
  }
}


int updateAudio(){
  return aSin.next()*gain;
}


void loop(){
  audioHook();
}



