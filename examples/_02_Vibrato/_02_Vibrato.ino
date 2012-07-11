/*  Example playing a sinewave with vibrato,
 *  using Mozzi sonification library.
 *
 *  Demonstrates simple FM using phase modulation.
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

#define CONTROL_RATE 64 // powers of 2 please

Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aVibrato(COS8192_DATA);

 const long intensity = 300;

void setup(){
  startMozzi(CONTROL_RATE);
  aCos.setFreq(mtof(84.f));
  aVibrato.setFreq(15.f);
}

void loop(){
  audioHook();
}

void updateControl(){
}

int updateAudio(){
    long vibrato = intensity * aVibrato.next();
    return (int)aCos.phMod(vibrato);
}
