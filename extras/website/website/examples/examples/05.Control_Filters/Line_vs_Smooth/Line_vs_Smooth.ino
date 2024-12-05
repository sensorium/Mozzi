/*  Example of 2 different ways to smooth analog inputs,
    using Mozzi sonification library.  The inputs are used
    to control the frequencies of two oscillators.

    Demonstrates how to read analog inputs asynchronously, so values are
    updated in the background while audio generation continues,
    and the most recent readings can be read anytime from an array.
    Also demonstrates linear interpolation with Line(),
    filtering with Smooth(), and fixed point numbers from FixMath

    Circuit: Audio output on digital pin 9
    (for standard output on a Uno or similar), or
    check the README or http://sensorium.github.io/Mozzi/
    Your choice of analog sesnsors, or
    2 10k Potentiometers with wipers (middle terminals)
    connected to analog pins 0, 1 and 2, and
    outside leads to ground and +5V.

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users


   Copyright 2013-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#define MOZZI_CONTROL_RATE 64 // Hz, powers of 2 are most reliable
#include <Mozzi.h>
#include <Oscil.h>
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <FixMath.h>
#include <Line.h>
#include <Smooth.h>
#include <mozzi_analog.h>

// 2 oscillators to compare linear interpolated vs smoothed control
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin0(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin1(SIN2048_DATA);


// Line to interpolate frequency for aSin0.
// UFix<16,16>(yourNumber) is basically encoded in 16bits 
// with 16 extra bits for additionnal precision.
// Line needs the small analog input integer values of 0-1023
// to be scaled up if the time (the number of steps) is big
// enough that distance/time gives a step-size of 0.
// Then you need floats (which are sometimes too slow and create glitches).
Line <UFix<16,16>> aInterpolate;

// the number of audio steps the line has to take to reach the next control value
const unsigned int AUDIO_STEPS_PER_CONTROL = MOZZI_AUDIO_RATE / MOZZI_CONTROL_RATE;

// Smoothing unit for aSin1
// This might be better with Q24n8 numbers for more precision,
// but we'll keep it simpler for the demo.
float smoothness = 0.995f;
Smooth <unsigned int> aSmooth(smoothness); // to smooth frequency for aSin1



void setup(){
  aSin0.setFreq(660);
  aSin1.setFreq(220);
  startMozzi();
}


unsigned int freq1;  // global so it can be used in updateAudio

void updateControl(){
  UFix<16,16> freq0 = mozziAnalogRead(0); // 0 to 1023, with an additionnal 16bits of precision (which will be used in the interpolation.)
  freq1 = (unsigned int) mozziAnalogRead(1); // 0 to 1023
  aInterpolate.set(freq0, AUDIO_STEPS_PER_CONTROL);
}


AudioOutput updateAudio(){
  auto interpolatedFreq = aInterpolate.next(); // get the next linear interpolated freq
  aSin0.setFreq(interpolatedFreq);

  int smoothedFreq = aSmooth.next(freq1); // get the next filtered frequency
  aSin1.setFreq(smoothedFreq);


  // Here we add to SFix numbers, created from the Oscil, for the output. Mozzi knows what is the final range of this allowing for auto-scaling.
  return MonoOutput::fromSFix(toSFraction(aSin0.next()) + toSFraction(aSin1.next())); // auto-scaling of the output.
}


void loop(){
  audioHook();
}
