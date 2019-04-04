/*  Example of simple FM with the phase modulation technique,
    using Mozzi sonification library.

    Demonstrates Oscil::phMod() for phase modulation,
    Smooth() for smoothing control signals,
    and Mozzi's fixed point number types for fractional frequencies.

    Also shows the limitations of Mozzi's 16384Hz Sample rate,
    as aliasing audibly intrudes as the sound gets brighter around
    midi note 48.

    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.com/Mozzi/

		Mozzi documentation/API
		https://sensorium.github.io/Mozzi/doc/html/index.html

		Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2012, CC by-nc-sa.
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/cos2048_int8.h> // table for Oscils to play
#include <mozzi_midi.h>
#include <mozzi_fixmath.h>
#include <EventDelay.h>
#include <Smooth.h>

#define CONTROL_RATE 256 // Hz, powers of 2 are most reliable

Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCarrier(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModulator(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kModIndex(COS2048_DATA);

// The ratio of deviation to modulation frequency is called the "index of modulation". ( I = d / Fm )
// It will vary according to the frequency that is modulating the carrier and the amount of deviation.
// so deviation d = I   Fm
// haven't quite worked this out properly yet...

Q8n8 mod_index;// = float_to_Q8n8(2.0f); // constant version
Q16n16 deviation;

Q16n16 carrier_freq, mod_freq;

// FM ratio between oscillator frequencies, stays the same through note range
Q8n8 mod_to_carrier_ratio = float_to_Q8n8(3.f);

EventDelay kNoteChangeDelay;

// for note changes
Q7n8 target_note, note0, note1, note_upper_limit, note_lower_limit, note_change_step, smoothed_note;

// using Smooth on midi notes rather than frequency,
// because fractional frequencies need larger types than Smooth can handle
// Inefficient, but...until there is a better Smooth....
Smooth <int> kSmoothNote(0.95f);

void setup(){
  kNoteChangeDelay.set(768); // ms countdown, taylored to resolution of CONTROL_RATE
  kModIndex.setFreq(.768f); // sync with kNoteChangeDelay
  target_note = note0;
  note_change_step = Q7n0_to_Q7n8(3);
  note_upper_limit = Q7n0_to_Q7n8(50);
  note_lower_limit = Q7n0_to_Q7n8(32);
  note0 = note_lower_limit;
  note1 = note_lower_limit + Q7n0_to_Q7n8(5);
  startMozzi(CONTROL_RATE);
}

void setFreqs(Q8n8 midi_note){
  carrier_freq = Q16n16_mtof(Q8n8_to_Q16n16(midi_note)); // convert midi note to fractional frequency
  mod_freq = ((carrier_freq>>8) * mod_to_carrier_ratio)  ; // (Q16n16>>8)   Q8n8 = Q16n16, beware of overflow
  deviation = ((mod_freq>>16) * mod_index); // (Q16n16>>16)   Q8n8 = Q24n8, beware of overflow
  aCarrier.setFreq_Q16n16(carrier_freq);
  aModulator.setFreq_Q16n16(mod_freq);
}

void updateControl(){
  // change note
  if(kNoteChangeDelay.ready()){
    if (target_note==note0){
      note1 += note_change_step;
      target_note=note1;
    }
    else{
      note0 += note_change_step;
      target_note=note0;
    }

    // change direction
    if(note0>note_upper_limit) note_change_step = Q7n0_to_Q7n8(-3);
    if(note0<note_lower_limit) note_change_step = Q7n0_to_Q7n8(3);

    // reset eventdelay
    kNoteChangeDelay.start();
  }

  // vary the modulation index
  mod_index = (Q8n8)350+kModIndex.next();

  // here's where the smoothing happens
  smoothed_note = kSmoothNote.next(target_note);
  setFreqs(smoothed_note);

}


int updateAudio(){
  Q15n16 modulation = deviation * aModulator.next() >> 8;
  return (int)aCarrier.phMod(modulation);
}


void loop(){
  audioHook();
}
