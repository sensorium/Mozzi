/*  Example of simple FM with the phase modulation technique,
    using Mozzi sonification library.

    Demonstrates Oscil::phMod() for phase modulation,
    Smooth() for smoothing control signals,
    and Mozzi's fixed point number types for fractional frequencies.

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

    Tim Barrass 2012-13, CC by-nc-sa.
*/

#include <MozziConfigValues.h>
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_2PIN_PWM
#define MOZZI_AUDIO_RATE 32768

#include <Mozzi.h>
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


AudioOutput_t updateAudio(){
  Q15n16 modulation = deviation * aModulator.next() >> 8;
  return MonoOutput::from8Bit(aCarrier.phMod(modulation)); // Internally still only 8 bits, will be shifted up to 14 bits in HIFI mode
}


void loop(){
  audioHook();
}
