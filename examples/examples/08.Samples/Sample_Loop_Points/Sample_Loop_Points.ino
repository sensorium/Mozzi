/*  Example playing a looped sampled sound while 
    changing the start end end loop points, 
    and sliding the frequency around,
    using Mozzi sonification library.
  
    Demonstrates Sample(), looping with random 
    changes to loop and frequency parameters
    scheduled with EventDelay.
  
    Circuit: Audio output on digital pin 9 (on a Uno or similar), or 
    check the README or http://sensorium.github.com/Mozzi/
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2012.
    This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Sample.h> // Sample template
#include <samples/abomb16384_int8.h> // table for Sample
#include <EventDelay.h>
#include <mozzi_rand.h>

#define CONTROL_RATE 128

// use: Sample <table_size, update_rate> SampleName (wavetable)
Sample <ABOMB_NUM_CELLS, AUDIO_RATE> aSample(ABOMB_DATA);

// for scheduling changes
EventDelay  kTriggerDelay;


const float playspeed = 1.3;
float playspeedmod = 0;
float speedchange = 0;

const unsigned int full = (int) (1000.f/playspeed) - 23; // adjustment approx for CONTROL_RATE difference

void setup(){
  randSeed(); // reseed the random generator for different results each time the sketch runs
  aSample.setFreq(playspeed*((float) ABOMB_SAMPLERATE / (float) ABOMB_NUM_CELLS));
  kTriggerDelay.set(full); 
  aSample.setLoopingOn();
  startMozzi(CONTROL_RATE);
}


unsigned int chooseStart(){
  return rand((unsigned int)ABOMB_NUM_CELLS);
}


unsigned int chooseEnd(unsigned int startpos){
  return rand(startpos, (unsigned int) ABOMB_NUM_CELLS);
}


void chooseSpeedMod(){
  if (rand((unsigned char)3) == 0){
    speedchange = (float)rand((char)-100,(char)100)/8000;
    float startspeed = (float)rand((char)-100,(char)100)/100;
    playspeedmod = playspeed + startspeed;
  }
  else{
    speedchange = 0;
    playspeedmod = playspeed;
  }
}


void chooseStartEnd(){
  unsigned int s,e;
  if (rand((unsigned char)2) == 0){
    s = chooseStart();
    e = chooseEnd(s);
  }
  else{
    s = 0;
    e = ABOMB_NUM_CELLS;
  }
  aSample.setStart(s);
  aSample.setEnd(e);
}


void updateControl(){
  if(kTriggerDelay.ready()){
    chooseStartEnd();
    chooseSpeedMod();
    kTriggerDelay.start();
  }
  playspeedmod += speedchange;
  aSample.setFreq(playspeedmod);
}


int updateAudio(){
  return (int)aSample.next();
}


void loop(){
  audioHook();
}







