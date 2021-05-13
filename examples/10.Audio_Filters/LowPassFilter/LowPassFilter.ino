/*  Example of filtering a wave,
    using Mozzi sonification library.

    Demonstrates LowPassFilter().

    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.io/Mozzi/

		Mozzi documentation/API
		https://sensorium.github.io/Mozzi/doc/html/index.html

		Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2012, CC by-nc-sa.
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/chum9_int8.h> // recorded audio wavetable
#include <tables/cos2048_int8.h> // for filter modulation
#include <LowPassFilter.h>
#include <mozzi_rand.h>

Oscil<CHUM9_NUM_CELLS, AUDIO_RATE> aCrunchySound(CHUM9_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kFilterMod(COS2048_DATA);

LowPassFilter lpf;
uint8_t resonance = 200; // range 0-255, 255 is most resonant

void setup(){
  startMozzi();
  aCrunchySound.setFreq(2.f);
  kFilterMod.setFreq(1.3f);
}

void loop(){
  audioHook();
}

void updateControl(){
  if (rand(CONTROL_RATE/2) == 0){ // about once every half second
    kFilterMod.setFreq((float)rand(255)/64);  // choose a new modulation frequency
  }
  // map the modulation into the filter range (0-255), corresponds with 0-8191Hz
  byte cutoff_freq = 100 + kFilterMod.next()/2;
  lpf.setCutoffFreqAndResonance(cutoff_freq, resonance);
}

AudioOutput_t updateAudio(){
  char asig = lpf.next(aCrunchySound.next());
  return MonoOutput::from8Bit(asig);
}
