/*  Example of Amplitude Modulation synthesis
    using Mozzi sonification library.
  
    Demonstrates modulating the gain of one oscillator
    by the instantaneous amplitude of another,
    shows the use of fixed-point numbers to express fractional
    values, random numbers with rand(), and EventDelay()
    for scheduling.
  
    IMPORTANT: this sketch requires Mozzi/mozzi_config.h to be
    be changed from STANDARD mode to HIFI.
    In Mozz/mozzi_config.h, change
    #define AUDIO_MODE STANDARD
    //#define AUDIO_MODE HIFI
    to
    //#define AUDIO_MODE STANDARD
    #define AUDIO_MODE HIFI
  
  
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

    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2012-13.
    This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/COS2048_int8.h> // table for Oscils to play
#include <mozzi_fixmath.h>
#include <EventDelay.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>


// audio oscils
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCarrier(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModulator(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModDepth(COS2048_DATA);

// for scheduling note changes in updateControl()
EventDelay  kNoteChangeDelay;

// synthesis parameters in fixed point formats
Q8n8 ratio; // unsigned int with 8 integer bits and 8 fractional bits
Q24n8 carrier_freq; // unsigned long with 24 integer bits and 8 fractional bits
Q24n8 mod_freq; // unsigned long with 24 integer bits and 8 fractional bits

// for random notes
Q8n0 octave_start_note = 42;



void setup(){
  ratio = float_to_Q8n8(3.0f);   // define modulation ratio in float and convert to fixed-point
  kNoteChangeDelay.set(200); // note duration ms, within resolution of CONTROL_RATE
  aModDepth.setFreq(13.f);     // vary mod depth to highlight am effects
  randSeed(); // reseed the random generator for different results each time the sketch runs
  startMozzi(); // use default CONTROL_RATE 64
}


void updateControl(){
  static Q16n16 last_note = octave_start_note;

  if(kNoteChangeDelay.ready()){

    // change octave now and then
    if(rand((unsigned char)5)==0){
      last_note = 36+(rand((unsigned char)6)*12);
    }

    // change step up or down a semitone occasionally
    if(rand((unsigned char)13)==0){
      last_note += 1-rand((unsigned char)3);
    }

    // change modulation ratio now and then
    if(rand((unsigned char)5)==0){
      ratio = ((Q8n8) 1+ rand((unsigned char)5)) <<8;
    }

    // sometimes add a fraction to the ratio
    if(rand((unsigned char)5)==0){
      ratio += rand((unsigned char)255);
    }

    // step up or down 3 semitones (or 0)
    last_note += 3 * (1-rand((unsigned char)3));

    // convert midi to frequency
    Q16n16 midi_note = Q8n0_to_Q16n16(last_note); 
    carrier_freq = Q16n16_to_Q24n8(Q16n16_mtof(midi_note));

    // calculate modulation frequency to stay in ratio with carrier
    mod_freq = (carrier_freq * ratio)>>8; // (Q24n8   Q8n8) >> 8 = Q24n8

    // set frequencies of the oscillators
    aCarrier.setFreq_Q24n8(carrier_freq);
    aModulator.setFreq_Q24n8(mod_freq);

    // reset the note scheduler
    kNoteChangeDelay.start();
  }
}


int updateAudio(){
  unsigned int mod = (128u+ aModulator.next()) * ((unsigned char)128+ aModDepth.next());
  int out = ((long)mod * aCarrier.next())>>10; // 16 bit   8 bit = 24 bit, then >>10 = 14 bit
  return out;
}


void loop(){
  audioHook();
}




