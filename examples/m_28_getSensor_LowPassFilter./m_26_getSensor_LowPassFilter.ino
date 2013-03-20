/*  Example of reading analog inputs to control a filter,
 *  using Mozzi sonification library.
 *
 *  Demonstrates reading analog inputs asynchronously, so values are
 *  updated in the background while audio generation continues,
 *  and the most recent readings can be read anytime from an array.
 *  
 *  Circuit: Audio output on digital pin 9 (on a Uno or similar), or 
 *  check the README or http://sensorium.github.com/Mozzi/
 *  3 10k Potentiometers with wipers (middle terminals) 
 *  connected to analog pins 0, 1 and 2, and
 *  outside leads to ground and +5V.  
 *
 *  Mozzi help/discussion/announcements:
 *  https://groups.google.com/forum/#!forum/mozzi-users
 *
 *  Tim Barrass 2013.
 *  This example code is in the public domain.
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/chum9_int8.h> // recorded audio wavetable
#include <tables/cos2048_int8.h> // for filter modulation
#include <LowPassFilter.h>
#include <mozzi_fixmath.h> // for fractional modulation speed
#include <mozzi_analog.h>

#define CONTROL_RATE 64 // powers of 2 please

Oscil<CHUM9_NUM_CELLS, AUDIO_RATE> aCrunchySound(CHUM9_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kFilterMod(COS2048_DATA);

LowPassFilter lpf;

// Don't use the Arduino A0, A1 etc pin notation, just say 0, 1, 2...
#define CENTRE_FREQ_ANALOG_IN 0
#define MOD_SPEED_ANALOG_IN 1
#define MOD_WIDTH_ANALOG_IN 2


void setup(){
  startMozzi(CONTROL_RATE);
  aCrunchySound.setFreq(2.f);
  lpf.setResonance(200);
  initADC();
}


void updateControl(){

  unsigned char centre_freq = (unsigned char) (getSensor(CENTRE_FREQ_ANALOG_IN)>>2); // 0 to 255

  Q16n16 modulation_speed = ((Q16n16)getSensor(MOD_SPEED_ANALOG_IN)<<10); // range 0 to 15, Q16n16 fractional
  kFilterMod.setFreq_Q16n16(modulation_speed);

  unsigned char modulation_width = (unsigned char) (getSensor(MOD_WIDTH_ANALOG_IN)>>4); // 0 to 63
  char modulation = ((int) kFilterMod.next() * modulation_width)>>8; // -32 to 31 

  // add centre_freq to modulation, and constrain into the filter range (0-255)
  unsigned char cutoff_freq = constrain(centre_freq + modulation, 0, 255);

  lpf.setCutoffFreq(cutoff_freq);

  startRead();
}


int updateAudio(){
  char asig = lpf.next(aCrunchySound.next());
  return (int) asig;
}


void loop(){
  audioHook();
}





