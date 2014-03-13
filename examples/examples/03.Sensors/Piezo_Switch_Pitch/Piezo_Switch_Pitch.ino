/*
  Example using a piezo to trigger an audio sample 
  and a switch to change the playback pitch.
  using Mozzi sonification library.
  
  Demonstrates one-shot samples, analog input for triggering,
  and digital input for switching.
  
  This example goes with a tutorial on the Mozzi site:
  http://sensorium.github.io/Mozzi/learn/Mozzi_Introductory_Tutorial.pdf
  
  The circuit:
    Audio output on digital pin 9 (on a Uno or similar), or 
    check the README or http://sensorium.github.com/Mozzi/
  
    Piezo on analog pin A3:
      + connection of the piezo attached to the analog pin
      - connection of the piezo attached to ground
      1-megOhm resistor attached from the analog pin to ground
    
    Pushbutton on digital pin D4
      button between the digital pin and +5V
      10K resistor from the digital pin to ground
  
  Mozzi help/discussion/announcements:
  https://groups.google.com/forum/#!forum/mozzi-users
  
  Tim Barrass 2013.
  This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Sample.h> // Sample template
#include <samples/burroughs1_18649_int8.h> // a converted audio sample included in the Mozzi download

const int PIEZO_PIN = 3;  // set the analog input pin for the piezo 
const int threshold = 80;  // threshold value to decide when the detected signal is a knock or not

const int BUTTON_PIN = 4;  // set the digital input pin for the button

// use: Sample <table_size, update_rate> SampleName (wavetable)
Sample <BURROUGHS1_18649_NUM_CELLS, AUDIO_RATE> aSample(BURROUGHS1_18649_DATA);
float recorded_pitch = (float) BURROUGHS1_18649_SAMPLERATE / (float) BURROUGHS1_18649_NUM_CELLS;

char button_state, previous_button_state; // variable for reading the pushbutton status       
char latest_button_change;
boolean triggered = false;
float pitch, pitch_change;

void setup(){
  Serial.begin(115200); // set up the Serial output so we can look at the piezo values
  startMozzi(); // :))
}


void buttonChangePitch(){
  // read the state of the pushbutton value:
  button_state = digitalRead(BUTTON_PIN);

  // has the button state changed?
  if(button_state != previous_button_state){

    // if the latest change was a press, pitch up, else pitch down
    if (button_state == HIGH) {
      pitch = 2.f * recorded_pitch;
    } 
    else {
      pitch = 0.5f * recorded_pitch;
    }
  }
  previous_button_state = button_state;
}



void updateControl(){
  // read the piezo
  int piezo_value = mozziAnalogRead(PIEZO_PIN); // value is 0-1023

  // print the value to the Serial monitor for debugging
  Serial.print("piezo value = ");
  Serial.print(piezo_value);

  // only trigger once each time the piezo goes over the threshold
  if (piezo_value>threshold) {
    if (!triggered){
      pitch = recorded_pitch;
      aSample.start();
      triggered = true;
    }
  }
  else{
    triggered = false;
  }

  buttonChangePitch();
  aSample.setFreq(pitch);

  Serial.println(); // next line
}


int updateAudio(){
  return aSample.next();
}


void loop(){
  audioHook();
}










