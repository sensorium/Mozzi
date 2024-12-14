/*  Example of playing a sampled sound,
    using Mozzi sonification library.

    Demonstrates one-shot samples scheduled
    with EventDelay.

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
#include <samples/burroughs1_18649_int8.h>
#include <EventDelay.h>

// use: Sample <table_size, update_rate> SampleName (wavetable)
Sample <BURROUGHS1_18649_NUM_CELLS, MOZZI_AUDIO_RATE> aSample(BURROUGHS1_18649_DATA);

// for scheduling sample start
EventDelay kTriggerDelay;

void setup(){
  startMozzi();
  aSample.setFreq((float) BURROUGHS1_18649_SAMPLERATE / (float) BURROUGHS1_18649_NUM_CELLS); // play at the speed it was recorded
  kTriggerDelay.set(1500); // 1500 msec countdown, within resolution of MOZZI_CONTROL_RATE
}


void updateControl(){
  if(kTriggerDelay.ready()){
    aSample.start();
    kTriggerDelay.start();
  }
}


AudioOutput updateAudio(){
  return MonoOutput::from8Bit((int) aSample.next());
}


void loop(){
  audioHook();
}
