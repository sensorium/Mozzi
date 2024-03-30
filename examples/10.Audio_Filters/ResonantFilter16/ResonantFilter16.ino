/*  Example of filtering a wave,
    using Mozzi sonification library.

    Demonstrates ResonantFilter<uint16_t, FILTER_TYPE>.

    ResonantFilter<uint16_t, FILTER_TYPE> is a different version of ResonantFilter<uint8_t, FILTER_TYPE> which uses bigger containing types for internal calculations.
    The main differences with LowPassFilter are:
      - parameters (resonance and cutoff_freq) are coded on 16bits, allowing for smoother transitions and finer tuning if needed,
      - for 8bits platforms (like the Arduino) it will accept input samples of more than 8bits without overflowing,
      - the drawback is higher CPU usage, especially for 8bits platform. If speed is crucial, you should probably use ResonantFilter<uint8_t, FILTER_TYPE> instead.

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

#include <Mozzi.h>
#include <Oscil.h>
#include <tables/chum9_int8.h> // recorded audio wavetable
#include <tables/cos2048_int8.h> // for filter modulation
#include <ResonantFilter.h>
#include <mozzi_rand.h>

Oscil<CHUM9_NUM_CELLS, MOZZI_AUDIO_RATE> aCrunchySound(CHUM9_DATA);
Oscil<COS2048_NUM_CELLS, MOZZI_CONTROL_RATE> kFilterMod(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, MOZZI_CONTROL_RATE> kFilterMod2(COS2048_DATA);

//Different types of filters available
LowPassFilter16 rf;  // Equivalent to ResonantFilter<LOWPASS, uint16_t>
//ResonantFilter<HIGHPASS, uint16_t> rf;  // HighPass filter
//ResonantFilter<BANDPASS, uint16_t> rf;  // BandPass filter
//ResonantFilter<NOTCH, uint16_t> rf;  // Notch filter


uint16_t resonance = 50000; // range 0-65535, 65535 is most resonant.
                            // note the difference of type with the LowPassFilter()

void setup(){
  startMozzi();
  aCrunchySound.setFreq(2.f);
  kFilterMod.setFreq(1.3f);
  kFilterMod2.setFreq(0.1f);
}

void loop(){
  audioHook();
}

void updateControl(){
  if (rand(MOZZI_CONTROL_RATE/2) == 0){ // about once every half second
    kFilterMod.setFreq((float)rand(255)/64);  // choose a new modulation frequency
  }
  // map the modulation into the filter range (0-255), corresponds with 0-MOZZI_AUDIO_RATE/(sqrt(2)*pi) Hz
  uint16_t cutoff_freq = (100 + kFilterMod.next()/2) * (170+kFilterMod2.next());
  rf.setCutoffFreqAndResonance(cutoff_freq, resonance);
}

AudioOutput updateAudio(){
  AudioOutput asig = rf.next(aCrunchySound.next());
  return MonoOutput::from8Bit(asig);
}
