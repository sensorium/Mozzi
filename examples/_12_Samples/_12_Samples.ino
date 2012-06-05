/*  Example of playing sampled sounds,
 *  using Cuttlefish sonification library.
 *
 *  Demonstrates one-shot samples scheduled
 *  with DelayCuttlefish, and use of xorshift96(),
 *  a random number generator.
 *
 *  Circuit: Audio output on digital pin 9.
 *
 *  unbackwards@gmail.com 2012.
 *  This example code is in the public domain.
 */

#include <CuttlefishGuts.h>
#include <Sample.h> // Sample template
#include <tables/bamboo1_1024_int8.h> // wavetable data
#include <tables/bamboo2_1024_int8.h> // wavetable data
#include <tables/bamboo3_2048_int8.h> // wavetable data
#include <DelayCuttlefish.h>
#include <utils.c> // for xorshift96() random number generator

#define CONTROL_RATE 64

// use: Sample <table_size, UPDATE_RATE> SampleName (wavetable)
Sample <BAMBOO1_1024_NUM_TABLE_CELLS, AUDIO_RATE> aBamboo1(BAMBOO1_1024_DATA);
Sample <BAMBOO2_1024_NUM_TABLE_CELLS, AUDIO_RATE> aBamboo2(BAMBOO2_1024_DATA);
Sample <BAMBOO3_2048_NUM_TABLE_CELLS, AUDIO_RATE> aBamboo3(BAMBOO3_2048_DATA);

// for scheduling audio gain changes
DelayCuttlefish kBoom(CONTROL_RATE);

void setup(){
  startCuttlefish(CONTROL_RATE);
  aBamboo1.setFreq((float) BAMBOO1_1024_SAMPLERATE / (float) BAMBOO1_1024_NUM_TABLE_CELLS); // play at the speed it was recorded at
  aBamboo2.setFreq((float) BAMBOO2_1024_SAMPLERATE / (float) BAMBOO2_1024_NUM_TABLE_CELLS);
  aBamboo3.setFreq((float) BAMBOO3_2048_SAMPLERATE / (float) BAMBOO3_2048_NUM_TABLE_CELLS);
  kBoom.set(111); // countdown ms, within resolution of CONTROL_RATE

}


unsigned char randomByte(){
  return lowByte(xorshift96());
}

unsigned char randomGain(){
  return lowByte(xorshift96())<<1;
}

struct gainstruct{
  unsigned char gain1;
  unsigned char gain2;
  unsigned char gain3;
}
gains;

void updateControl(){
  if(kBoom.ready()){
    switch(byteMod(lowByte(xorshift96()), 3)) {
    case 0:
      gains.gain1 = randomGain();
      aBamboo1.start();
      break;
    case 1:
      gains.gain2 = randomGain();
      aBamboo2.start();
      break;
    case 2:
      gains.gain3 = randomGain();
      aBamboo3.start();
      break;
    }
    kBoom.start();
  }
}


int updateAudio(){
  int asig= (int) ((long) aBamboo1.next()*gains.gain1 +
    aBamboo2.next()*gains.gain2 +
    aBamboo3.next()*gains.gain3)/16;
  //clip
  if (asig > 243) asig = 243;
  if (asig < -244) asig = -244;
  return asig;
}


void loop(){
  audioHook();
}











