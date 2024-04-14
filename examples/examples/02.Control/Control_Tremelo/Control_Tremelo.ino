/*  Example of amplitude modulation (as tremelo),
    using Mozzi sonification library.

    Demonstrates audio and control rate updates.
    The tremelo oscillator is updated at control rate,
    and a Line is used to interpolate the control updates
    at audio rate, to remove zipper noise.
    A bit contrived and probably less efficient than just
    using an audio-rate tremelo oscillator, but hey it's a demo!

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

#define MOZZI_CONTROL_RATE 64 // Hz, powers of 2 are most reliable
#include <Mozzi.h>
#include <Oscil.h>
#include <tables/triangle_valve_2048_int8.h>
#include <tables/sin2048_int8.h>
#include <Line.h>
#include <mozzi_midi.h>

// audio oscillator
Oscil<TRIANGLE_VALVE_2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSig(TRIANGLE_VALVE_2048_DATA);
// control oscillator for tremelo
Oscil<SIN2048_NUM_CELLS, MOZZI_CONTROL_RATE> kTremelo(SIN2048_DATA);
// a line to interpolate control tremolo at audio rate
Line <unsigned int> aGain;


void setup(){
  aSig.setFreq(mtof(65.f));
  kTremelo.setFreq(5.5f);
  startMozzi();
}


void updateControl(){
  // gain shifted up to give enough range for line's internal steps
   unsigned int gain = (128u+kTremelo.next())<<8;
   aGain.set(gain, MOZZI_AUDIO_RATE / MOZZI_CONTROL_RATE); // divide of literals should get optimised away
}


AudioOutput updateAudio(){
  // cast to long before multiply to give room for intermediate result,
  // and also before shift,
  // to give consistent results for both 8 and 32 bit processors.
  return MonoOutput::fromNBit(24, (int32_t) aSig.next() * aGain.next()); // shifted back to audio range after multiply
}


void loop(){
  audioHook();
}
