/*  Example of playing sampled sounds,
    using Mozzi sonification library.

    Demonstrates one-shot samples scheduled
    with EventDelay(), and fast random numbers with
    xorshift96() and rand(), a more friendly wrapper for xorshift96().

    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.io/Mozzi/

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2012-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#include <Mozzi.h>
#include <Sample.h> // Sample template
#include <samples/bamboo/bamboo_00_2048_int8.h> // wavetable data
#include <samples/bamboo/bamboo_01_2048_int8.h> // wavetable data
#include <samples/bamboo/bamboo_02_2048_int8.h> // wavetable data
#include <EventDelay.h>
#include <mozzi_rand.h>

// use: Sample <table_size, update_rate> SampleName (wavetable)
Sample <BAMBOO_00_2048_NUM_CELLS, MOZZI_AUDIO_RATE>aBamboo0(BAMBOO_00_2048_DATA);
Sample <BAMBOO_01_2048_NUM_CELLS, MOZZI_AUDIO_RATE>aBamboo1(BAMBOO_01_2048_DATA);
Sample <BAMBOO_02_2048_NUM_CELLS, MOZZI_AUDIO_RATE>aBamboo2(BAMBOO_02_2048_DATA);

// for scheduling audio gain changes
EventDelay kTriggerDelay;

void setup(){
  startMozzi();
  aBamboo0.setFreq((float) BAMBOO_00_2048_SAMPLERATE / (float) BAMBOO_00_2048_NUM_CELLS); // play at the speed it was recorded at
  aBamboo1.setFreq((float) BAMBOO_01_2048_SAMPLERATE / (float) BAMBOO_01_2048_NUM_CELLS);
  aBamboo2.setFreq((float) BAMBOO_02_2048_SAMPLERATE / (float) BAMBOO_02_2048_NUM_CELLS);
  kTriggerDelay.set(111); // countdown ms, within resolution of MOZZI_CONTROL_RATE
}


byte randomGain(){
  //return lowByte(xorshift96())<<1;
  return rand(200) + 55;
}

// referencing members from a struct is meant to be a bit faster than seperately
// ....haven't actually tested it here...
struct gainstruct{
  byte gain0;
  byte gain1;
  byte gain2;
}
gains;


void updateControl(){
  if(kTriggerDelay.ready()){
    switch(rand(0, 3)) {
    case 0:
      gains.gain0 = randomGain();
      aBamboo0.start();
      break;
    case 1:
      gains.gain1 = randomGain();
      aBamboo1.start();
      break;
    case 2:
      gains.gain2 = randomGain();
      aBamboo2.start();
      break;
    }
    kTriggerDelay.start();
  }
}


AudioOutput updateAudio(){
  int asig= (int)
    ((long) aBamboo0.next()*gains.gain0 +
      aBamboo1.next()*gains.gain1 +
      aBamboo2.next()*gains.gain2)>>4;
  // clip to keep sample loud but still in range
  return MonoOutput::fromAlmostNBit(9, asig).clip();
}


void loop(){
  audioHook();
}
