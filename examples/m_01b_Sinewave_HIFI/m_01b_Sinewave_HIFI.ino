/*  Example playing a sinewave at a set frequency,
 *  using Mozzi sonification library.
 *
 *  Demonstrates the use of Oscil to play a wavetable.
 *
 *  IMPORTANT: this sketch requires Mozzi/mozzi_config.h to be
 *  be changed from STANDARD mode to HIFI.
 *  In Mozz/mozzi_config.h, change
 *  #define AUDIO_MODE STANDARD
 *  //#define AUDIO_MODE HIFI
 *  to
 *  //#define AUDIO_MODE STANDARD
 *  #define AUDIO_MODE HIFI
 *
 *
 *  Circuit: Audio output on digital pin 9 and 10 (on a Uno or similar),
 *  Check the Mozzi core module documentation for others and more detail
 *
 *                   3.9k
 *   pin 9  ---WWWW-----|-----output
 *                     1M           |
 *   pin 10 ---WWWW---- |
 *                                     |
 *                           4.7n  ==
 *                                     |
 *                                 ground
 *
 *
 *  Mozzi help/discussion/announcements:
 *  https://groups.google.com/forum/#!forum/mozzi-users
 *
 *  Tim Barrass 2012-13.
 *  This example code is in the public domain.
 */
 
#define AUDIO_MODE HIFI
#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator

// use: Oscil <table_size, update_rate> oscilName (wavetable)
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

void setup(){
  startMozzi(); // uses the default control rate of 64, defined in mozzi_config.h
  aSin.setFreq(440); // set the frequency
}


void updateControl(){}


int updateAudio(){
  // this would make more sense with a higher resolution signal
  // but still benefits from using HIFI to avoid the 16kHz pwm noise
  return aSin.next()<<6; // 8 bits scaled up to 14 bits
}


void loop(){
  audioHook(); // required here
}
