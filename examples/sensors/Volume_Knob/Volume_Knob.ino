/*  
  Example using a potentiometer to control the amplitude of a sinewave
  with Mozzi sonification library.

  Demonstrates the use of Oscil to play a wavetable, and analog input for control.

  The circuit:
  *  Audio output on digital pin 9 (on a Uno or similar), or 
     check the README or http://sensorium.github.com/Mozzi/

  *  Potentiometer connected to analog pin 0.
  *  Center pin of the potentiometer goes to the analog pin.
  *  Side pins of the potentiometer go to +5V and ground

  Mozzi help/discussion/announcements:
  https://groups.google.com/forum/#!forum/mozzi-users

  Tim Barrass 2013.
  This example code is in the public domain.
 */

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <mozzi_analog.h>

// use: Oscil <table_size, update_rate> oscilName (wavetable)
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

const int INPUT_PIN = 0; // set the input for the Light Dependent Resistor to analog pin 0

unsigned char volume;


void setup(){
  Serial.begin(115200); // set up the Serial output so we can look at the light level
  setupFastAnalogRead(); // increases the speed of reading the analog input
  aSin.setFreq(440);
  startMozzi(); // :))
}


void updateControl(){
  // read the light dependent resistor
  int sensor_value = analogRead(INPUT_PIN); // value is 0-1023
  
  // map it to an 8 bit range for efficient calculations in updateAudio
  volume = map(sensor_value, 0, 1023, 0, 255);  
  
  // print the value to the Serial monitor for debugging
  Serial.print("volume = ");
  Serial.println((int)volume);
}


int updateAudio(){
  return ((int)aSin.next() * volume)>>8; // shift back into range after multiplying by 8 bit value
}


void loop(){
  audioHook(); // required here
}
