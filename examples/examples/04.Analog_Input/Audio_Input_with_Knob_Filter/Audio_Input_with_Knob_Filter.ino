/*  
  Test of audio input and processing with control input,
  using Mozzi sonification library.

  Configuration: requires "#define USE_AUDIO_INPUT true" to be in
  Mozzi/mozzi_config.h file.

  Circuit: 
  * Audio input on pin 0
  * output on digital pin 9 (on a Uno or similar), or
    check the README or http://sensorium.github.com/Mozzi/

  *  Potentiometer connected to analog pin A1.
  *  Center pin of the potentiometer goes to the analog pin.
  *  Side pins of the potentiometer go to +5V and ground

  Mozzi help/discussion/announcements:
  https://groups.google.com/forum/#!forum/mozzi-users

*/

#include <MozziGuts.h>
#include <LowPassFilter.h>
#include <mozzi_analog.h>

#define KNOB_PIN 1

LowPassFilter lpf;


// no special setup required for analog input if USE_AUDIO_INPUT=true in mozzi_config.h
void setup(){
  lpf.setResonance(220);
  startMozzi();
}



void updateControl(){
  int knob = adcGetResult(KNOB_PIN); // gets whatever the last conversion was
  adcReadAllChannels(); // for next time through
  unsigned char cutoff_freq = knob>>2; // range 0-255
  lpf.setCutoffFreq(cutoff_freq);
}


int updateAudio(){
  // subtracting 512 moves the unsigned audio data into 0-centred, 
  // signed range required by all Mozzi units
  int asig = getAudioInput()-512;
  asig = lpf.next(asig>>1);
  return asig;
}


void loop(){
  audioHook();
}




