/*  Test of audio and control rate analog input using Mozzi sonification library.
 
 An audio input using the range between 0 to 5V on analog pin A0
 is sampled and output on digital pin 9.  
 All the other analog channels are sampled at control rate and printed to Serial.
 
 Configuration: requires "#define USE_AUDIO_INPUT true" to be in
 Mozzi/mozzi_config.h file.
 
 Circuit: 
 Audio cable centre wire on pin A0, outer shielding to Arduino Ground.
 Audio output on digital pin 9 (on a Uno or similar), or
 check the README or http://sensorium.github.com/Mozzi/
 Analog sensor inputs on any other analog input pins.
 The serial printing might cause glitches, so try commenting 
 them out to test if this is a problem.
 
 Mozzi help/discussion/announcements:
 https://groups.google.com/forum/#!forum/mozzi-users
 
 */

#include <MozziGuts.h>
#include <mozzi_analog.h>


void setup(){
  Serial.begin(115200);
  Serial.print("num_analog_inputs ");
  Serial.println(NUM_ANALOG_INPUTS);
  startMozzi();
}


void updateControl(){

  for (int i=1;i<NUM_ANALOG_INPUTS;i++){ // channel 0 is used for audio
    // adcGetResult(n) gets the most recent reading for analog channel n
    Serial.print(adcGetResult(i));
    Serial.print("\t"); // tab
  }
  Serial.println();

  // start the next read cycle in the background
  adcReadAllChannels();
}


int updateAudio(){
  int asig = getAudioInput(); // range 0-1023
  asig = asig - 512; // now range is -512 to 511
  // output range in STANDARD mode is -244 to 243, 
  // so you might need to adjust your signal to suit
  return asig;
}


void loop(){
  audioHook();
}





