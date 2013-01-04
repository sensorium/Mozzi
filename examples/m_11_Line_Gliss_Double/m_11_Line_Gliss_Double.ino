/*  Example of sliding smoothly
 *  between oscillator frequencies,
 *  using Mozzi sonification library.
 *
 *  Demonstrates using a Line to change the
 *  frequency of an oscillator.
 *
 *  Circuit: Audio output on digital pin 9.
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
#include <utils.h>
#include <fixedMath.h>

// use: Oscil <table_size, update_rate> oscilName (wavetable)
Oscil <TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> aTriangle1(TRIANGLE_WARM8192_DATA);
Oscil <TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> aTriangle2(TRIANGLE_WARM8192_DATA);


// use: Line <type> lineName
Line <long> aGliss1;
Line <long> aGliss2;

#define CONTROL_RATE 64 // powers of 2 please

unsigned char lo_note = 24;
unsigned char hi_note = 46;
long audio_steps_per_gliss = AUDIO_RATE / 4; // ie. 4 glisses per second
long control_steps_per_gliss = CONTROL_RATE / 4;
int counter = 0;
unsigned char gliss_offset = 0;
unsigned char gliss_offset_step = 2;
unsigned char  gliss_offset_max = 36;


void setup() {
  SET_PIN13_OUT;
  xorshiftSeed((unsigned long)analogRead(0));
  pinMode(0,OUTPUT); // without this, updateControl() gets interrupted ........??
  startMozzi(CONTROL_RATE); // optional control rate parameter
}


void loop(){
  audioHook();
}

long variation(){
  //return ((float)rand((unsigned char)100))/16;
  return xorshift96()>>18;
  //return aTriangle1.phaseIncFromFreq(Q16n16_to_Q16n0(Q16n16_mtof(xorshift96()>>16)));
}

void updateControl(){ // 900 us floats vs 160 fixed
  if (--counter <= 0){
    SET_PIN13_HIGH;

    gliss_offset += gliss_offset_step;
    
    if(gliss_offset >= gliss_offset_max) {
      gliss_offset=0;
      
      if(gliss_offset_step != 2){
        gliss_offset_step = 2;
      }
      else{
        gliss_offset_step = 3;
      }
    }
    
    //long gliss_start = aTriangle1.phaseIncFromFreq(mtof(float(lo_note+gliss_offset)));
    Q16n16 gliss_start = aTriangle1.phaseIncFromFreq(Q16n16_to_Q16n0(Q16n16_mtof(Q8n0_to_Q16n16(lo_note+gliss_offset))));
    //long gliss_end = aTriangle1.phaseIncFromFreq(mtof(float(hi_note+gliss_offset)));
    Q16n16 gliss_end = aTriangle1.phaseIncFromFreq(Q16n16_to_Q16n0(Q16n16_mtof(Q8n0_to_Q16n16(hi_note+gliss_offset))));
    aGliss1.set(gliss_start, gliss_end, audio_steps_per_gliss);
    aGliss2.set(gliss_start+(variation()*gliss_offset), gliss_end+(variation()*gliss_offset), audio_steps_per_gliss);
    counter = control_steps_per_gliss;
    SET_PIN13_LOW;
  }
}


int updateAudio(){
  aTriangle1.setPhaseInc(aGliss1.next());
  aTriangle2.setPhaseInc(aGliss2.next());
  return ((int)aTriangle1.next()+aTriangle2.next())/2;
}

