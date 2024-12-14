/*  Example of Self Modulation synthesis,
    using Mozzi sonification library.

    Demonstrates the use of Oscil::phMod to modulate an Oscillator by itself.
    Self Modulation is a part of frequency modulation synthesis (FM). 
    Compared to "standard" FM where one oscillator modulates another
    in Self Modulation the output of one oscillator is used to modulate
    itself, directly, of after further modulation, or to modulate one of its
    modulator (looking at the DX7 diagram is probably clearer).

    Here we demonstrate the simple case of Self Modulation, one oscillator modulating himself.
    The equivalent diagram is:

       _____   
      \/    |
    |----|  |
    | 1  |  |
    |----|  |
      |_____|

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
#include <Oscil.h>                // oscillator template
#include <tables/sin2048_int8.h>  // sine table for oscillator
#include <EventDelay.h>
#include <Smooth.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>

EventDelay kModulationChangeDelay;  // to change the modulation amount
EventDelay kChangeNoteDelay;        // to change the base note
Smooth<uint8_t> kSmoothModulation(0.99f);
Oscil<SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin(SIN2048_DATA);

uint8_t self_mod_amount, smoothed_self_mod_amount;

UFix<8, 0> notes[4] = { 40 - 12, 52 - 12, 28 - 12, 30 - 12 };  // note played. Because of the Modulation the oscillator is called *two times* 
                                                              // hence produces a note is which an octave to high, so we compensate for that here (12 midi notes makes an octave).
  

void setup() {
  kModulationChangeDelay.set(2000);
  kChangeNoteDelay.set(300);
  startMozzi();                  // :)
  aSin.setFreq(mtof(notes[0]));  // set the frequency
}


void updateControl() {
  if (kModulationChangeDelay.ready()) {
    self_mod_amount = rand(255);  // next target value of modulation
    kModulationChangeDelay.start();
  }
  smoothed_self_mod_amount = kSmoothModulation(self_mod_amount); // smoothing of the modulation
  
  if (kChangeNoteDelay.ready()) {
    aSin.setFreq(mtof(notes[rand(4)]));
    kChangeNoteDelay.start();
  }
}


AudioOutput updateAudio() {
  return MonoOutput::from8Bit(aSin.phMod((int32_t(aSin.next()) * smoothed_self_mod_amount) << 4));
}


void loop() {
  audioHook();  // required here
}
