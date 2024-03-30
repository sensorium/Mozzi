/*  Example of a simple light-sensing theremin-like
    instrument with long echoes,
    using Mozzi sonification library.

    Demonstrates ControlDelay() for echoing control values,
    and smoothing an analog input from a sensor
    signal with RollingAverage().

    The circuit:

    Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.io/Mozzi/

    Light dependent resistor (LDR) and 5.1k resistor on analog pin 1:
    LDR from analog pin to +5V (3.3V on Teensy 3.1)
    5.1k resistor from analog pin to ground

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2012-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#define MOZZI_CONTROL_RATE 64
#include <Mozzi.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <RollingAverage.h>
#include <ControlDelay.h>

#define INPUT_PIN 0 // analog control input

unsigned int echo_cells_1 = 32;
unsigned int echo_cells_2 = 60;
unsigned int echo_cells_3 = 127;

ControlDelay <128, int> kDelay; // 2seconds

// oscils to compare bumpy to averaged control input
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin0(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin1(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin2(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin3(SIN2048_DATA);

// use: RollingAverage <number_type, how_many_to_average> myThing
RollingAverage <int, 32> kAverage; // how_many_to_average has to be power of 2
int averaged;

void setup(){
  kDelay.set(echo_cells_1);
  startMozzi();
}


void updateControl(){
  int bumpy_input = mozziAnalogRead<10>(INPUT_PIN); // request reading at 10-bit resolution, i.e. 0-1023
  averaged = kAverage.next(bumpy_input);
  aSin0.setFreq(averaged);
  aSin1.setFreq(kDelay.next(averaged));
  aSin2.setFreq(kDelay.read(echo_cells_2));
  aSin3.setFreq(kDelay.read(echo_cells_3));
}


AudioOutput updateAudio(){
  return MonoOutput::fromAlmostNBit(12,
    3*((int)aSin0.next()+aSin1.next()+(aSin2.next()>>1)
    +(aSin3.next()>>2))
  );
}


void loop(){
  audioHook();
}
