/*  Example playing a squarewave on a sweep without aliases,
    using Mozzi sonification library.

    Demonstrates the use of MetaOscil to play a wavetables sequentially to avoid aliases.

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
#include <MetaOscil.h>
#include <tables/SQUARE_BAND_LIMITED/square_max_500_at_16384_2048_int8.h> // sine table for oscillator
#include <tables/SQUARE_BAND_LIMITED/square_max_1000_at_16384_2048_int8.h> // sine table for oscillator
#include <tables/SQUARE_BAND_LIMITED/square_max_2000_at_16384_2048_int8.h> // sine table for oscillator
#include <tables/SQUARE_BAND_LIMITED/square_max_4000_at_16384_2048_int8.h> // sine table for oscillator

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SQUARE_MAX_500_AT_16384_2048_NUM_CELLS, AUDIO_RATE> aSq200(SQUARE_MAX_500_AT_16384_2048_DATA);
Oscil <SQUARE_MAX_1000_AT_16384_2048_NUM_CELLS, AUDIO_RATE> aSq1000(SQUARE_MAX_1000_AT_16384_2048_DATA);
Oscil <SQUARE_MAX_2000_AT_16384_2048_NUM_CELLS, AUDIO_RATE> aSq2000(SQUARE_MAX_2000_AT_16384_2048_DATA);
Oscil <SQUARE_MAX_4000_AT_16384_2048_NUM_CELLS, AUDIO_RATE> aSq4000(SQUARE_MAX_4000_AT_16384_2048_DATA);

// use: MetaOscil <table_size, update_rate, number_of_oscil> MetaoscilName. All oscils used should have the same table_size.
MetaOscil<SQUARE_MAX_500_AT_16384_2048_NUM_CELLS, AUDIO_RATE, 4> BL_aSq;

// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 256 // Hz, powers of 2 are most reliable

int freq = 10;


void setup() {
  // Adding the oscillators to the MetaOscillator. First argument is the adress of the Oscillator to add (note the &), the second one is
  BL_aSq.addOscil(&aSq200, 500);
  BL_aSq.addOscil(&aSq1000, 1000);
  BL_aSq.addOscil(&aSq2000,2000);
  BL_aSq.addOscil(&aSq4000, 4000);

  startMozzi(CONTROL_RATE);
}


void updateControl() {
  // Manually increasing the frequency by 1Hz
  freq += 1;
  if (freq > 3000) freq = 10;
  aSq200.setFreq(freq);
  BL_aSq.setFreq(freq);
}


AudioOutput_t updateAudio() {
  return MonoOutput::from8Bit(BL_aSq.next());  // return a sample of the correct oscil to acheive no alias
  //return MonoOutput::from8Bit(aSq200.next());  // try to use this line instead to hear the non-band limited version
}


void loop() {
  audioHook(); // required here
}
