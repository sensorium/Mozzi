/*  Example of filtering a wave,
    using Mozzi sonification library.
  
    Demonstrates StateVariable().
  
    Circuit: Audio output on digital pin 9 (on a Uno or similar), or 
    check the README or http://sensorium.github.com/Mozzi/
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2012.
    This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/whitenoise8192_int8.h>
#include <tables/cos2048_int8.h> // for filter modulation
#include <StateVariable.h>
#include <mozzi_rand.h> // for rand()

Oscil <WHITENOISE8192_NUM_CELLS, AUDIO_RATE> aNoise(WHITENOISE8192_DATA); // audio noise
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kFilterMod(COS2048_DATA);

StateVariable <NOTCH> svf; // can be LOWPASS, BANDPASS, HIGHPASS or NOTCH


void setup(){
  startMozzi();
  // cast to float because the resulting freq will be small and fractional
  aNoise.setFreq((float)AUDIO_RATE/WHITENOISE8192_SAMPLERATE);
  kFilterMod.setFreq(1.3f);
  svf.setResonance(25);
  svf.setCentreFreq(1200);
}



void updateControl(){
  if (rand(CONTROL_RATE/2) == 0){ // about once every half second
    kFilterMod.setFreq((float)rand(255)/64);  // choose a new modulation frequency
  }
  int cutoff_freq = 2200 + kFilterMod.next()*12;
  svf.setCentreFreq(cutoff_freq);
}


int updateAudio(){
  // watch output levels, they can distort if too high
  // also, at very resonant settings, the input signal may need attenuating
  return svf.next(aNoise.next())>>3;
}


void loop(){
  audioHook();
}





