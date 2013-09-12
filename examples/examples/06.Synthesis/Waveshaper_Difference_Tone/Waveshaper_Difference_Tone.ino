/*  Example using waveshaping to modify the spectrum of an audio signal
    using Mozzi sonification library.
  
    Demonstrates the use of WaveShaper(), EventDelay(), Smooth(),
    rand(), and fixed-point numbers.
  
    Circuit: Audio output on digital pin 9 (on a Uno or similar), or 
    check the README or http://sensorium.github.com/Mozzi/
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2012.
    This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <WaveShaper.h>
#include <EventDelay.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Smooth.h>
#include <tables/sin2048_int8.h>
#include <tables/waveshape_chebyshev_3rd_256_int8.h>
#include <tables/waveshape_chebyshev_6th_256_int8.h>
#include <tables/waveshape_compress_512_to_488_int16.h>


// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 64 // powers of 2 please

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin1(SIN2048_DATA); // sine wave sound source
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin2(SIN2048_DATA); // sine wave sound source
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aGain(SIN2048_DATA); // to fade audio signal in and out before waveshaping

// for scheduling note changes
EventDelay kChangeNoteDelay;

// audio frequency as Q16n16 fractional number
Q16n16 freq1 = Q8n0_to_Q16n16(300); 

// smooth transitions between notes
Smooth <unsigned int> kSmoothFreq(0.85f);
int target_freq, smoothed_freq;


void setup(){
  startMozzi(CONTROL_RATE); // set a control rate of 64 (powers of 2 please)
  aSin1.setFreq_Q16n16(freq1); // set the frequency with a Q16n16 fractional number
  aGain.setFreq(0.2f); // use a float for low frequencies, in setup it doesn't need to be fast
  kChangeNoteDelay.set(2000); // note duration ms, within resolution of CONTROL_RATE
}


void updateControl(){
  if(kChangeNoteDelay.ready()){
    // change proportional frequency of second tone
    byte harmonic = rand((byte)12);
    byte shimmer = rand((byte)255);
    Q16n16 harmonic_step = freq1/12;
    Q16n16 freq2difference = harmonic*harmonic_step;
    freq2difference += (harmonic_step*shimmer)>>11;
    Q16n16 freq2 = freq1-freq2difference;
    aSin2.setFreq_Q16n16(freq2); // set the frequency with a Q16n16 fractional number
    kChangeNoteDelay.start();
  }
}


int updateAudio(){
  int asig = (int)((((long)aSin1.next()+ aSin2.next())*(200u+aGain.next()))>>3);
  int clipped = constrain(asig,-244,243);
  return clipped;
}


void loop(){
  audioHook(); // required here
}







