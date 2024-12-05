/*  Example of a pair of detuned oscillators
    sliding smoothly between frequencies,
    using Mozzi sonification library.

    Demonstrates using Lines to efficiently change the
    frequency of each oscillator at audio rate.   Calculating
    a new frequency for every step of a slide is a lot to
    do for every single sample, so instead this sketch works out the
    start and end frequencies for each control period and
    the phase increments (size of the steps through the sound table)
    required for the audio oscillators to generate those frequencies.
    Then, computationally cheap Lines are used to slide between the
    different phase increments smoothly at audio rate.
    Also shows how to use random offsets between the oscillators'
    frequencies to produce a chorusing/doubling effect.

    Important:
    This sketch uses MOZZI_OUTPUT_2PIN_PWM (aka HIFI) output mode, which
    is not available on all boards (among others, it works on the
    classic Arduino boards, but not Teensy 3.x and friends).

    The MOZZI_AUDIO_RATE (sample rate) is additionally configured at 32768 Hz,
    which is the default on most platforms, but twice the standard rate on
    AVR-CPUs.

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

#include <Mozzi.h>
#include <Line.h> // for smooth transitions
#include <Oscil.h> // oscillator template
#include <tables/saw8192_int8.h> // saw table for oscillator
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <mozzi_fixmath.h>

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SAW8192_NUM_CELLS, MOZZI_AUDIO_RATE> aSaw1(SAW8192_DATA);
Oscil <SAW8192_NUM_CELLS, MOZZI_AUDIO_RATE> aSaw2(SAW8192_DATA);

// use: Line <type> lineName
Line <long> aGliss1;
Line <long> aGliss2;

byte lo_note = 24; // midi note numbers
byte hi_note = 46;

long audio_steps_per_gliss = MOZZI_AUDIO_RATE / 4; // ie. 4 glisses per second
long control_steps_per_gliss = MOZZI_CONTROL_RATE / 4;

// stuff for changing starting positions, probably just confusing really
int counter = 0;
byte gliss_offset = 0;
byte gliss_offset_step = 2;
byte  gliss_offset_max = 36;


void setup() {
  //randSeed();
  //pinMode(0,OUTPUT); // without this, updateControl() gets interrupted ........??
  startMozzi(); // uses default 50 control rate
}


// variation between oscillator's phase increments
// looks like a big number, but they are high precision
// and this is just a fractional part
long variation(){
  return rand(16383);
}


void updateControl(){ // 900 us floats vs 160 fixed
  if (--counter <= 0){

    // start at a new note
    gliss_offset += gliss_offset_step;

    if(gliss_offset >= gliss_offset_max) {
      gliss_offset=0;

      // this is just confusing, you should ignore it!
      // alternate between 2 and 3 steps per start note each time a new "run" starts
      if(gliss_offset_step != 2){
        gliss_offset_step = 2;
      }
      else{
        gliss_offset_step = 3;
      }
    }

    // only need to calculate frequencies once each control update
    int start_freq = mtof(lo_note+gliss_offset);
    int end_freq = mtof(hi_note+gliss_offset);

    // find the phase increments (step sizes) through the audio table for those freqs
    // they are big ugly numbers which the oscillator understands but you don't really want to know
    long gliss_start = aSaw1.phaseIncFromFreq(start_freq);
    long gliss_end = aSaw1.phaseIncFromFreq(end_freq);

    aGliss1.set(gliss_start, gliss_end, audio_steps_per_gliss);
    // aGliss2 is an octave up and detuned
    aGliss2.set((gliss_start+(variation()*gliss_offset))*2, (gliss_end+(variation()*gliss_offset))*2, audio_steps_per_gliss);
    counter = control_steps_per_gliss;
  }
}


AudioOutput updateAudio(){
  aSaw1.setPhaseInc(aGliss1.next());
  aSaw2.setPhaseInc(aGliss2.next());
  return MonoOutput::fromNBit(9, (int)aSaw1.next()+aSaw2.next());
}


void loop(){
  audioHook();
}
