/*  Example playing an enveloped noise source
 *  using Mozzi sonification library.
 *
 *  Demonstrates Ead (exponential attack decay).
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
#include <Oscil.h> // oscillator template
#include <tables/whitenoise8192_int8.h>
#include <Ead.h> // exponential attack decay
#include <EventDelay.h>
#include <mozzi_rand.h>

#define CONTROL_RATE 256 // powers of 2 please

Oscil <WHITENOISE8192_NUM_CELLS, AUDIO_RATE> aNoise(WHITENOISE8192_DATA); // audio noise
EventDelay <CONTROL_RATE>  kDelay; // for triggering envelope start
Ead kEnvelope(CONTROL_RATE); // resolution will be CONTROL_RATE

int gain;

void initRandomEnvelope(){

}


void setup(){
  startMozzi(CONTROL_RATE);
  randSeed(); // reseed the random generator for different results each time the sketch runs
  // use float to set freq because it will be small and fractional
  aNoise.setFreq((float)AUDIO_RATE/WHITENOISE8192_SAMPLERATE);
  kDelay.start(1000);
}


void updateControl(){
  // jump around in audio noise table to disrupt obvious looping
  aNoise.setPhase(rand((uint)WHITENOISE8192_NUM_CELLS));
  
  if(kDelay.ready()){
    // set random parameters
    unsigned int duration = rand(500u)+200;
    unsigned int attack = rand(75);
    unsigned int decay = duration - attack;
    kEnvelope.start(attack,decay);
    kDelay.start(duration+500);
  }
  gain = (int) kEnvelope.next();
}


int updateAudio(){
  return (gain*aNoise.next())>>8;
}


void loop(){
  audioHook(); // required here
}








