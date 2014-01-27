/*
   Example using a piezo to trigger an audio sample to play,
   and a knob to set the playback pitch,
   with Mozzi sonification library.
 
   Demonstrates one-shot samples and analog input for control.
   
   This example goes with a tutorial on the Mozzi site:
   http://sensorium.github.io/Mozzi/learn/Mozzi_Introductory_Tutorial.pdf
  
   The circuit:
     Audio output on digital pin 9 (on a Uno or similar), or 
     check the README or http://sensorium.github.com/Mozzi/

     Potentiometer connected to analog pin 0.
       Center pin of the potentiometer goes to the analog pin.
       Side pins of the potentiometer go to +5V and ground
 
     Piezo on analog pin 3:
       + connection of the piezo attached to the analog pin
       - connection of the piezo attached to ground
       1-megohm resistor between the analog pin and ground
 
   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users
 
   Tim Barrass 2013.
   This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Sample.h> // Sample template
#include <samples/burroughs1_18649_int8.h> // a converted audio sample included in the Mozzi download

const char KNOB_PIN = 0;  // set the analog input pin for the knob
const char PIEZO_PIN = 3;  // set the analog input pin for the piezo 
const int threshold = 80;  // threshold value to decide when the detected signal is a knock or not

// use: Sample <table_size, update_rate> SampleName (wavetable)
Sample <BURROUGHS1_18649_NUM_CELLS, AUDIO_RATE> aSample(BURROUGHS1_18649_DATA);
float recorded_pitch = (float) BURROUGHS1_18649_SAMPLERATE / (float) BURROUGHS1_18649_NUM_CELLS;

boolean triggered = false;

void setup(){
  Serial.begin(115200); // set up the Serial output so we can look at the piezo values
  startMozzi(); // :))
}


void updateControl(){
  // read the knob
  int knob_value = mozziAnalogRead(KNOB_PIN); // value is 0-1023
  
  // map it to values between 0.1 and about double the recorded pitch
  float pitch = (recorded_pitch * (float) knob_value / 512.f) + 0.1f; 
  
  // set the sample playback frequency
  aSample.setFreq(pitch);
  
  // read the piezo
  int piezo_value = mozziAnalogRead(PIEZO_PIN); // value is 0-1023

  // print the value to the Serial monitor for debugging
  Serial.print("piezo value = ");
  Serial.print(piezo_value);

// only trigger once each time the piezo goes over the threshold
  if (piezo_value>threshold) {
    if (!triggered){
      aSample.start();
      triggered = true;
    }
  }else{
    triggered = false;
  }
 
  Serial.println(); // next line
}


int updateAudio(){
  return aSample.next();
}


void loop(){
  audioHook();
}

