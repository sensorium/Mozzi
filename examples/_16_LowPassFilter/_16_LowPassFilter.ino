/*  Example of filtering a wave,
 *  using Mozzi sonification library.
 *
 *  Demonstrates LowPassFilter.
 *
 *  Circuit: Audio output on digital pin 9.
 *
 *  Tim Barrass 2012.
 *  This example code is in the public domain.
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/chum9_int8.h> // recorded audio wavetable
#include <tables/cos8192_int8.h> // for filter modulation
#include <LowPassFilter.h>
#include <utils.h> // for xorshift96() random number generator

#define CONTROL_RATE 64 // powers of 2 please

Oscil<CHUM9_NUM_CELLS, AUDIO_RATE> aTriangle(CHUM9_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kFilterMod(COS8192_DATA);

LowPassFilter lpf;

void setup(){
  startMozzi(CONTROL_RATE);
  aTriangle.setFreq(2.f);
  kFilterMod.setFreq(1.3f);
  lpf.setResonance(200);
}

void loop(){
  audioHook();
}

void updateControl(){
  // change the modulation frequency now and then
  if (lowByte(xorshift96()) < 5){
    kFilterMod.setFreq((float)lowByte(xorshift96())/64);
  }
  // map the modulation into the filter range (0-255)
  unsigned char cutoff_freq = 100 + kFilterMod.next()/2;
  lpf.setCutoffFreq(cutoff_freq);
}

int updateAudio(){
  char asig = lpf.next(aTriangle.next());
  return (int) asig;
}






