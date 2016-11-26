/*  Example crudely panning noise to test stereo output hack,
 *  using Mozzi sonification library.
 *
 *  Tests stereo output hack.  This requires #define STEREO_HACK true in mozzi_config.h
 *
 *  Circuit: Audio outputs on digital pins 9 and 10.
 *
 *  Mozzi help/discussion/announcements:
 *  https://groups.google.com/forum/#!forum/mozzi-users
 *
 *  Tim Barrass 2012.
 *  This example code is in the public domain.
 */

#include <MozziGutsStereo.h>
#include <Phasor.h> // for controlling panning position
#include <Oscil.h> // oscil for audio sig
#include <tables/pinknoise8192_int8.h> // table for oscillator


// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 64 // powers of 2 please

// use: Oscil <table_size, update_rate> oscilName (wavetable)
Oscil <PINKNOISE8192_NUM_CELLS, AUDIO_RATE> aNoise(PINKNOISE8192_DATA);

//Phasor for panning
Phasor <unsigned int, CONTROL_RATE> kPan;
unsigned int pan; // convey pan from updateControl() to updateAudioStereo();


void setup(){
  aNoise.setFreq(2.111f); // set the frequency with an unsigned int or a float
  kPan.setFreq(0.25f); // take 4 seconds to move left-right
  startMozzi(CONTROL_RATE); // set a control rate of 64 (powers of 2 please)
}


void updateControl(){
  pan = kPan.next();
}


int audio_signal_1, audio_signal_2; // required for the stereo hack

void updateAudioStereo(){
  int asig = aNoise.next();
  audio_signal_1 = (int)(((long)pan*asig)>>16);
  audio_signal_2 = (int)((((long)65535-pan)*asig)>>16);
}


void loop(){
  audioHook(); // required here
}




