/*
   Example using a piezo to scrub through an audio sample,
 using Mozzi sonification library.
 
 Demonstrates playing a sample at an offset from the beginning,
 analog input for continuous transitions and digital input for switching.
 
 The circuit:
 * Audio output on digital pin 9 (on a Uno or similar), or 
 check the README or http://sensorium.github.com/Mozzi/
 
 Piezo on analog pin A3:
 + connection of the piezo attached to the analog pin
 - connection of the piezo attached to ground
 1-megOhm resistor attached from the analog pin to ground
 
 Mozzi help/discussion/announcements:
 https://groups.google.com/forum/#!forum/mozzi-users
 
 Tim Barrass 2013.
 This example code is in the public domain.
 */

#include <MozziGuts.h>
#include <Sample.h> // Sample template
#include <samples/burroughs1_18649_int8.h> // a converted audio sample included in the Mozzi download
#include <tables/horse_lips_8192_int8.h> // table for Sample
#include <mozzi_analog.h> // fast functions for reading analog inputs 
#include <Smooth.h>

const int PIEZO_PIN = 3;  // set the analog input pin for the piezo 
const int threshold = 50;  // threshold value to decide when the detected signal is a knock or not

float smoothness = 0.99f;
Smooth <unsigned int> kSmooth(smoothness);

// use: Sample <table_size, update_rate> SampleName (wavetable)
Sample <BURROUGHS1_18649_NUM_CELLS, AUDIO_RATE> burroughs(BURROUGHS1_18649_DATA);

void setup(){
  Serial.begin(115200); // set up the Serial output so we can look at the piezo values
  setupFastAnalogRead(); // one way of increasing the speed of reading the input
  burroughs.setFreq((float) BURROUGHS1_18649_SAMPLERATE / (float) BURROUGHS1_18649_NUM_CELLS); // play at the speed it was recorded  
  startMozzi(); // :))
}


void updateControl(){
  // read the piezo
  int piezo_value = analogRead(PIEZO_PIN); // value is 0-1023

  // print the value to the Serial monitor for debugging
  Serial.print("piezo value = ");
  Serial.print(piezo_value);

  unsigned int start_point = piezo_value *400; // calibrate here
  burroughs.start(kSmooth.next(start_point));

  Serial.println(); // next line
}


int updateAudio(){
  return burroughs.next();
}


void loop(){
  audioHook();
}


