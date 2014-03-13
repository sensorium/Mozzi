/*  Example of filtering 2 waves with different filter settings,
    using Mozzi sonification library.
  
    Demonstrates 2 Oscillators, each with a LowPassFilter().
  
    Circuit: Audio output on digital pin 9 (on a Uno or similar), or 
    check the README or http://sensorium.github.com/Mozzi/
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2012.
    This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/chum9_int8.h> // recorded audio wavetable
#include <tables/cos512_int8.h> // for filter modulation
#include <LowPassFilter.h>
#include <mozzi_rand.h> // for rand()

#define CONTROL_RATE 64 // powers of 2 please

Oscil<CHUM9_NUM_CELLS, AUDIO_RATE> aCrunchySound1(CHUM9_DATA); //audio oscillator
Oscil<CHUM9_NUM_CELLS, AUDIO_RATE> aCrunchySound2(CHUM9_DATA); //audio oscillator
Oscil<COS512_NUM_CELLS, CONTROL_RATE> kFilterMod1(COS512_DATA); // to modulate filter frequency
Oscil<COS512_NUM_CELLS, CONTROL_RATE> kFilterMod2(COS512_DATA); // to modulate filter frequency

LowPassFilter lpf1;
LowPassFilter lpf2;

void setup(){
  startMozzi(CONTROL_RATE);
  aCrunchySound1.setFreq(2.f);
  aCrunchySound2.setFreq(6.f);
  kFilterMod1.setFreq(1.3f);
  kFilterMod2.setFreq(0.1f);
  lpf1.setResonance(180);
  lpf2.setResonance(220);
}


void updateControl(){
  // map the modulation depth into the filter range, 0-255 to represent 0-8192 Hz
  byte cutoff_freq1 = 100 + kFilterMod1.next()/2; // 100 ± 63
  lpf1.setCutoffFreq(cutoff_freq1);
  // also update lpf2 cutoff
  byte cutoff_freq2 = 70 + kFilterMod2.next()/4; // 70 ± 31
  lpf2.setCutoffFreq(cutoff_freq2);
}


int updateAudio(){
  return (((char)lpf1.next(aCrunchySound1.next()))>>1) + (char)lpf2.next(aCrunchySound2.next());
}


void loop(){
  audioHook();
}





