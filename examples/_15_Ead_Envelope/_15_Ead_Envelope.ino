/*  Example playing an enveloped noise source
 *  using Cuttlefish sonification library.
 *
 *  Demonstrates Ead (exponential attack decay).
 *
 *  Circuit: Audio output on digital pin 9.
 *
 *  unbackwards@gmail.com 2012.
 *  This example code is in the public domain.
 */

#include <CuttlefishGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/whitenoise8192_int8.h>
#include <Ead.h> // exponential attack decay
#include <DelayCuttlefish.h>

#define CONTROL_RATE 256 // powers of 2 please

Oscil <WHITENOISE8192_NUM_CELLS, AUDIO_RATE> aNoise(WHITENOISE8192_DATA); // audio noise
DelayCuttlefish kDelay(CONTROL_RATE); // for triggering envelope start
Ead kEnvelope(CONTROL_RATE); // resolution will be CONTROL_RATE

int gain;

void setup(){
  startCuttlefish(CONTROL_RATE);
  // cast to float because the resulting freq will be small
  aNoise.setFreq((float)AUDIO_RATE/WHITENOISE8192_SAMPLERATE);
  int attack = 20;
  int decay = 600;
  kEnvelope.set(attack,decay+100);
  kDelay.set(attack+decay);
}


void updateControl(){
  if(kDelay.ready()){
    kEnvelope.start();
    kDelay.start();
  }
  gain = (int) kEnvelope.next();
  Serial.println(gain);
}


int updateAudio(){
  return (gain*aNoise.next())>>8;
}


void loop(){
  audioHook(); // required here
}







