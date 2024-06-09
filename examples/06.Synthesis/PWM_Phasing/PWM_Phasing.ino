/*  Example of pulse width modulation,
    using Mozzi sonification library.

    Based Miller Puckette's j03.pulse.width.mod example
    in the Pure Data documentation, subtracting 2 sawtooth
    waves with slightly different tunings to produce a
    varying phase difference.

    Demonstrates Phasor().

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
#include <Phasor.h>

Phasor <MOZZI_AUDIO_RATE> aPhasor1;
Phasor <MOZZI_AUDIO_RATE> aPhasor2;

float freq = 55.f;

void setup(){
  aPhasor1.setFreq(freq);
  aPhasor2.setFreq(freq+0.2f);
  startMozzi(); // :)
}


void updateControl(){
}


AudioOutput updateAudio(){
  char asig1 = (char)(aPhasor1.next()>>24);
  char asig2 = (char)(aPhasor2.next()>>24);
  return MonoOutput::fromNBit(9, ((int)asig1-asig2));
}


void loop(){
  audioHook(); // required here
}
