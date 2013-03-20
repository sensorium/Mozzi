/*  Example of a pair of detuned oscillators 
 *  sliding smoothly between frequencies,
 *  using Mozzi sonification library.
 *
 *  Demonstrates using Lines to efficiently change the
 *  frequency of each oscillator at audio rate.   Calculating
 *  a new frequency for every step of a slide is a lot to
 *  do for every single sample, so instead this sketch works out the 
 *  start and end frequencies for each control period and
 *  the phase increments (size of the steps through the sound table)
 *  required for the audio oscillators to generate those frequencies.
 *  Then, computationally cheap Lines are used to slide between the
 *  different phase increments smoothly at audio rate.
 *  Also shows how to use random offsets between the oscillators'
 *  frequencies to produce a chorusing/doubling effect.
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
#include <Line.h> // for smooth transitions
#include <Oscil.h> // oscillator template
#include <tables/triangle_warm8192_int8.h> // triangle table for oscillator
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <mozzi_fixmath.h>


// use: Oscil <table_size, update_rate> oscilName (wavetable)
Oscil <TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> aTriangle1(TRIANGLE_WARM8192_DATA);
Oscil <TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> aTriangle2(TRIANGLE_WARM8192_DATA);


// use: Line <type> lineName
Line <long> aGliss1;
Line <long> aGliss2;

#define CONTROL_RATE 64 // powers of 2 please

unsigned char lo_note = 24; // midi note numbers
unsigned char hi_note = 46;

long audio_steps_per_gliss = AUDIO_RATE / 4; // ie. 4 glisses per second
long control_steps_per_gliss = CONTROL_RATE / 4;

// stuff for changing starting positions, probably just confusing really
int counter = 0;
unsigned char gliss_offset = 0;
unsigned char gliss_offset_step = 2;
unsigned char  gliss_offset_max = 36;


void setup() {
  randSeed();
  pinMode(0,OUTPUT); // without this, updateControl() gets interrupted ........??
  startMozzi(CONTROL_RATE); // optional control rate parameter
}


void loop(){
  audioHook();
}


// variation between oscillator's phase increments
// looks like a big number, but they are high precision 
// and this is just like a fractional part
long variation(){
  return rand(16383); 
}

void updateControl(){ // 900 us floats vs 160 fixed
  if (--counter <= 0){
    
    // start at a new note
    gliss_offset += gliss_offset_step;
    
    if(gliss_offset >= gliss_offset_max) {
      gliss_offset=0;
      
      // this is just confusing, you should ignore it!
      // alternate between 2 and 3 steps per start note each time a new "run" starts
      if(gliss_offset_step != 2){
        gliss_offset_step = 2;
      }
      else{
        gliss_offset_step = 3;
      }
    }
    
    // only need to calculate frequencies once each control update
    int start_freq = mtof(lo_note+gliss_offset);
    int end_freq = mtof(hi_note+gliss_offset);
    
    // find the phase increments (step sizes) through the audio table for those freqs
    // they are big ugly numbers which the oscillator understands but you don't really want to know
    long gliss_start = aTriangle1.phaseIncFromFreq(start_freq);
    long gliss_end = aTriangle2.phaseIncFromFreq(end_freq);
    
    aGliss1.set(gliss_start, gliss_end, audio_steps_per_gliss);
    aGliss2.set(gliss_start+(variation()*gliss_offset), gliss_end+(variation()*gliss_offset), audio_steps_per_gliss);
    counter = control_steps_per_gliss;
  }
}


int updateAudio(){
  aTriangle1.setPhaseInc(aGliss1.next());
  aTriangle2.setPhaseInc(aGliss2.next());
  return ((int)aTriangle1.next()+aTriangle2.next())/2;
}

