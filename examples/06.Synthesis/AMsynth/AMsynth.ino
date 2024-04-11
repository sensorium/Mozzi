/*  Example of Amplitude Modulation synthesis
    using Mozzi sonification library.

    Demonstrates modulating the gain of one oscillator
    by the instantaneous amplitude of another,
    shows the use of fixed-point numbers to express fractional
    values, random numbers with rand(), and EventDelay()
    for scheduling.

    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.io/Mozzi/

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2013-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/


#define MOZZI_CONTROL_RATE 64  // Hz, powers of 2 are most reliable
#include <Mozzi.h>
#include <Oscil.h>
#include <tables/cos2048_int8.h>  // table for Oscils to play
#include <EventDelay.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Smooth.h>


// audio oscils
Oscil<COS2048_NUM_CELLS, MOZZI_AUDIO_RATE> aCarrier(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, MOZZI_AUDIO_RATE> aModulator(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, MOZZI_AUDIO_RATE> aModDepth(COS2048_DATA);

// for scheduling note changes in updateControl()
EventDelay kNoteChangeDelay;

UFix<8, 8> ratio;          // unsigned int with 8 integer bits and 8 fractional bits
UFix<24, 8> carrier_freq;  // unsigned long with 24 integer bits and 8 fractional bits

// for random notes
const UFix<7, 0> octave_start_note = 42;

void setup() {
  ratio = 3;
  kNoteChangeDelay.set(200);  // note duration ms, within resolution of MOZZI_CONTROL_RATE
  aModDepth.setFreq(13.f);    // vary mod depth to highlight am effects
  randSeed();                 // reseed the random generator for different results each time the sketch runs
  startMozzi();
}


void updateControl() {
  static auto last_note = octave_start_note;


  if (kNoteChangeDelay.ready()) {
    // change octave now and then
    if (rand((byte)5) == 0) {
      last_note = UFix<7, 0>(36 + (rand((byte)6) * 12));
    }

    // change step up or down a semitone occasionally
    if (rand((byte)13) == 0) {
      last_note = last_note + SFix<7, 0>(1 - rand((byte)3));
    }

    // change modulation ratio now and then
    if (rand((byte)5) == 0) {
      ratio = 1 + rand((byte)5);
    }

    // sometimes add a fractionto the ratio
    if (rand((byte)5) == 0) {
      ratio = ratio + toUFraction(rand((byte)255));
    }

    // step up or down 3 semitones (or 0)
    last_note = last_note + SFix<7, 0>(3 * (1 - rand((byte)3)));

    // convert midi to frequency
    carrier_freq = mtof(last_note);

    // calculate modulation frequency to stay in ratio with carrier
    auto mod_freq = carrier_freq * ratio;

    // set frequencies of the oscillators
    aCarrier.setFreq(carrier_freq);
    aModulator.setFreq(mod_freq);

    // reset the note scheduler
    kNoteChangeDelay.start();
  }
}

AudioOutput updateAudio() {
  auto mod = UFix<8, 0>(128 + aModulator.next()) * UFix<8, 0>(128 + aModDepth.next());
  return MonoOutput::fromSFix(mod * toSFraction(aCarrier.next()));
}

void loop() {
  audioHook();
}
