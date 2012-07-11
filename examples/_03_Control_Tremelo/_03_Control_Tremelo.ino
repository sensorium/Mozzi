/*  Example of amplitude modulation (as tremelo),
 *  using Mozzi sonification library.
 *
 *  Demonstrates audio and control rate updates.
 *  The tremelo oscillator is updated at control rate,
 *  saving processor time by incrementing the gain
 *  2048 times per second (quite a lot!) rather than at the
 *  audio rate of 16384 Hz.  The control rate is set high
 *  to avoid "zipper noise" when changing
 *  the audio gain by steps.
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
#include <Oscil.h>
#include <tables/cos8192_int8.h> // table for Oscils to play
#include <utils.h> // for mtof

#define CONTROL_RATE 2048 // powers of 2 please

Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aSig(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kTremelo(COS8192_DATA);

// updated by kTremelo and reused in updateAudio until next control step
char gain;

void setup(){
  startMozzi(CONTROL_RATE);
  aSig.setFreq(mtof(60.f));
  kTremelo.setFreq(3.5f);
}

void loop(){
  audioHook();
}

void updateControl(){
   gain = kTremelo.next();
}

int updateAudio(){
  // Mozzi limits output to -244 to 243 range (almost 9 bits)
  return ((int) aSig.next() * gain) >> 8; // shift back to 8 bit audio output range after multiplying
}
