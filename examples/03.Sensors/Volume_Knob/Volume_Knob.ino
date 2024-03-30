/*
  Example using a potentiometer to control the amplitude of a sinewave
  with Mozzi sonification library.

  Demonstrates the use of Oscil to play a wavetable, and analog input for control.

  This example goes with a tutorial on the Mozzi site:
  http://sensorium.github.io/Mozzi/learn/introductory-tutorial/

  The circuit:
    Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.io/Mozzi/

  Potentiometer connected to analog pin 0:
     Center pin of the potentiometer goes to the analog pin.
     Side pins of the potentiometer go to +5V and ground

 +5V ---|
              /
  A0 ----\  potentiometer
              /
 GND ---|

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2013-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#include <Mozzi.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin(SIN2048_DATA);

const char INPUT_PIN = 0; // set the input for the knob to analog pin 0

// to convey the volume level from updateControl() to updateAudio()
byte volume;


void setup(){
  //Serial.begin(9600); // for Teensy 3.1, beware printout can cause glitches
  Serial.begin(115200); // set up the Serial output so we can look at the piezo values // set up the Serial output so we can look at the input values
  aSin.setFreq(440);
  startMozzi(); // :))
}


void updateControl(){
  // read the variable resistor for volume. We specifically request only 8 bits of resolution, here, which
  // is less than the default on most platforms, but a convenient range to work with, where accuracy is not too important.
  volume = mozziAnalogRead<8>(INPUT_PIN);

  // print the value to the Serial monitor for debugging
  Serial.print("volume = ");
  Serial.println((int)volume);
}


AudioOutput updateAudio(){
  return MonoOutput::from16Bit((int)aSin.next() * volume); // 8 bit * 8 bit gives 16 bits value
}


void loop(){
  audioHook(); // required here
}
