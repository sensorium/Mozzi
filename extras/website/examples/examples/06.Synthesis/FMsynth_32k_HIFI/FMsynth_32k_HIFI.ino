/*  Example of simple FM with the phase modulation technique,
    using Mozzi sonification library.

    Demonstrates Oscil::phMod() for phase modulation,
    Smooth() for smoothing control signals,
    and FixMath fixed point number types for fractional frequencies.

    This sketch using HIFI mode is not for Teensy 3.1.

    This sketch uses MOZZI_OUTPUT_2PIN_PWM (aka HIFI) output mode, which
    is not available on all boards (among others, it works on the
    classic Arduino boards, but not Teensy 3.x and friends).

    The MOZZI_AUDIO_RATE (sample rate) is additionally configured at 32768 Hz,
    which is the default on most platforms, but twice the standard rate on
    AVR-CPUs. Try chaging it back to hear the difference.

    Circuit: Audio output on digital pin 9 and 10 (on a Uno or similar),
    Check the Mozzi core module documentation for others and more detail

                     3.9k
     pin 9  ---WWWW-----|-----output
                    499k           |
     pin 10 ---WWWW---- |
                                       |
                             4.7n  ==
                                       |
                                   ground

    Resistors are ±0.5%  Measure and choose the most precise
    from a batch of whatever you can get.  Use two 1M resistors
    in parallel if you can't find 499k.
    Alternatively using 39 ohm, 4.99k and 470nF components will
    work directly with headphones.

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2012-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#include <MozziConfigValues.h>
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_2PIN_PWM
#define MOZZI_AUDIO_RATE 32768
#define MOZZI_CONTROL_RATE 256  // Hz, powers of 2 are most reliable

#include <Mozzi.h>
#include <Oscil.h>
#include <tables/cos2048_int8.h>  // table for Oscils to play
#include <mozzi_midi.h>
#include <FixMath.h>
#include <EventDelay.h>
#include <Smooth.h>



Oscil<COS2048_NUM_CELLS, MOZZI_AUDIO_RATE> aCarrier(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, MOZZI_AUDIO_RATE> aModulator(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, MOZZI_CONTROL_RATE> kModIndex(COS2048_DATA);

UFix<0, 16> mod_index;
UFix<8, 16> deviation;  // 8 so that we do not exceed 32bits in updateAudio

UFix<16, 16> carrier_freq, mod_freq;
const UFix<0,16> modulation_amp = 0.001; // how much the modulation index will vary around its mean
const UFix<2,0> mean_modulation_unscaled = 2; // the real mean modulation will be mean_modulation_unscaled * modulation_max 
                                        // this one is adding a bias to the oscillator, hence it should be bigger than one.

const UFix<2, 0> mod_to_carrier_ratio = 3;  // 3 fits in UFix<2,0> which has a maximum range of (2^2)-1=3

EventDelay kNoteChangeDelay;

const UFix<7, 0> note_upper_limit = 50, note_lower_limit = 32;

UFix<7, 0> note0 = note_lower_limit, note1 = note_lower_limit + UFix<7, 0>(5), target_note = 0;
SFix<2, 0> note_change_step = 3;  // will only take +3 or -3, 2bits are enough for that

UFix<7,8> smoothed_note;

Smooth<UFix<7, 8>> kSmoothNote(0.95f);

void setup() {
  kNoteChangeDelay.set(768);
  kModIndex.setFreq(.768f);  // sync with kNoteChangeDelay
  startMozzi();
}

void setFreqs(UFix<7, 8> midi_note) {
  carrier_freq = mtof(midi_note);
  mod_freq = UFix<14,16>(carrier_freq) * mod_to_carrier_ratio;
  deviation = ((UFix<16,0>(mod_freq)) * mod_index).sR<8>();  // the sR here cheaply divides the deviation by 256.

  aCarrier.setFreq(carrier_freq);
  aModulator.setFreq(mod_freq);
}

void updateControl() {
  if (kNoteChangeDelay.ready()) {
    if (target_note == note0) 
    {
      note1 = note1 + note_change_step;
      target_note = note1;
    } 
    else 
    {
      note0 = note0 + note_change_step;
      target_note = note0;
    }
    if(note0>note_upper_limit) note_change_step = -3;
    if(note0<note_lower_limit) note_change_step = 3;
    kNoteChangeDelay.start();
  }
  mod_index = (toSFraction(kModIndex.next()) + mean_modulation_unscaled).sR<2>(); // the sR is to scale back in pure fractional range

  smoothed_note = kSmoothNote.next(target_note);
  setFreqs(smoothed_note);

}


AudioOutput updateAudio(){
auto modulation = (deviation * toSFraction(aModulator.next()));
return MonoOutput::from8Bit(aCarrier.phMod(modulation));
}


void loop() {
    audioHook();
}