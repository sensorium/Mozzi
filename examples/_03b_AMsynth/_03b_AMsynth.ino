/*  Example of Amplitude Modulation synthesis
 *  using Mozzi sonification library.
 *
 *  Demonstrates modulating the gain of one oscillator
 *  by the instantaneous amplitude of another oscillator,
 *  shows the use of fixed-point numbers to express fractional
 *  values, random numbers with xorshift96(), and EventDelay()
 *  for scheduling.
 *
 *  Circuit: Audio output on digital pin 9.
 *
 *  Mozzi help/discussion/announcements:
 *  https://groups.google.com/forum/#!forum/mozzi-users
 *
 *  Tim Barrass 2012.
 *  This example code is in the public domain.
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/COS2048_int8.h> // table for Oscils to play
#include <utils.h>
#include <fixedMath.h>
#include <EventDelay.h>

#define CONTROL_RATE 64 // powers of 2 please

// audio oscils
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCarrier(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModulator(COS2048_DATA);

// control oscil
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kCarrierFreqSweeper(COS2048_DATA);

// for scheduling note changes in updateControl()
EventDelay kChangeNoteDelay(CONTROL_RATE);

unsigned char mod_depth = 256;

// synthesis parameters in fixed point formats
Q8n8 ratio; // unsigned int with 8 integer bits and 8 fractional bits
Q24n8 carrier_freq; // unsigned long with 24 integer bits and 8 fractional bits
Q24n8 mod_freq; // unsigned long with 24 integer bits and 8 fractional bits

// for random notes
Q8n0 octave_start_note = 42;

void setup(){
  ratio = float_to_Q8n8(2.0f); // define ratio in float and convert to fixed-point
  kCarrierFreqSweeper.setFreq(0.1f);
  kChangeNoteDelay.set(200); // note duration ms, within resolution of CONTROL_RATE

  startMozzi(CONTROL_RATE);
}

void updateControl(){
  if(kChangeNoteDelay.ready()){
    if(lowByte(xorshift96())>200){
      // change base note of sequence to midi 36 or any of 3 octaves above
      octave_start_note = ((lowByte(xorshift96())>>6)*12)+36;
    }
    Q8n0 random0to15 = lowByte(xorshift96())>>4;
    Q16n16 midi_note = Q8n0_to_Q16n16(octave_start_note+random0to15);
    carrier_freq = Q16n16_to_Q24n8(Q16n16_mtof(midi_note));
    mod_freq = (carrier_freq * ratio)>>8; // (Q24n8 * Q8n8) >> 8 = Q24n8
    aCarrier.setFreq_Q24n8(carrier_freq);
    aModulator.setFreq_Q24n8(mod_freq);
    kChangeNoteDelay.start();
  }


}

int updateAudio(){
  int out = ((int)aCarrier.next() * ((unsigned char)128+ aModulator.next()))>>8;
  return out;
}

void loop(){
  audioHook();
}


