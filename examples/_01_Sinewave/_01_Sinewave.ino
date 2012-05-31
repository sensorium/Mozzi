/*  Example playing a sinewave at a set frequency,
 *  using Cuttlefish sonification library.
 *
 *  Demonstrates the use of Oscil to play a wavetable.
 *  
 *  Circuit: Audio output on digital pin 9.
 *
 *  sweatsonics@sweatsonics.com 2012.
 *  This example code is in the public domain.
 */

#include <CuttlefishGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin8192_int8.h> // sine table for oscillator

// use: Oscil <table_size, update_rate> oscilName (wavetable)
Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> aSin(SIN8192_DATA);

// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 64 // powers of 2 please

void setup(){
  startCuttlefish(CONTROL_RATE); // set a control rate of 64 (powers of 2 please)
  aSin.setFreq(440u); // set the frequency with an unsigned int or a float
}


void updateControl(){
  // put changing controls in here
} 


int updateAudio(){
  return aSin.next(); // return an int signal centred around 0
}


void loop(){
  audioHook(); // required here
}



