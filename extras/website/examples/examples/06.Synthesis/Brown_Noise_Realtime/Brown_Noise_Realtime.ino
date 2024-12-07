/*  Example generating brownian noise in real time,
    using Mozzi sonification library.

    Demonstrates rand(), a fast random generator,
    a filter used as a "leaky integrator"
    (based on an implementation by Barry Dorr:
    https://www.edn.com/design/systems-design/4320010/A-simple-software-lowpass-filter-suits-embedded-system-applications)
    and the use of Oscil to modulate amplitude.

    Circuit: Audio output on digital pin 9 on a Uno or similar.

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2013-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#include <Mozzi.h>
#include <mozzi_rand.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator

#define FILTER_SHIFT 6 // 5 or 6 work well - the spectrum of 6 looks a bit more linear, like the generated brown noise in Audacity

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin(SIN2048_DATA);

void setup()
{
  startMozzi(); // :)
  aSin.setFreq(0.05f); // set the frequency
}


void updateControl()
{
  // put changing controls in here
}


AudioOutput updateAudio()
{
  static int filtered;

  char whitenoise = rand((byte)255) - 128;
  filtered = filtered - (filtered>>FILTER_SHIFT) + whitenoise;

  int asig = filtered>>3; // shift to 8 bit range (trial and error)
  return MonoOutput::from16Bit((int)asig * aSin.next());
}


void loop(){
  audioHook(); // required here
}
