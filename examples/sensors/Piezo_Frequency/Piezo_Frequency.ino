/*
 Example using a piezo to change the frequency of a sinewave
 with Mozzi sonification library.
 
 Demonstrates the use of Oscil to play a wavetable, and analog input for control.
 
 The circuit:
 * Audio output on digital pin 9 (on a Uno or similar), or 
 check the README or http://sensorium.github.com/Mozzi/
 
 Piezo:
 + connection of the piezo attached to analog pin 2
 - connection of the piezo attached to ground
 1-megohm resistor attached from analog pin 2 to ground
 
 Mozzi help/discussion/announcements:
 https://groups.google.com/forum/#!forum/mozzi-users
 
 Tim Barrass 2013.
 This example code is in the public domain.
 */

#include <MozziGuts.h>
#include <Oscil.h> // oscillator 
#include <tables/sin2048_int8.h> // table for Oscils to play
#include <mozzi_analog.h> // fast functions for reading analog inputs 
#include <Smooth.h>

// increase the rate of updateControl from the default of 64, to catch the piezo's rapid transients
#define CONTROL_RATE 128 

const int PIEZO_PIN = 3;  // set the analog input pin for the piezo 

// use: Oscil <table_size, update_rate> oscilName (wavetable)
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

float smoothness = 0.995f;
Smooth <int> kSmooth(smoothness);

void setup(){
  Serial.begin(115200); // set up the Serial output so we can look at the piezo values
  setupFastAnalogRead(); // one way of increasing the speed of reading the input
  startMozzi(CONTROL_RATE); // :)) use the new control rate
}


void updateControl(){
  // read the piezo
  int piezo_value = analogRead(PIEZO_PIN); // value is 0-1023

  // print the value to the Serial monitor for debugging
  Serial.print("piezo value = ");
  Serial.print(piezo_value);

  // set the frequency
  aSin.setFreq(piezo_value*3);

/*
  // Suggestion: smooth the piezo signal
  aSin.setFreq(kSmooth.next(piezo_value*3));
*/

/*
  // Suggestion: freq fast on the way up, slow on the way down
  static int previous_freq; // a local static variable will keep its value between calls
  int freq = piezo_value*3;
  if (freq<previous_freq) freq = previous_freq-3;
  previous_freq = freq;
  
  // print the value to the Serial monitor for debugging
  Serial.print("  freq = ");
  Serial.print(freq);
  
  aSin.setFreq(freq);
  */
  
  Serial.println(); // next line
}


int updateAudio(){
  return aSin.next();
}


void loop(){
  audioHook();
}







