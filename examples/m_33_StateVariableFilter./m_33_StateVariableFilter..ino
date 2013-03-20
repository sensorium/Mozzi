/*  Example of filtering a wave,
 *  using Mozzi sonification library.
 *
 *  Demonstrates StateVariable().
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
#include <tables/whitenoise8192_int8.h>
#include <tables/cos2048_int8.h> // for filter modulation
#include <StateVariable.h>
#include <mozzi_rand.h> // for rand()

#define CONTROL_RATE 64 // powers of 2 please

Oscil <WHITENOISE8192_NUM_CELLS, AUDIO_RATE> aNoise(WHITENOISE8192_DATA); // audio noise
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kFilterMod(COS2048_DATA);

StateVariable <NOTCH> svf;

void setup(){
  startMozzi(CONTROL_RATE);
  Serial.begin(115200);
  // cast to float because the resulting freq will be small and fractional
  aNoise.setFreq((float)AUDIO_RATE/WHITENOISE8192_SAMPLERATE);
  kFilterMod.setFreq(1.3f);
  svf.setResonance(25.f);
  svf.setCentreFreq(1200.f);
}

void loop(){
  audioHook();
}

void updateControl(){
  // change the modulation frequency now and then
  if (rand((byte)CONTROL_RATE/2) == 0){ // about once every half second
    kFilterMod.setFreq((float)lowByte(xorshift96())/64);
  }
  float cutoff_freq = 2200.f + kFilterMod.next()*12;
  svf.setCentreFreq(cutoff_freq);
}

int updateAudio(){
  // watch output levels, they can distort if too high
  return svf.next(aNoise.next())>>3;
}






