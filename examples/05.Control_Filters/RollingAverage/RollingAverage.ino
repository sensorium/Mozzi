/*  Example changing the frequency of 2 sinewaves with 1 analog input,
    using RollingAverage() to filter one of the control signals 
    and the other one unfiltered,
    using Mozzi sonification library.
  
    Demonstrates the difference between a raw control
    signal and one smoothed with RollingAverage().
  
    Circuit: Audio output on digital pin 9 (on a Uno or similar), or 
    check the README or http://sensorium.github.com/Mozzi/
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2013.
    This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <RollingAverage.h>

#define INPUT_PIN 0 // analog control input

// oscils to compare bumpy to averaged control input
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin0(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin1(SIN2048_DATA);

// use: RollingAverage <number_type, how_many_to_average> myThing
RollingAverage <int, 32> kAverage; // how_many_to_average has to be power of 2
int averaged;

void setup(){
  Serial.begin(115200);
  startMozzi();
}


void updateControl(){
  int bumpy_input = mozziAnalogRead(INPUT_PIN);
  averaged = kAverage.next(bumpy_input);

  Serial.print("bumpy \t");
  Serial.print(bumpy_input);
  Serial.print("\t averaged \t");
  Serial.println(averaged);

  aSin0.setFreq(bumpy_input);
  aSin1.setFreq(averaged);
}


int updateAudio(){
  return 3*(aSin0.next()+aSin1.next())>>2;
}


void loop(){
  audioHook();
}


