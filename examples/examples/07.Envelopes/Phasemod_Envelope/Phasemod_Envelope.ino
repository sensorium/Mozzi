/*  Example of a modulating timbre with a rhythmic
    volume envelope, using Mozzi sonification library.
  
  
    Demonstrates phase modulation and modifying
    the volume of a sound with an envelope setd in a table.
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2012.
    This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/cos8192_int8.h>
#include <tables/envelop2048_uint8.h>

#define CONTROL_RATE 512 // quite fast, keeps modulation smooth

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <COS8192_NUM_CELLS, AUDIO_RATE> aCarrier(COS8192_DATA);
Oscil <COS8192_NUM_CELLS, AUDIO_RATE> aModulator(COS8192_DATA);
Oscil <COS8192_NUM_CELLS, AUDIO_RATE> aModWidth(COS8192_DATA);
Oscil <COS8192_NUM_CELLS, CONTROL_RATE> kModFreq1(COS8192_DATA);
Oscil <COS8192_NUM_CELLS, CONTROL_RATE> kModFreq2(COS8192_DATA);
Oscil <ENVELOP2048_NUM_CELLS, AUDIO_RATE> aEnvelop(ENVELOP2048_DATA);


void setup() {
  startMozzi(CONTROL_RATE);
  aCarrier.setFreq(220);
  kModFreq1.setFreq(1.78f);
  kModFreq2.setFreq(0.1757f);
  aModWidth.setFreq(0.1434f);
  aEnvelop.setFreq(9.0f);
}


void updateControl() {
  aModulator.setFreq(277.0f + 0.4313f*kModFreq1.next() + kModFreq2.next());
}


int updateAudio(){
  int asig = aCarrier.phMod((int)aModulator.next()*(260u+aModWidth.next()));
  return (asig*(unsigned char)aEnvelop.next())>>8;
}


void loop() {
  audioHook();
}


