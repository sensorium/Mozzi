/* Example playing a sinewave with vibrato,
   using Mozzi sonification library.

   Demonstrates how to use fixed point arithmetics and midi notes to make an easy-to-adjust vibrato effect.
   This is probably less efficient than the Vibrato example, but is also easier to understand the amount
   of modulation

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2012-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#include <Mozzi.h>
#include <Oscil.h>
#include <tables/cos2048_int8.h>  // table for Oscils to play
#include <mozzi_midi.h>           // for mtof
#include <FixMath.h>              // Fixed point arithmetics

#define CONTROL_RATE 256  // Hz, powers of 2 are most reliable \
                          // we chose here an higher number than in the Vibrato example \
                          // because the modulation is performed at CONTROL_RATE

Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCos(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kVibrato(COS2048_DATA);

UFix<7, 0> midi_note = 65;
UFix<2, 1> mod_amplitude = 2.5;  // the amplitude of the vibrato, in semi-tones.
                                     // 2.5 can be represented with only 2 integer bits
                                     // and 1 fractionnal bit
                                     // hence UFix<2,1> is good enough to represent
                                     // that number. You can put numbers with decimals
                                     // or higher ones, but beware to adjust the number
                                     // of bits NI and NF of the UFix<NI,NF> accordingly.
                                     // It is always good to keep these as small as possible
                                     // for performances.


void setup() {
  startMozzi(CONTROL_RATE);
  aCos.setFreq(mtof(midi_note));
  kVibrato.setFreq(UFix<16, 16>(10));  // frequency of the modulation
}


void updateControl() {
  auto modulation = toSFraction(kVibrato.next()) * mod_amplitude;  // Oscil in Mozzi are fundamentally 8 bits: 7bits of data and 1bit of sign.
                                                                               // Here, we make it oscillate between nearly -1 and 1 (no integer bit).
                                                                               // The FixMath class will take care of making modulation the correct type
                                                                               // to preserve range and precision.
  aCos.setFreq(mtof(midi_note + modulation));                                  // changing the frequency of the main oscillator, adding the modulation as a midi note.
}


AudioOutput_t updateAudio() {
  return MonoOutput::from8Bit(aCos.next()); 
}

void loop() {
  audioHook();
}
