/*  Example of allpass interpolation for fractional delays,
    using Mozzi sonification library.

    Demonstrates AudioDelayFeedback with allpass interpolation,
    random delaytimes and feedback levels on a
    (random) percussive adsr whitenoise sound.

    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.io/Mozzi/

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2013-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#define MOZZI_CONTROL_RATE 128 // Hz, powers of 2 are most reliable
#include <Mozzi.h>
#include <Oscil.h>
#include <AudioDelayFeedback.h>
#include <Ead.h> // exponential attack decay
#include <EventDelay.h>
#include <mozzi_rand.h>
#include <tables/whitenoise8192_int8.h>

Oscil <WHITENOISE8192_NUM_CELLS, MOZZI_AUDIO_RATE> aNoise(WHITENOISE8192_DATA); // audio noise
EventDelay kDelay; // for triggering envelope start
Ead kEnvelope(MOZZI_CONTROL_RATE); // resolution will be MOZZI_CONTROL_RATE

AudioDelayFeedback <256, ALLPASS> aDel;

int gain;


void setup(){
  //Serial.begin(9600);
  startMozzi();
  randSeed(); // reseed the random generator for different results each time the sketch runs
  // use float to set freq because it will be small and fractional
  aNoise.setFreq((float)MOZZI_AUDIO_RATE/WHITENOISE8192_SAMPLERATE);
  kDelay.start(1000);
}


void updateControl(){
  // jump around in audio noise table to disrupt obvious looping
  aNoise.setPhase(rand((unsigned int)WHITENOISE8192_NUM_CELLS));

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


AudioOutput updateAudio(){
  return MonoOutput::from8Bit(aDel.next((gain*aNoise.next())>>8));
}


void loop(){
  audioHook();
}
