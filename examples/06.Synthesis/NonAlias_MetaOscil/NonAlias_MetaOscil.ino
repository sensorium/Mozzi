/*  Example playing a sinewave at a set frequency,
    using Mozzi sonification library.

    Demonstrates the use of Oscil to play a wavetable.

    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.com/Mozzi/

    Mozzi documentation/API
		https://sensorium.github.io/Mozzi/doc/html/index.html

		Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2012, CC by-nc-sa.
*/

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <MetaOscil.h>32768
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

// use: MetaOscil <table_size, update_rate, number_of_oscil> MetaoscilName. All oscils used should have the same table_size.
MetaOscil<SQUARE_MAX_90_AT_16384_512_NUM_CELLS, AUDIO_RATE, 16> BL_aSq {&aSq90, &aSq101, &aSq122, &aSq138, &aSq154, &aSq174, &aSq210, &aSq264, &aSq327, &aSq431, &aSq546, &aSq744, &aSq1170, &aSq1638, &aSq2730, &aSq8192};
//MetaOscil<SQUARE_MAX_500_AT_16384_2048_NUM_CELLS, AUDIO_RATE, 4> BL_aSq;

// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 256 // Hz, powers of 2 are most reliable

int freq = 10;


void setup() {
  // Set the cutoff frequencies for all the Oscil in the MetaOscil
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
  //return MonoOutput::from8Bit(aSq90.next());  // try to use this line instead to hear the non-band limited version
  return MonoOutput::from8Bit(BL_aSq.next());  // return a sample of the correct oscil to acheive no alias

}


void loop() {
  audioHook(); // required here
}
