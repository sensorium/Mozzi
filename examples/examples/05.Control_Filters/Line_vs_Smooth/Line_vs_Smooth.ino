/*  Example of 2 different ways to smooth analog inputs,
    using Mozzi sonification library.  The inputs are used 
    to control the frequencies of two oscillators.
  
    Demonstrates how to read analog inputs asynchronously, so values are
    updated in the background while audio generation continues,
    and the most recent readings can be read anytime from an array.
    Also demonstrates linear interpolation with Line(),
    filtering with Smooth(), and fixed point numbers.
    
    Circuit: Audio output on digital pin 9 
    (for standard output on a Uno or similar), or 
    check the README or http://sensorium.github.com/Mozzi/
    Your choice of analog sesnsors, or
    2 10k Potentiometers with wipers (middle terminals) 
    connected to analog pins 0, 1 and 2, and
    outside leads to ground and +5V.  
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2013.
    This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <mozzi_fixmath.h>
#include <Line.h>
#include <Smooth.h>
#include <mozzi_analog.h>

#define CONTROL_RATE 64 // powers of 2 please

// 2 oscillators to compare linear interpolated vs smoothed control
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin0(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin1(SIN2048_DATA);


// Line to interpolate frequency for aSin0.
// Q16n16 is basically (yourNumber << 16)
// Line needs the small analog input integer values of 0-1023 
// to be scaled up if the time (the number of steps) is big 
// enough that distance/time gives a step-size of 0.  
// Then you need floats (which are sometimes too slow and create glitches), 
// or fixed point.  Q16n16: ugly but good.
Line <Q16n16> aInterpolate;

// the number of audio steps the line has to take to reach the next control value
const unsigned int AUDIO_STEPS_PER_CONTROL = AUDIO_RATE / CONTROL_RATE;

// Smoothing unit for aSin1
// This might be better with Q24n8 numbers for more precision, 
// but we'll keep it simpler for the demo.
float smoothness = 0.995f;
Smooth <unsigned int> aSmooth(smoothness); // to smooth frequency for aSin1



void setup(){
  aSin0.setFreq(660.f);
  aSin1.setFreq(220.f);
  startMozzi(CONTROL_RATE);
}


volatile unsigned int freq1;  // global so it can be used in updateAudio, volatile to stop it getting changed while being used

void updateControl(){
  Q16n16 freq0 = Q16n0_to_Q16n16(mozziAnalogRead(0)); // 0 to 1023, scaled up to Q16n16 format
  freq1 = (unsigned int) mozziAnalogRead(1); // 0 to 1023
   aInterpolate.set(freq0, AUDIO_STEPS_PER_CONTROL);
}


int updateAudio(){
  Q16n16 interpolatedFreq = aInterpolate.next(); // get the next linear interpolated freq
  aSin0.setFreq_Q16n16(interpolatedFreq); 

  int smoothedFreq = aSmooth.next(freq1); // get the next filtered frequency
  aSin1.setFreq(smoothedFreq);
  
  return (int) (aSin0.next() + aSin1.next())>>1; // >>1 is like /2 but faster (the compiler doesn't seem to optimise it automatically)
}


void loop(){
  audioHook();
}





