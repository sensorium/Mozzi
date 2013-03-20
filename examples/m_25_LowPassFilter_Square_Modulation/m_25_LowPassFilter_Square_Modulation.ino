/*  Example of filtering a wave,
 *  using Mozzi sonification library.
 *
 *  Demonstrates LowPassFilter().
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
#include <Oscil.h>
#include <tables/chum9_int8.h> // recorded audio wavetable
#include <tables/square_no_alias512_int8.h> // for filter modulation
#include <LowPassFilter.h>
#include <mozzi_rand.h> // for rand()

#define CONTROL_RATE 64 // powers of 2 please

Oscil<CHUM9_NUM_CELLS, AUDIO_RATE> aCrunchySound(CHUM9_DATA);
Oscil<SQUARE_NO_ALIAS512_NUM_CELLS, CONTROL_RATE> kFilterMod(SQUARE_NO_ALIAS512_DATA);

LowPassFilter lpf;

void setup(){
  startMozzi(CONTROL_RATE);
  aCrunchySound.setFreq(2.f);
  kFilterMod.setFreq(3.3f);
  lpf.setResonance(200);
}

void loop(){
  audioHook();
}

void updateControl(){
  // map the modulation into the filter range (0-255)
  unsigned char cutoff_freq = 100 + kFilterMod.next()/2;
  lpf.setCutoffFreq(cutoff_freq);
}

int updateAudio(){
  char asig = lpf.next(aCrunchySound.next());
  return (int) asig;
}






