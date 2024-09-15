/*  Example applying an ADSR envelope to an audio filter
    with Mozzi sonification library.  This shows
    how to use an ADSR which updates at MOZZI_AUDIO_RATE, in updateAudio(),
    and output using next() at MOZZI_AUDIO_RATE in updateAudio().

    Demonstrates a simple ADSR object being controlled with
    noteOn() and controlling the cutoff frequency of a LowPassFilter.

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#include <Mozzi.h>
#include <Oscil.h>
#include <EventDelay.h>
#include <ADSR.h>
#include <ResonantFilter.h>
#include <tables/saw2048_int8.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>

Oscil<SAW2048_NUM_CELLS, MOZZI_AUDIO_RATE> aOscil(SAW2048_DATA);


// for triggering the envelope
EventDelay noteDelay;

ADSR<MOZZI_AUDIO_RATE, MOZZI_AUDIO_RATE> envelope;

// LowPass filter controlled by the envelope
LowPassFilter lpf;


void setup() {
  randSeed();  // fresh random
  envelope.setADLevels(128, 64);
  envelope.setTimes(2000, 1500, 250, 1250);  // always the same envelope times
  noteDelay.set(5000);                       // 5 second countdown
  lpf.setResonance(230);
  startMozzi();
}


void updateControl() {
  if (noteDelay.ready()) {
    envelope.noteOn(true);
    byte midi_note = rand(50) + 20;
    aOscil.setFreq((int)mtof(midi_note));
    noteDelay.start(5000);
  }
}


AudioOutput updateAudio() {
  envelope.update();
  // change the cutoff frequency with the envelope.
  lpf.setCutoffFreq(envelope.next());
  // the signal returned by aOscil is 8 bits, but with this much resonance the filtered signal goes higher,
  // so we let 1 bit of headroom, and clip in case it goes beyond...
  return MonoOutput::fromNBit(9, (int)(lpf.next(aOscil.next()))).clip();
}


void loop() {
  audioHook();  // required here
}