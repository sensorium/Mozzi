/*  Example playing a sinewave with vibrato,
    using Mozzi sonification library.

    Demonstrates simple FM using phase modulation.

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
#include <tables/cos2048_int8.h> // table for Oscils to play
#include <mozzi_midi.h> // for mtof
//#include <mozzi_fixmath.h>
#include <FixMath.h> // Fixed point arithmetics

Oscil<COS2048_NUM_CELLS, MOZZI_AUDIO_RATE> aCos(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, MOZZI_AUDIO_RATE> aVibrato(COS2048_DATA);

//const byte intensity = 255;
const UFix<0,8> intensity = 0.5; // amplitude of the phase modulation
                                     // 0.5 leads to a modulation of half the
                                     // wavetable

void setup(){
  startMozzi();
  aCos.setFreq(mtof(84.f));
  aVibrato.setFreq(15.f);
}


void updateControl(){
}

AudioOutput updateAudio(){
  //Q15n16 vibrato = (Q15n16) intensity * aVibrato.next();
  auto vibrato = intensity * toSFraction(aVibrato.next()); // Oscils in Mozzi are 8bits: 7bits of signal plus one bit of sign, so what they fit into a SFixMath<0,7> which is a signed fixMath type with 7 bits of value. 
  return MonoOutput::from8Bit(aCos.phMod(vibrato)); // phase modulation to modulate frequency
}

void loop(){
  audioHook();
}
