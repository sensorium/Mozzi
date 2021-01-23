/*  Example of simple FM with the phase modulation technique,
    using Mozzi sonification library and an external DAC MCP4921 (original library by Thomas Backman - https://github.com/exscape/electronics/tree/master/Arduino/Libraries/DAC_MCP49xx)
    using an user-defined audioOutput() function.
    Based on Mozzi's example: FMsynth.

    #define EXTERNAL_AUDIO_OUTPUT true should be uncommented in mozzi_config.h.

    Circuit: (see the DAC library README for details)

    MCP4921   //  Connect to:
    -------       -----------
    Vdd           V+
    CS            any digital pin defined by SS_PIN (see after), or pin 7 on UNO / 38 on Mega if you are using Portwrite
    SCK           SCK of Arduino
    SDI           MOSI of Arduino
    VoutA         to headphones/loudspeaker
    Vss           to GND
    VrefA         to V+ or a clean tension ref between V+ and GND
    LDAC          to GND


		Mozzi documentation/API
		https://sensorium.github.io/Mozzi/doc/html/index.html

		Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2012, CC by-nc-sa.
    T. Combriat 2020, CC by-nc-sa.
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/cos2048_int8.h> // table for Oscils to play
#include <mozzi_midi.h>
#include <mozzi_fixmath.h>
#include <EventDelay.h>
#include <Smooth.h>
#include <DAC_MCP49xx.h>  // https://github.com/tomcombriat/DAC_MCP49XX 
                          // which is an adapted fork from https://github.com/exscape/electronics/tree/master/Arduino/Libraries/DAC_MCP49xx  (Thomas Backman)

#define CONTROL_RATE 256 // Hz, powers of 2 are most reliable


// Synthesis part
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCarrier(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModulator(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kModIndex(COS2048_DATA);


Q8n8 mod_index;
Q16n16 deviation;
Q16n16 carrier_freq, mod_freq;
Q8n8 mod_to_carrier_ratio = float_to_Q8n8(3.f);
EventDelay kNoteChangeDelay;

// for note changes
Q7n8 target_note, note0, note1, note_upper_limit, note_lower_limit, note_change_step, smoothed_note;
Smooth <int> kSmoothNote(0.95f);


// External audio output parameters and DAC declaration
#define SS_PIN 38  // if you are on AVR and using PortWrite you need still need to put the pin you are actually using: 7 on Uno, 38 on Mega
DAC_MCP49xx dac(DAC_MCP49xx::MCP4921, SS_PIN);



void audioOutput(const AudioOutput f)
{
  // signal is passed as 16 bit, zero-centered, internally. This DAC expects 12 bits unsigned,
  // so shift back four bits, and add a bias of 2^(12-1)=2048
  uint16_t out = (f.l() >> 4) + 2048;
  dac.output(out);
}



void setup() {
  dac.init();

  kNoteChangeDelay.set(768); // ms countdown, taylored to resolution of CONTROL_RATE
  kModIndex.setFreq(.768f); // sync with kNoteChangeDelay
  target_note = note0;
  note_change_step = Q7n0_to_Q7n8(3);
  note_upper_limit = Q7n0_to_Q7n8(50);
  note_lower_limit = Q7n0_to_Q7n8(32);
  note0 = note_lower_limit;
  note1 = note_lower_limit + Q7n0_to_Q7n8(5);




  dac.setPortWrite(true);  //comment this line if you do not want to use PortWrite (for non-AVR platforms)
  startMozzi(CONTROL_RATE);
}

void setFreqs(Q8n8 midi_note) {
  carrier_freq = Q16n16_mtof(Q8n8_to_Q16n16(midi_note)); // convert midi note to fractional frequency
  mod_freq = ((carrier_freq >> 8) * mod_to_carrier_ratio)  ; // (Q16n16>>8)   Q8n8 = Q16n16, beware of overflow
  deviation = ((mod_freq >> 16) * mod_index); // (Q16n16>>16)   Q8n8 = Q24n8, beware of overflow
  aCarrier.setFreq_Q16n16(carrier_freq);
  aModulator.setFreq_Q16n16(mod_freq);
}

void updateControl() {
  // change note
  if (kNoteChangeDelay.ready()) {
    if (target_note == note0) {
      note1 += note_change_step;
      target_note = note1;
    }
    else {
      note0 += note_change_step;
      target_note = note0;
    }

    // change direction
    if (note0 > note_upper_limit) note_change_step = Q7n0_to_Q7n8(-3);
    if (note0 < note_lower_limit) note_change_step = Q7n0_to_Q7n8(3);

    // reset eventdelay
    kNoteChangeDelay.start();
  }

  // vary the modulation index
  mod_index = (Q8n8)350 + kModIndex.next();

  // here's where the smoothing happens
  smoothed_note = kSmoothNote.next(target_note);
  setFreqs(smoothed_note);

}


AudioOutput_t updateAudio() {
  Q15n16 modulation = deviation * aModulator.next() >> 8;

  return MonoOutput::from8Bit(aCarrier.phMod(modulation));
  // this example does not really use the full capability of the 12bits of the DAC
}


void loop() {
  audioHook();
}
