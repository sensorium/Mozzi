/*
  Example using a potentiometer to change the amplitude of a sinewave
  and a light dependent resister (LDR) to change the frequency.
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

  Light dependent resistor (LDR) and 5.1k resistor on analog pin 1:
    LDR from analog pin to +5V (3.3V on Teensy 3.1)
    5.1k resistor from analog pin to ground

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

const char KNOB_PIN = 0; // set the input for the knob to analog pin 0
const char LDR_PIN = 1; // set the input for the LDR to analog pin 1

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin(SIN2048_DATA);

byte volume;

void setup(){
  //Serial.begin(9600); // for Teensy 3.1, beware printout can cause glitches
  Serial.begin(115200); // set up the Serial output so we can look at the piezo values // set up the Serial output so we can look at the analog levels
  startMozzi(); // :))
}


void updateControl(){
  // read the potentiometer as only 8 bit volume range for efficient calculations in updateAudio
  volume = mozziAnalogRead<8>(KNOB_PIN); // value is 0-255

  // print the value to the Serial monitor for debugging
  Serial.print("volume = ");
  Serial.print((int)volume);
  Serial.print("\t"); // prints a tab

  // read the light dependent resistor
  int light_level = mozziAnalogRead<10>(LDR_PIN); // We request 10 bits, here, however. Value is 0-1023

  // print the value to the Serial monitor for debugging
  Serial.print("light level = ");
  Serial.print(light_level);
  Serial.print("\t"); // prints a tab

  // set the frequency
  aSin.setFreq(light_level);

  Serial.println(); // next line
}


AudioOutput updateAudio(){
  // cast char output from aSin.next() to int to make room for multiplication
  return MonoOutput::from16Bit((int)aSin.next() * volume); // 8 bit * 8 bit gives 16 bits value
}


void loop(){
  audioHook(); // required here
}
