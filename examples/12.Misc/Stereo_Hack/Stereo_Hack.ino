/*  Example crudely panning noise to test stereo output hack,
 *  using Mozzi sonification library.
 *
 *  Tests stereo output.  This requires #define AUDIO_CHANNELS STEREO in mozzi_config.h
 *
 *  Circuit: Audio outputs on digital pins 9 and 10.
 *
 *  		Mozzi help/discussion/announcements:
 *  https://groups.google.com/forum/#!forum/mozzi-users
 *
 *  Tim Barrass 2012.
 *  This example code is in the public domain.
 */

#include <MozziGuts.h>
#include <Phasor.h> // for controlling panning position
#include <Oscil.h> // oscil for audio sig
#include <tables/pinknoise8192_int8.h> // table for oscillator

// use: Oscil <table_size, update_rate> oscilName (wavetable)
Oscil <PINKNOISE8192_NUM_CELLS, AUDIO_RATE> aNoise(PINKNOISE8192_DATA);

//Phasor for panning
Phasor <CONTROL_RATE> kPan; // outputs an unsigned long 0-max 32 bit positive number
unsigned int pan; // convey pan from updateControl() to updateAudioStereo();


void setup(){
  aNoise.setFreq(2.111f); // set the frequency with an unsigned int or a float
  kPan.setFreq(0.25f); // take 4 seconds to move left-right
  startMozzi(); // :)
  Serial.begin(115200);
}


void updateControl(){
  pan = kPan.next()>>16;
  Serial.println(pan);
}

AudioOutput_t updateAudio(){
  int asig = aNoise.next();
  return StereoOutput::fromNBit(24, (long)pan*asig, ((long)65535-pan)*asig);
}


void loop(){
  audioHook(); // required here
}
