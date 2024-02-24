/*  Example using a MetaOscil to generate an alias free square wave on a sweep
    using Mozzi sonification library.

    Waveforms which are not only sines (Saw, square, triangle) are composed by a lot of harmonics which are at frequencies multiple of the fundamental frequency.
    If the frequency of one of these harmonics is higher than half the sampling frequency (https://en.wikipedia.org/wiki/Nyquist_frequency) (AUDIO_RATE/2 here)
    it will create "aliases" (https://en.wikipedia.org/wiki/Aliasing) which will sound out of tune are they not harmonically related to the fundamental.
    The higher the pitch, the more harmonics are above the Nyquist limit and the more aliased will be present for a given waveform. 

    One way to avoid aliases is to use "band-limited" waveforms which have a limited sets of harmonics in order to avoid them to reach the Nyquist limit.
    As these waveforms are band-limited they will sound less "crunchy" if they are used at frequencies lower than what they are meant to be because they 
    lack the high frequency contents.

    In order to paliate that, a common technique is to "swap" wave tables on the fly in order to keep the frequency content up to the Nyquist frequency but
    not above. This is the principal usage of MetaOscil.

    MetaOscil can be used (after construction) as a proper Oscil but really is a bunch of oscillators with only one playing.
    It will switch between different oscils seemlessly depending on the asked frequency. This allows to switch between oscillators with less 
    and less harmonics as the pitch goes up, in order to avoid aliases, which is demonstrated by this example.

    The bandlimited tables are nammed according to the max frequency they can play without bringing aliases at a given frequency. For example:
    square_max_90_at_16384_512_int8.h ensures that no aliases will be present up to 90Hz at 16384Hz sampling rate (the default for Arduino).
    If your sampling rate is higher (say 32768 which is the default for most 32bits platforms) this table will be able to play up to 
    180=90*2Hz without aliases, as the Nyquist frequency is two times higher.
    
    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.io/Mozzi/

    Mozzi documentation/API
		https://sensorium.github.io/Mozzi/doc/html/index.html

		Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2012, Combriat T. 2021, CC by-nc-sa.
*/

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <MetaOscil.h>

// All the tables used for the MetaOscil need to be included
#include <tables/BandLimited_SQUARE/512/square_max_90_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 90Hz at a sampling frequency of 16384 (or 180Hz at a sampling frequency of 32768Hz)
#include <tables/BandLimited_SQUARE/512/square_max_101_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 101Hz at a sampling frequency of 16384
#include <tables/BandLimited_SQUARE/512/square_max_122_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 122Hz at a sampling frequency of 16384
#include <tables/BandLimited_SQUARE/512/square_max_138_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 138Hz at a sampling frequency of 16384
#include <tables/BandLimited_SQUARE/512/square_max_154_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 154Hz at a sampling frequency of 16384
#include <tables/BandLimited_SQUARE/512/square_max_174_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 174Hz at a sampling frequency of 16384
#include <tables/BandLimited_SQUARE/512/square_max_210_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 210Hz at a sampling frequency of 16384
#include <tables/BandLimited_SQUARE/512/square_max_264_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 264Hz at a sampling frequency of 16384
#include <tables/BandLimited_SQUARE/512/square_max_327_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 327Hz at a sampling frequency of 16384
#include <tables/BandLimited_SQUARE/512/square_max_431_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 431Hz at a sampling frequency of 16384
#include <tables/BandLimited_SQUARE/512/square_max_546_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 546Hz at a sampling frequency of 16384
#include <tables/BandLimited_SQUARE/512/square_max_744_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 744Hz at a sampling frequency of 16384
#include <tables/BandLimited_SQUARE/512/square_max_910_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 910Hz at a sampling frequency of 16384
#include <tables/BandLimited_SQUARE/512/square_max_1170_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 1170Hz at a sampling frequency of 16384
#include <tables/BandLimited_SQUARE/512/square_max_1638_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 1638Hz at a sampling frequency of 16384
#include <tables/BandLimited_SQUARE/512/square_max_2730_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 2730Hz at a sampling frequency of 16384
#include <tables/BandLimited_SQUARE/512/square_max_8192_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 8192Hz at a sampling frequency of 16384 (this is basically a sine wave)

// The proper Oscillators that will be managed by the MetaOscil
// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SQUARE_MAX_90_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq90(SQUARE_MAX_90_AT_16384_512_DATA);
Oscil <SQUARE_MAX_101_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq101(SQUARE_MAX_101_AT_16384_512_DATA);
Oscil <SQUARE_MAX_122_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq122(SQUARE_MAX_122_AT_16384_512_DATA);
Oscil <SQUARE_MAX_138_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq138(SQUARE_MAX_138_AT_16384_512_DATA);
Oscil <SQUARE_MAX_154_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq154(SQUARE_MAX_154_AT_16384_512_DATA);
Oscil <SQUARE_MAX_174_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq174(SQUARE_MAX_174_AT_16384_512_DATA);
Oscil <SQUARE_MAX_210_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq210(SQUARE_MAX_210_AT_16384_512_DATA);
Oscil <SQUARE_MAX_264_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq264(SQUARE_MAX_264_AT_16384_512_DATA);
Oscil <SQUARE_MAX_327_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq327(SQUARE_MAX_327_AT_16384_512_DATA);
Oscil <SQUARE_MAX_431_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq431(SQUARE_MAX_431_AT_16384_512_DATA);
Oscil <SQUARE_MAX_546_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq546(SQUARE_MAX_546_AT_16384_512_DATA);
Oscil <SQUARE_MAX_744_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq744(SQUARE_MAX_744_AT_16384_512_DATA);
Oscil <SQUARE_MAX_910_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq910(SQUARE_MAX_910_AT_16384_512_DATA);
Oscil <SQUARE_MAX_1170_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq1170(SQUARE_MAX_1170_AT_16384_512_DATA);
Oscil <SQUARE_MAX_1638_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq1638(SQUARE_MAX_1638_AT_16384_512_DATA);
Oscil <SQUARE_MAX_2730_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq2730(SQUARE_MAX_2730_AT_16384_512_DATA);
Oscil <SQUARE_MAX_8192_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq8192(SQUARE_MAX_8192_AT_16384_512_DATA);

// use: MetaOscil <table_size, update_rate, number_of_oscil> MetaoscilName. All oscils used should have the same table_size and **have to be put in increasing order of cutoff_frequencies**.
MetaOscil<SQUARE_MAX_90_AT_16384_512_NUM_CELLS, AUDIO_RATE, 16> BL_aSq {&aSq90, &aSq101, &aSq122, &aSq138, &aSq154, &aSq174, &aSq210, &aSq264, &aSq327, &aSq431, &aSq546, &aSq744, &aSq1170, &aSq1638, &aSq2730, &aSq8192};


// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 256 // Hz, powers of 2 are most reliable

int freq = 10;


void setup() {
  // Set the cutoff frequencies for all the Oscil in the MetaOscil ie at which frequency the MetaOscil will switch to the next Oscillator. Note that these are the same frequencies than the tables names in this case.
  // This have to follow the order given at the MetaOscil creation: this needs to be in increasing order.
  BL_aSq.setCutoffFreqs(90, 101, 122, 138, 154, 174, 210, 264, 327, 431, 546, 744, 1170, 1638, 2730, 8192);

  // Cutoff frequencies can also be set or changed individually.
  BL_aSq.setCutoffFreq(3000, 14);
  startMozzi(CONTROL_RATE);
}


void updateControl() {
  // Manually increasing the frequency by 1Hz
  freq += 1;
  if (freq > 3000) freq = 10;

  aSq90.setFreq(freq);
  BL_aSq.setFreq(freq);  //BL_aSq which is a metaOscil can be used just as a regular Oscil.
}


AudioOutput_t updateAudio() {
  //return MonoOutput::from8Bit(aSq90.next());  // try to use this line instead to hear the non-band limited version, sounds a bit like a radio
  return MonoOutput::from8Bit(BL_aSq.next());  // return a sample of the correct oscil to acheive no alias

}


void loop() {
  audioHook(); // required here
}
