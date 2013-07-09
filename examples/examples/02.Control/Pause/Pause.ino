/*  Example pausing Mozzi, restoring timers to previous states,
 *  during which Arduino delay() works, then resuming Mozzi again.
 *  
 *  This may be useful when using sensors or other libraries which need to use
 *  the same timers as Mozzi.  (Timer 0, Timer 1, and in HIFI mode, Timer 2).
 *
 *  Mozzi help/discussion/announcements:
 *  https://groups.google.com/forum/#!forum/mozzi-users
 *
 *  Tim Barrass 2013.
 *  This example code is in the public domain.
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/brownnoise8192_int8.h> // recorded audio wavetable
#include <mozzi_analog.h>
#include <mozzi_rand.h>

Oscil<BROWNNOISE8192_NUM_CELLS, AUDIO_RATE> aCrunchySound(BROWNNOISE8192_DATA);

#define STOP_PIN A0

void setup(){
  aCrunchySound.setFreq(2.f);
  setupFastAnalogRead();
  startMozzi();
}


void updateControl(){
  if(analogRead(STOP_PIN)>512){
    pauseMozzi();
    delay(2000);
    unPauseMozzi();
  }
  // jump to a new spot in the noise table to stop it sounding like it repeats
  aCrunchySound.setPhase(rand(BROWNNOISE8192_NUM_CELLS));
}


int updateAudio(){
  return aCrunchySound.next();
}


void loop(){
  audioHook();
}




