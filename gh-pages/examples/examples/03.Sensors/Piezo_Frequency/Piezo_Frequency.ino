/*
   Example using a piezo to change the frequency of a sinewave
   with Mozzi sonification library.

   Demonstrates the use of Oscil to play a wavetable, and analog input for control.

   This example goes with a tutorial on the Mozzi site:
   http://sensorium.github.io/Mozzi/learn/introductory-tutorial/

   The circuit:
     Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
     check the README or http://sensorium.github.io/Mozzi/

     Piezo on analog pin 3:
       + connection of the piezo attached to the analog pin
       - connection of the piezo attached to ground
       1-megohm resistor between the analog pin and ground

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2013-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

// increase the rate of updateControl from the default of 64, to catch the piezo's rapid transients
#define MOZZI_CONTROL_RATE 128
#include <Mozzi.h>
#include <Oscil.h> // oscillator
#include <tables/sin2048_int8.h> // table for Oscils to play
#include <Smooth.h>

const int PIEZO_PIN = 3;  // set the analog input pin for the piezo

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin(SIN2048_DATA);


void setup(){
  //Serial.begin(9600); // for Teensy 3.1, beware printout can cause glitches
  Serial.begin(115200); // set up the Serial output so we can look at the piezo values
  startMozzi(); // :)) uses the control rate defined above
}


void updateControl(){
  // read the piezo. We request 12-bits resolution, here, for values of 0-4095. Some boards
  // will actually provide that much accuracy, for others the readings are simply shifted to a
  // larger range.
  int piezo_value = mozziAnalogRead(PIEZO_PIN); // value is 0-1023

  // print the value to the Serial monitor for debugging
  Serial.print("piezo_value = ");
  Serial.print(piezo_value);
  Serial.print("\t \t"); // prints 2 tabs

  // print the frequency to the Serial monitor for debugging
  Serial.print("frequency = ");
  Serial.print(frequency);

  // set the frequency
  aSin.setFreq(frequency);

  Serial.println(); // next line
}


AudioOutput updateAudio(){
  return MonoOutput::from8Bit(aSin.next());
}


void loop(){
  audioHook();
}
