/*  Example sliding between frequencies,
    using Mozzi sonification library.

    Demonstrates using Smooth to filter a control signal.

    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.io/Mozzi/

		Mozzi documentation/API
		https://sensorium.github.io/Mozzi/doc/html/index.html

		Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2012, CC by-nc-sa.
*/

// this is a high value to avoid zipper noise
#define MOZZI_CONTROL_RATE 1280
#include <Mozzi.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <EventDelay.h>
#include <Smooth.h>
#include <mozzi_midi.h>

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin(SIN2048_DATA);

// for scheduling freq changes
EventDelay kFreqChangeDelay;

Smooth <int> kSmoothFreq(0.975f);
int target_freq, target_freq1, target_freq2;


void setup(){
  target_freq1 = 441;
  target_freq2 = 330;
  kFreqChangeDelay.set(1000); // 1000ms countdown, within resolution of MOZZI_CONTROL_RATE
  startMozzi();
}


void updateControl(){
  if(kFreqChangeDelay.ready()){
    if (target_freq == target_freq1) {
      target_freq = target_freq2;
    }
    else{
      target_freq = target_freq1;
    }
    kFreqChangeDelay.start();
  }

  int smoothed_freq = kSmoothFreq.next(target_freq);
  aSin.setFreq(smoothed_freq);
}


AudioOutput_t updateAudio(){
  return MonoOutput::from8Bit(aSin.next());
}


void loop(){
  audioHook();
}
