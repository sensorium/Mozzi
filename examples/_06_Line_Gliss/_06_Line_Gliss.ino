/*  Example of sliding smoothly
 *  between oscillator frequencies,
 *  using Cuttlefish sonification library.
 *
 *  Demonstrates using a Line to change the
 *  frequency of an oscillator.
 *
 *  Circuit: Audio output on digital pin 9.
 *
 *  Tim Barrass 2012.
 *  This example code is in the public domain.
 */

#include <CuttlefishGuts.h>
#include <Line.h> // for smooth transitions
#include <Oscil.h> // oscillator template
#include <tables/triangle_warm8192_int8.h> // triangle table for oscillator
#include <utils.h>
#include <utils.c>

// use: Oscil <table_size, update_rate> oscilName (wavetable)
Oscil <TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> aTriangle(TRIANGLE_WARM8192_DATA);

// use: Line <type> lineName
Line <long> aGliss;

#define CONTROL_RATE 64 // powers of 2 please
unsigned char lo_note = 24;
unsigned char hi_note = 36;
long audio_steps_per_gliss = AUDIO_RATE / 4; // ie. 4 glisses per second
long control_steps_per_gliss = CONTROL_RATE / 4;
int counter = 0;
unsigned char gliss_offset = 0;
unsigned char gliss_offset_step = 2;
unsigned char  gliss_offset_max = 36;


void setup(){
  startCuttlefish(CONTROL_RATE); // optional control rate parameter
}


void loop(){
  audioHook();
}


void updateControl(){
  if (--counter <= 0){
    gliss_offset += gliss_offset_step;
    if(gliss_offset >= gliss_offset_max) gliss_offset=0;
    long gliss_start = aTriangle.phaseIncFromFreq(mtof(float(lo_note+gliss_offset)));
    long gliss_end = aTriangle.phaseIncFromFreq(mtof(float(hi_note+gliss_offset)));
    aGliss.set(gliss_start, gliss_end, audio_steps_per_gliss);
    counter = control_steps_per_gliss;
  }
}


int updateAudio(){
  aTriangle.setPhaseInc(aGliss.next());
  return aTriangle.next();
}
