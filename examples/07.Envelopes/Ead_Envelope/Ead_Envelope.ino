/* Example playing an enveloped noise source
   using Mozzi sonification library.

   Demonstrates Ead (exponential attack decay).

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

#define MOZZI_CONTROL_RATE 256 // Hz, powers of 2 are most reliable
#include <Mozzi.h>
#include <Oscil.h> // oscillator template
#include <tables/brownnoise8192_int8.h> // recorded audio wavetable
#include <Ead.h> // exponential attack decay
#include <EventDelay.h>
#include <mozzi_rand.h>

Oscil<BROWNNOISE8192_NUM_CELLS, MOZZI_AUDIO_RATE> aNoise(BROWNNOISE8192_DATA);
EventDelay kDelay; // for triggering envelope start
Ead kEnvelope(MOZZI_CONTROL_RATE); // resolution will be MOZZI_CONTROL_RATE

int gain;


void setup(){
  // use float to set freq because it will be small and fractional
  aNoise.setFreq((float)MOZZI_AUDIO_RATE/BROWNNOISE8192_SAMPLERATE);
  randSeed(); // fresh random, MUST be called before startMozzi - wierd bug
  startMozzi();
  kDelay.start(1000);
}


void updateControl(){
  // jump around in audio noise table to disrupt obvious looping
  aNoise.setPhase(rand((unsigned int)BROWNNOISE8192_NUM_CELLS));

  if(kDelay.ready()){
    // set random parameters
    unsigned int duration = rand(500u)+200;
    unsigned int attack = rand(75)+5; // +5 so the internal step size is more likely to be >0
    unsigned int decay = duration - attack;
    kEnvelope.start(attack,decay);
    kDelay.start(duration+500);
  }
  gain = (int) kEnvelope.next();
}


AudioOutput updateAudio(){
  return MonoOutput::from16Bit(gain*aNoise.next());
}


void loop(){
  audioHook(); // required here
}
