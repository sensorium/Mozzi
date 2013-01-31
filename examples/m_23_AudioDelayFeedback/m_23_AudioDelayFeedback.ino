/*  Example of flanging,
 *  using Mozzi sonification library.
 *
 *  Demonstrates AudioDelayFeedback.
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
#include <tables/triangle_analogue512_int8.h> // wavetable
#include <tables/cos8192_int8.h> // wavetable
#include <AudioDelayFeedback.h>
#include <mozzi_midi.h> // for mtof

#define CONTROL_RATE 128 // powers of 2 please

Oscil<TRIANGLE_ANALOGUE512_NUM_CELLS, AUDIO_RATE> aTriangle(TRIANGLE_ANALOGUE512_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kDelSamps(COS8192_DATA);

AudioDelayFeedback <128> aDel;
unsigned char del_samps;

void setup(){
  startMozzi(CONTROL_RATE);
  aTriangle.setFreq(mtof(48.f));
  kDelSamps.setFreq(.163f);
  aDel.setFeedbackLevel(-101);
}

void loop(){
  audioHook();
}

void updateControl(){
  del_samps = 64+kDelSamps.next()/2;
}

int updateAudio(){
  return aDel.next(aTriangle.next(), del_samps);
}






