/*  Example of modulating a signal by using a variable delay,
    using Mozzi sonification library.

    Demonstrates AudioDelay.

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

#define MOZZI_CONTROL_RATE 256 // Hz, powers of 2 are most reliable
#include <Mozzi.h>
#include <Oscil.h>
#include <tables/triangle_analogue512_int8.h> // wavetable
#include <tables/cos2048_int8.h> // wavetable
#include <AudioDelay.h>
#include <mozzi_midi.h> // for mtof

Oscil<TRIANGLE_ANALOGUE512_NUM_CELLS, MOZZI_AUDIO_RATE> aTriangle(TRIANGLE_ANALOGUE512_DATA);
Oscil<COS2048_NUM_CELLS, MOZZI_CONTROL_RATE> kFreq(COS2048_DATA);

AudioDelay <256> aDel;
int del_samps;

void setup(){
  aTriangle.setFreq(mtof(60.f));
  kFreq.setFreq(.63f);
  startMozzi();
}

void loop(){
  audioHook();
}

void updateControl(){
  del_samps = 128+kFreq.next();
}

AudioOutput updateAudio(){
  int8_t asig = aDel.next(aTriangle.next(), del_samps);
  return MonoOutput::from8Bit(asig);
}
