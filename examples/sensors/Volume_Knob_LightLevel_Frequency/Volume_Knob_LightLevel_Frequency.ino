/*  
  Example using a potentiometer to change the amplitude of a sinewave
  and a light dependent resister (LDR) to change the frequency.
  with Mozzi sonification library.

  Demonstrates the use of Oscil to play a wavetable, and analog input for control.

  The circuit:
  *  Audio output on digital pin 9 (on a Uno or similar), or 
     check the README or http://sensorium.github.com/Mozzi/

  Potentiometer connected to analog pin 0:
  *  Center pin of the potentiometer goes to the analog pin.
  *  Side pins of the potentiometer go to +5V and ground

   Light dependent resistor connected to analog pin 1:
   *  1 leg of the LDR to the analog pin
   *  1 leg of the LDR to +5v
   *  5k resistor from the analog pin to ground
 
 
  Mozzi help/discussion/announcements:
  https://groups.google.com/forum/#!forum/mozzi-users

  Tim Barrass 2013.
  This example code is in the public domain.
 */

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <mozzi_analog.h>

const int KNOB_PIN = 0; // set the input for the knob to analog pin 0
const int LDR_PIN = 1; // set the input for the Light Dependent Resistor to analog pin 1

// use: Oscil <table_size, update_rate> oscilName (wavetable)
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

unsigned char volume;

void setup(){
  Serial.begin(115200); // set up the Serial output so we can look at the analog levels
  setupFastAnalogRead(); // increases the speed of reading the analog input
  startMozzi(); // :))
}


void updateControl(){
  // read the light dependent resistor
  int knob_value = analogRead(KNOB_PIN); // value is 0-1023
  
  // map it to an 8 bit range for efficient calculations in updateAudio
  volume = map(knob_value, 0, 1023, 0, 255);  
  
  // print the value to the Serial monitor for debugging
  Serial.print("volume = ");
  Serial.print((int)volume);
  
  // read the light dependent resistor
  int light_level = analogRead(LDR_PIN); // value is 0-1023

  // print the value to the Serial monitor for debugging
  Serial.print("   light level = ");
  Serial.print(light_level);
  
  // set the frequency
  aSin.setFreq((int)light_level);
  
  /*
  // Suggestion: calibrate the light level to a better range of frequencies
  
  // map the observed range of input levels to a new chosen range
  light_level =  map(light_level,850,950,50,1000);
  
  // expand the responsiveness at the bright end (cast long to make room for large number)
  unsigned long adjusted = (unsigned long) light_level * light_level;
  Serial.print("   adjusted = ");
  Serial.print(adjusted);
  
  // move it back down to a usable range
  adjusted /= 500;
  Serial.print("   re-adjusted = ");
  Serial.print(adjusted);

  // set the frequency (comment out the previous aSin.setFreq to use this)
  aSin.setFreq((int)adjusted);
  */
  
  Serial.println(); // next line
}


int updateAudio(){
  // cast char output from aSin.next() to int to make room for multiplication
  return ((int)aSin.next() * volume)>>8; // shift back into range after multiplying by 8 bit value
}


void loop(){
  audioHook(); // required here
}
