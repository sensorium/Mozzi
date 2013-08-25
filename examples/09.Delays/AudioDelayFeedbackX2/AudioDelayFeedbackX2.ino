/*  Example of flanging,
    using Mozzi sonification library.
  
    Demonstrates 2 oscillators going through 2 AudioDelayFeedback units,
    with straight signal mixed in.
  
    Circuit: Audio output on digital pin 9 (on a Uno or similar), or 
    check the README or http://sensorium.github.com/Mozzi/
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2012-13.
    This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/triangle_analogue512_int8.h> // wavetable for audio
#include <tables/triangle512_int8.h> // wavetable for delay sweep
#include <AudioDelayFeedback.h>
#include <mozzi_midi.h> // for mtof

#define CONTROL_RATE 128 // powers of 2 please

Oscil<TRIANGLE_ANALOGUE512_NUM_CELLS, AUDIO_RATE> aTriangle1(TRIANGLE_ANALOGUE512_DATA); // audio oscillator
Oscil<TRIANGLE_ANALOGUE512_NUM_CELLS, AUDIO_RATE> aTriangle2(TRIANGLE_ANALOGUE512_DATA); // audio oscillator

Oscil<TRIANGLE512_NUM_CELLS, CONTROL_RATE> kDelSamps1(TRIANGLE512_DATA); // for modulating delay time, measured in audio samples
Oscil<TRIANGLE512_NUM_CELLS, CONTROL_RATE> kDelSamps2(TRIANGLE512_DATA); // for modulating delay time, measured in audio samples

AudioDelayFeedback <128> aDel1;
AudioDelayFeedback <128> aDel2;

// the delay time, measured in samples, updated in updateControl, and used in updateAudio 
unsigned int del_samps1;
unsigned int del_samps2;


void setup(){
  aTriangle1.setFreq(mtof(48.f));
  kDelSamps1.setFreq(.163f); // set the delay time modulation frequency (ie. the sweep frequency)
  aDel1.setFeedbackLevel(-111); // can be -128 to 127

  aTriangle2.setFreq(mtof(52.f));
  kDelSamps2.setFreq(1.43f); // set the delay time modulation frequency (ie. the sweep frequency)
  aDel2.setFeedbackLevel(109); // can be -128 to 127

  startMozzi(CONTROL_RATE);
}


void updateControl(){
  // delay time range from 0 to 127 samples, @ 16384 samps per sec = 0 to 7 milliseconds
  del_samps1 = 64+kDelSamps1.next();

  // delay time range from 1 to 33 samples, @ 16384 samps per sec = 0 to 2 milliseconds
  del_samps2 = 17+kDelSamps2.next()/8; 
}


int updateAudio(){
  char asig1 = aTriangle1.next(); // get this so it can be used twice without calling next() again
  int aflange1 = (asig1>>3) + aDel1.next(asig1, del_samps1); // mix some straignt signal with the delayed signal

  char asig2 = aTriangle2.next(); // get this so it can be used twice without calling next() again
  int aflange2 = (asig2>>3) + aDel2.next(asig2, del_samps2); // mix some straignt signal with the delayed signal
  return (aflange1 + aflange2)>>1;
}


void loop(){
  audioHook();
}




