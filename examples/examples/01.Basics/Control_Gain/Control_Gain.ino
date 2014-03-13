/*  Example changing the gain of a sinewave,
    using Mozzi sonification library.
  
    Demonstrates the use of a control variable to influence an
    audio signal.
  
    Circuit: Audio output on digital pin 9 (on a Uno or similar), or 
    check the README or http://sensorium.github.com/Mozzi/
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2012.
    This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

// control variable, use the smallest data size you can for anything used in audio
unsigned char gain = 255;


void setup(){
  startMozzi(); // start with default control rate of 64
  aSin.setFreq(3320); // set the frequency
}


void updateControl(){
  // as unsigned char, this will automatically roll around to 255 when it passes 0
  gain = gain - 3 ; 
}


int updateAudio(){
  return (aSin.next()* gain)>>8; // shift back to STANDARD audio range, like /256 but faster
}


void loop(){
  audioHook(); // required here
}



