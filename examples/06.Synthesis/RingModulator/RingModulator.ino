/*  Example playing a sinewave at a set frequency,
    using Mozzi sonification library.

    Demonstrates the use of Oscil to play a wavetable.

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

#define MOZZI_CONTROL_RATE 64  // Hz, powers of 2 are most reliable; 64 Hz is actually the default, but shown here, for clarity
#include <Mozzi.h>
#include <Oscil.h>                // oscillator template
#include <tables/sin2048_int8.h>  // sine table for oscillator
#include <EventDelay.h>
#include <Smooth.h>
#include <mozzi_rand.h>

EventDelay kModulationChangeDelay;
Smooth<uint8_t> kSmoothModulation(0.9f);
// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil<SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin(SIN2048_DATA);
//Oscil <SIN2048_NUM_CELLS, MOZZI_CONTROL_RATE> aSin2(SIN2048_DATA); // controls the amount of Ring Modulation

int8_t prev_sample = 0;
uint8_t ring_mod, smoothed_ring_mod;

void setup() {
  Serial.begin(115200);
  kModulationChangeDelay.set(2000);
  startMozzi();      // :)
  aSin.setFreq(64);  // set the frequency
}


void updateControl() {
  //ring_mod_amount = int(aSin2.next() + 128);
  //Serial.println(ring_mod_amount);
  //ring_mod_amount = 255;
  if (kModulationChangeDelay.ready()) {
    ring_mod = rand(150);
    kModulationChangeDelay.start();
  }
  smoothed_ring_mod = kSmoothModulation(ring_mod);
}


AudioOutput updateAudio() {
  prev_sample = aSin.phMod(int32_t(prev_sample) * smoothed_ring_mod);
  return MonoOutput::from8Bit(prev_sample);  // return an int signal centred around 0
}


void loop() {
  audioHook();  // required here
}
