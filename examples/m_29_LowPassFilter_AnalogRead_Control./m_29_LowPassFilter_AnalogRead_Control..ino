/*  Example of filtering a wave, with analog inputs for control,
 *  using Mozzi sonification library.
 *
 *  This method of reading analog input is now depreciated in Mozzi.
 *  A better way to read analog is to use the asynchronous method
 *  shown in the m_26_getSensor_LowPassFilter example.
 *
 *  Still, there may be a situation where this is useful...
 *  Demonstrates LowPassFilter(), fixed point fractional number use,
 *  and Mozzi's setupFastAnalogRead().  In this example, 
 *  setupFastAnalogRead() is enough to prevent
 *  glitching during analogRead().
 *
 *  Mozzi also provides startAnalogRead() and receiveAnalogRead(),
 *  which enable reading analog input one channel at a time without 
 *  blocking other processes.  
 *  (Though in most cases the asynchronous getSensor() method is probably simpler and better).
 *
 *  Circuit: Audio output on digital pin 9 (on a Uno or similar), or 
 *  check the README or http://sensorium.github.com/Mozzi/
 *  3 10k Potentiometers with wipers (middle terminals) 
 *  connected to analog pins A2, A3 and A4, and
 *  outside leads to ground and +5V.  
 *  Or your choice of more interesting inputs!
 *  See the Arduino documentation for analogRead().
 *
 *  Mozzi help/discussion/announcements:
 *  https://groups.google.com/forum/#!forum/mozzi-users
 *
 *  Tim Barrass 2012.
 *  This example code is in the public domain.
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/chum9_int8.h> // recorded audio wavetable
#include <tables/cos2048_int8.h> // for filter modulation
#include <LowPassFilter.h>
#include <mozzi_analog.h>
#include <mozzi_fixmath.h> // for fractional modulation frequency

#define CONTROL_RATE 64 // powers of 2 please

Oscil<CHUM9_NUM_CELLS, AUDIO_RATE> aCrunchySound(CHUM9_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kFilterMod(COS2048_DATA);

LowPassFilter lpf;

#define CENTRE_FREQ_PIN A2
#define MOD_SPEED_PIN A3
#define MOD_WIDTH_PIN A4


void setup(){
  startMozzi(CONTROL_RATE);
  aCrunchySound.setFreq(2.f);
  lpf.setResonance(200);
  setupFastAnalogRead();
}


void updateControl(){
  // change any of the ranges here to suit your inputs, etc.
  
  unsigned char centre_freq = (unsigned char) (analogRead(CENTRE_FREQ_PIN)>>2); // 0 to 255

  Q16n16 modulation_speed = ((Q16n16)analogRead(MOD_SPEED_PIN)<<10); // range 0 to 15, Q16n16 fractional
  kFilterMod.setFreq_Q16n16(modulation_speed);
  
  unsigned char modulation_width = (unsigned char) (analogRead(MOD_WIDTH_PIN)>>4); // 0 to 63
  char modulation = ((int) kFilterMod.next() * modulation_width)>>8; // -32 to 31 

  // add centre_freq and modulation, and constrain into the filter range (0-255)
  unsigned char cutoff_freq = constrain(centre_freq + modulation, 0, 255);

  lpf.setCutoffFreq(cutoff_freq);

}


int updateAudio(){
  char asig = lpf.next(aCrunchySound.next());
  return (int) asig;
}


void loop(){
  audioHook();
}




