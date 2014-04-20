/*  Example playing sine tables of different sizes
 with Mozzi sonification library.
 
 Demonstrates the audible quality of different length tables
 played with Oscil and scheduling with EventDelay.
 
 Mozzi help/discussion/announcements:
 https://groups.google.com/forum/#!forum/mozzi-users
 
 Tim Barrass 2012.
 This example code is in the public domain.
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/sin256_int8.h>
#include <tables/sin512_int8.h>
#include <tables/sin1024_int8.h>
#include <tables/sin2048_int8.h>
#include <tables/sin4096_int8.h>
#include <tables/sin8192_int8.h>
#include <EventDelay.h> // for scheduling events
#include <Line.h>

#define CONTROL_RATE 64

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN256_NUM_CELLS, AUDIO_RATE> aSin0(SIN256_DATA); // can hear significant aliasing noise
Oscil <SIN512_NUM_CELLS, AUDIO_RATE> aSin1(SIN512_DATA); // noise still there but less noticeable
Oscil <SIN1024_NUM_CELLS, AUDIO_RATE> aSin2(SIN1024_DATA); // borderline, hardly there if at all
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin3(SIN2048_DATA); // no audible improvement from here on
Oscil <SIN4096_NUM_CELLS, AUDIO_RATE> aSin4(SIN4096_DATA); // for 45 year old loud sound damaged ears
Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> aSin5(SIN8192_DATA);

EventDelay kWhoseTurnDelay;

const unsigned char NUM_OSCILS = 6;
unsigned char whose_turn = 0; // which oscil to listen to

// Line to sweep frequency at control rate
Line <float> kSweep;
const unsigned int MILLIS_PER_SWEEP = 2000;
const unsigned int MILLIS_PER_CONTROL = 1000u / CONTROL_RATE;
const unsigned long CONTROL_STEPS_PER_SWEEP = (unsigned long) MILLIS_PER_SWEEP / MILLIS_PER_CONTROL;

void setup(){
  startMozzi(CONTROL_RATE);
  kWhoseTurnDelay.set(MILLIS_PER_SWEEP);
  kSweep.set(0.f, 8192.f, CONTROL_STEPS_PER_SWEEP);
}


void updateControl(){

  if(kWhoseTurnDelay.ready()){
    if (++whose_turn >= NUM_OSCILS) whose_turn =0;
    kWhoseTurnDelay.start();
    kSweep.set(0UL);
  }

  float f = kSweep.next();

  switch (whose_turn) {
  case 0:
    aSin0.setFreq(f);
    break;
  case 1:
    aSin1.setFreq(f);
    break;
  case 2:
    aSin2.setFreq(f);
    break;
  case 3:
    aSin3.setFreq(f);
    break;
  case 4:
    aSin4.setFreq(f);
    break;
  case 5:
    aSin5.setFreq(f);
    break;
  }
}


int updateAudio(){
  int asig;
  switch (whose_turn) {
  case 0:
    asig = aSin0.next();
    break;
  case 1:
    asig = aSin1.next();
    break;
  case 2:
    asig = aSin2.next();
    break;
  case 3:
    asig = aSin3.next();
    break;
  case 4:
    asig = aSin4.next();
    break;
  case 5:
    asig = aSin5.next();
    break;
  }
  return asig;
}


void loop(){
  audioHook(); // required here
}







