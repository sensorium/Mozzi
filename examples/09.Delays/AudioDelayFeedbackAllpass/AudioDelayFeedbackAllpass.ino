/*  Example of allpass interpolation for fractional delays,
    using Mozzi sonification library.
  
    Demonstrates AudioDelayFeedback with allpass interpolation,
    random delaytimes and feedback levels on a 
    (random) percussive adsr whitenoise sound.
  
    Circuit: Audio output on digital pin 9 (on a Uno or similar), or 
    check the README or http://sensorium.github.com/Mozzi/
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2012-13.
    This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <AudioDelayFeedback.h>
#include <Ead.h> // exponential attack decay
#include <EventDelay.h>
#include <mozzi_rand.h>
#include <tables/whitenoise8192_int8.h>

#define CONTROL_RATE 128 // powers of 2 please

Oscil <WHITENOISE8192_NUM_CELLS, AUDIO_RATE> aNoise(WHITENOISE8192_DATA); // audio noise
EventDelay kDelay; // for triggering envelope start
Ead kEnvelope(CONTROL_RATE); // resolution will be CONTROL_RATE

AudioDelayFeedback <256, ALLPASS> aDel;

int gain;


void setup(){
  //Serial.begin(115200);
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
    // set random adsr parameters
    unsigned int duration = rand(500u)+200;
    unsigned int attack = rand(75);
    unsigned int decay = duration - attack;
    kEnvelope.start(attack,decay);
    
    // set random delay parameters
    float del_cells = (float)rand(65535u)/256;
    aDel.setDelayTimeCells(del_cells); // Allpass interpolation for fractional delay time
    char fb = rand(-50,50);
    aDel.setFeedbackLevel(fb);
    kDelay.start(duration+500);
  }
  gain = (int) kEnvelope.next();
}


int updateAudio(){
  return aDel.next((gain*aNoise.next())>>8);
}


void loop(){
  audioHook();
}




