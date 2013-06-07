/*
   Example using a piezo to scrub through 2 audio samples 
   and a switch to choose which sample to play,
   using Mozzi sonification library.
 
   Demonstrates playing samples at an offset from the beginning,
   analog input for continuous transitions and digital input for switching.
 
   The circuit:
   * Audio output on digital pin 9 (on a Uno or similar), or 
     check the README or http://sensorium.github.com/Mozzi/
 
   Piezo on analog pin A3:
   + connection of the piezo attached to the analog pin
   - connection of the piezo attached to ground
   1-megOhm resistor attached from the analog pin to ground
 
   Pushbutton on digital pin D4
   *  button attached to the digital pin from +5V
   * 10K resistor attached to the digital pin from ground
 
 Mozzi help/discussion/announcements:
 https://groups.google.com/forum/#!forum/mozzi-users
 
 Tim Barrass 2013.
 This example code is in the public domain.
 */

#include <MozziGuts.h>
#include <Sample.h> // Sample template
#include <samples/burroughs2_8192_int8.h> // a converted audio sample included in the Mozzi download
#include <tables/horse_lips_8192_int8.h> // table for Sample
#include <mozzi_analog.h> // fast functions for reading analog inputs 
#include <Smooth.h>

const int PIEZO_PIN = 3;  // set the analog input pin for the piezo 
const int threshold = 50;  // threshold value to decide when the detected signal is a knock or not

const int BUTTON_PIN = 4;  // set the digital input pin for the button

// for smoothing the piezo signal
float smoothness = 0.99f;
Smooth <unsigned int> kSmooth(smoothness);

// use: Sample <table_size, update_rate> SampleName (wavetable)
Sample <BURROUGHS2_8192_NUM_CELLS, AUDIO_RATE> burroughs(BURROUGHS2_8192_DATA);
Sample <HORSE_LIPS_8192_NUM_CELLS, AUDIO_RATE> horse(HORSE_LIPS_8192_DATA);


char burroughs_gain, horse_gain; // variable volume levels for each sample
char button_state; // variable for reading the pushbutton status       

void setup(){
  Serial.begin(115200); // set up the Serial output so we can look at the piezo values
  setupFastAnalogRead(); // one way of increasing the speed of reading the input
  burroughs.setFreq((float) BURROUGHS2_8192_SAMPLERATE / (float) BURROUGHS2_8192_NUM_CELLS); // play at the speed it was recorded
  horse.setFreq((float) HORSE_LIPS_8192_SAMPLERATE / (float) HORSE_LIPS_8192_NUM_CELLS); // play at the speed it was recorded at
  startMozzi(); // :))
}


void updateControl(){
  // read the piezo
  int piezo_value = analogRead(PIEZO_PIN); // value is 0-1023

  // read the state of the pushbutton value:
  button_state = digitalRead(BUTTON_PIN);

  // print the value to the Serial monitor for debugging
  Serial.print("piezo value = ");
  Serial.print(piezo_value);

  unsigned int start_point = piezo_value *100; // calibrate here
  // smooth the start_point changes
  start_point = kSmooth.next(start_point);
  
    // check if the pushbutton is pressed.
    // if it is, the button_state is HIGH:
    if (button_state == HIGH) {
      burroughs_gain =1;
      horse_gain = 0;
      burroughs.start(start_point);
    } 
    else {
      burroughs_gain =0;
      horse_gain = 1;
      horse.start(start_point);
    }

  Serial.println(); // next line
}


int updateAudio(){
  return (int) burroughs.next()*burroughs_gain + horse.next()*horse_gain;
}


void loop(){
  audioHook();
}



