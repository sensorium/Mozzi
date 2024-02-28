/*  Example using waveshaping to modify the spectrum of an audio signal
    using Mozzi sonification library.

    Nearly verbatim translation from Waveshaper_Difference_Tone example
    but using using FixMath instead of mozzi_fixmath.
  
    Demonstrates the use of WaveShaper(), EventDelay(), Smooth(),
    rand(), and fixed-point numbers using FixMath.
  
    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or 
    check the README or http://sensorium.github.com/Mozzi/
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2012, Thomas Combriat and the Mozzi team 2024, CC by-nc-sa.
*/


#include <Mozzi.h>
#include <Oscil.h>
#include <WaveShaper.h>
#include <EventDelay.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <FixMath.h>
#include <Smooth.h>
#include <tables/sin2048_int8.h>


// use #define for CONTROL_RATE, not a constant
#define MOZZI_CONTROL_RATE 64  // powers of 2 please

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aSin1(SIN2048_DATA);  // sine wave sound source
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aSin2(SIN2048_DATA);  // sine wave sound source
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aGain(SIN2048_DATA);  // to fade audio signal in and out before waveshaping

// for scheduling note changes
EventDelay kChangeNoteDelay;

const UFix<6, 0> freq1 = 44;                                      // original example says 300, but overflows to 44;
const auto harmonic_step = freq1 * UFix<8, 0>(12).invAccurate();  // a bit weird way of saying harmonic_step = freq1/12…




void setup() {
  aSin1.setFreq(freq1);        // set the frequency with a Q16n16 fractional number
  aGain.setFreq(0.2f);         // use a float for low frequencies, in setup it doesn't need to be fast
  kChangeNoteDelay.set(2000);  // note duration ms, within resolution of CONTROL_RATE
  startMozzi();
}

void updateControl() {
  if (kChangeNoteDelay.ready()) {
    UFix<4, 0> harmonic = rand((byte)12);
    auto shimmer = toSFraction(rand((byte)255));
    auto freq2difference = (harmonic * harmonic_step) + (harmonic_step * shimmer).sR<2>();
    auto freq2 = (freq1 - freq2difference);
    aSin2.setFreq(freq2);
    kChangeNoteDelay.start();
  }
}


/**
@todo Add a constrain for FixMath
*/
AudioOutput updateAudio() {
  int asig = (int)((((uint32_t)aSin1.next() + aSin2.next()) * (200u + aGain.next())) >> 3);
  int16_t clipped = constrain(asig, -244, 243);
  return clipped;
}

void loop() {
  audioHook();  // required here
}
