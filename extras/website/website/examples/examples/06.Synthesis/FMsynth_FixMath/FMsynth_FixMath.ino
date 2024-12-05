/*  Example of simple FM with the phase modulation technique,
    using Mozzi sonification library.

    Demonstrates Oscil::phMod() for phase modulation,
    Smooth() for smoothing control signals,
    and FixMath fixed point number types for fractional frequencies.
    This is the same technique than the FMsynth example but
    using FixMath instead of mozzi_fixmath.

    Note that it is slightly less optimized (but runs fine on an
    Uno) in order to make the underlying algorithm clearer.
    An optimized version, using FixMath can be found in the 
    example FMsynth_32k_HIFI.

		Mozzi documentation/API
		https://sensorium.github.io/Mozzi/doc/html/index.html

		Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2012, Thomas Combriat and the Mozzi team 2023, CC by-nc-sa.
*/

#include <MozziConfigValues.h>
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
UFix<16, 16> deviation;

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
  mod_freq = carrier_freq * mod_to_carrier_ratio;
  deviation = mod_freq * mod_index;
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
  mod_index = (toSFraction(kModIndex.next()) + mean_modulation_unscaled) * modulation_amp; // toSFraction(kModIndex.next() is in [-1,1[, 
                                                                                          // we bias it to [1, 3[ and then scale it with modulation_amp to [0.001, 0.003[

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
