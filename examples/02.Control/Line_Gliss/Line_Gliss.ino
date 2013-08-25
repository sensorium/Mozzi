/*  Example of sliding smoothly
    between oscillator frequencies,
    using Mozzi sonification library.
  
    Demonstrates using a Line to efficiently change the
    frequency of an oscillator at audio rate.   Calculating
    a new frequency for every step of a slide is a lot to
    do for every single sample, so instead this sketch works out the 
    start and end frequencies for each control period and
    the phase increments (size of the steps through the sound table)
    required for the audio oscillator to generate those frequencies.
    Then, a computationally cheap Line() is used to slide between the
    different phase increments smoothly at audio rate.
  
    Circuit: Audio output on digital pin 9 (on a Uno or similar), or 
    check the README or http://sensorium.github.com/Mozzi/
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2012.
    This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Line.h> // for smooth transitions
#include <Oscil.h> // oscillator template
#include <tables/triangle_warm8192_int8.h> // triangle table for oscillator
#include <mozzi_midi.h>

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> aTriangle(TRIANGLE_WARM8192_DATA);

// use: Line <type> lineName
Line <long> aGliss;

#define CONTROL_RATE 64 // powers of 2 please

unsigned char lo_note = 24; // midi note numbers
unsigned char hi_note = 36;

long audio_steps_per_gliss = AUDIO_RATE / 4; // ie. 4 glisses per second
long control_steps_per_gliss = CONTROL_RATE / 4;

// stuff for changing starting positions, probably just confusing really
int counter = 0;
unsigned char gliss_offset = 0;
unsigned char gliss_offset_step = 2;
unsigned char  gliss_offset_max = 36;


void setup(){
  startMozzi(CONTROL_RATE);
}


void loop(){
  audioHook();
}


void updateControl(){
  if (--counter <= 0){
    
    // start at a new note
    gliss_offset += gliss_offset_step;
    if(gliss_offset >= gliss_offset_max) gliss_offset=0;
    
    // only need to calculate frequencies once each control update
    int start_freq = mtof(lo_note+gliss_offset);
    int end_freq = mtof(hi_note+gliss_offset);
    
    // find the phase increments (step sizes) through the audio table for those freqs
    // they are big ugly numbers which the oscillator understands but you don't really want to know
    long gliss_start = aTriangle.phaseIncFromFreq(start_freq);
    long gliss_end = aTriangle.phaseIncFromFreq(end_freq);
    
    // set the audio rate line to transition between the different step sizes
    aGliss.set(gliss_start, gliss_end, audio_steps_per_gliss);
    
    counter = control_steps_per_gliss;
  }
}


int updateAudio(){
  aTriangle.setPhaseInc(aGliss.next());
  return aTriangle.next();
}
