/*  Example of a sound being toggled on an off,
    using Mozzi sonification library.
  
    Demonstrates scheduling with EventDelay.
    EventDelay is a way to make non-blocking
    time delays for events.  Use this instead of
    the Arduino delay() function, which doesn't
    work with Mozzi.
  
    Circuit: Audio output on digital pin 9 (on a Uno or similar), or 
    check the README or http://sensorium.github.com/Mozzi/
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2012.
    This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin8192_int8.h> // sine table for oscillator
#include <EventDelay.h>

#define CONTROL_RATE 64

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> aSin(SIN8192_DATA);

// for scheduling audio gain changes
EventDelay kGainChangeDelay;

char gain = 1;

void setup(){
  startMozzi(CONTROL_RATE);
  aSin.setFreq(330); // set the frequency
  kGainChangeDelay.set(1000); // 1 second countdown, within resolution of CONTROL_RATE
}


void updateControl(){
  if(kGainChangeDelay.ready()){
    gain = 1-gain; // flip 0/1
    kGainChangeDelay.start();
  }
}


int updateAudio(){
  return aSin.next()*gain;
}


void loop(){
  audioHook();
}



