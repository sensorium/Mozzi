/*  Example of filtering a wave with different types of filters
    using Mozzi sonification library.

    Demonstrates MultiResonantFilter<uint8_t>.

    MultiResonantFilter is a derivative of ResonantFilter which allows all filter types to be accessed with only one filter.
    It behaves similarly to ResonantFilter except:
      - next(in) does not return anything and is just used to pass the current sample to the filter. Except for special cases should probably be called only once in updateAudio()
      - different filter outputs are accessible via low(), high(), band() and notch() functions.
      - note that the different filter types can be called in the same updateAudio(), for eg. to get different part of the signal.

    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.io/Mozzi/

    Mozzi documentation/API
    https://sensorium.github.io/Mozzi/doc/html/index.html

    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2012, CC by-nc-sa.
    Thomas Combriat 2023, CC by-nc-sa.
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/chum9_int8.h> // recorded audio wavetable
#include <tables/cos2048_int8.h> // for filter modulation
#include <ResonantFilter.h>
#include <mozzi_rand.h>

Oscil<CHUM9_NUM_CELLS, AUDIO_RATE> aCrunchySound(CHUM9_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kFilterMod(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kFilterMod2(COS2048_DATA);

MultiResonantFilter<uint8_t> mf; // Multifilter applied to a 8 bits signal.
                         // MultiResonantFilter<uint16_t> can also be used for signals with higher number of bits
                         // in this last case, both the cutoff frequency and the resonance are uint16_t,
                         // ranging from 0, to 65535.
                         
enum types {lowpass, bandpass, highpass, notch};
byte filter_type = 0;

uint8_t resonance = 200; // range 0-255, 255 is most resonant.

void setup() {
  startMozzi();
  aCrunchySound.setFreq(2.f);
  kFilterMod.setFreq(1.3f);
  kFilterMod2.setFreq(0.1f);
}

void loop() {
  audioHook();
}

void updateControl() {
  if (rand(CONTROL_RATE / 2) == 0) { // about once every half second
    kFilterMod.setFreq((float)rand(255) / 64); // choose a new modulation frequency
    filter_type = rand(4);  // change the filter type, randomly
  }
  // map the modulation into the filter range (0-255), corresponds with 0-AUDIO_RATE/(sqrt(2)*pi) Hz
  uint8_t cutoff_freq = (100 + kFilterMod.next() / 2) ;
  mf.setCutoffFreqAndResonance(cutoff_freq, resonance);
}

AudioOutput_t updateAudio() {  
  AudioOutput_t asig;
  mf.next(aCrunchySound.next()); // this send the current sample to the filter, does not return anything
  switch (filter_type) // recover the output from the current selected filter type.
  {
    case lowpass:
      asig = mf.low(); // lowpassed sample
      break;
    case highpass:
      asig = mf.high();  // highpassed sample
      break;
    case bandpass:
      asig = mf.band();  // bandpassed sample
      break;
    case notch:
      asig = mf.notch();  // notched sample
      break;
  }
  return MonoOutput::fromNBit(9, asig); // signal is theoretically 8 bits, but resonance push it further so we let one bit of margin
}