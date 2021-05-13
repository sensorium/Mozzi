/*  Test of audio input using Mozzi sonification library.

 An audio input using the range between 0 to 5V on analog pin A0
 is sampled and output on digital pin 9.

  Configuration: requires these lines in the Mozzi/mozzi_config.h file:
  #define USE_AUDIO_INPUT true
  #define AUDIO_INPUT_PIN 0

 Circuit:
 Audio cable centre wire on pin A0, outer shielding to Arduino Ground.
 Audio output on DAC/A14 on Teensy 3.0, 3.1, or digital pin 9 on a Uno or similar, or
 check the README or http://sensorium.github.io/Mozzi/

 		Mozzi help/discussion/announcements:
  https://groups.google.com/forum/#!forum/mozzi-users

  Tim Barrass 2013, CC by-nc-sa.
*/

#include <MozziGuts.h>

void setup(){
  startMozzi();
}


void updateControl(){
}


AudioOutput_t updateAudio(){
  int asig = getAudioInput(); // range 0-1023
  asig = asig - 512; // now range is -512 to 511
  // output range in STANDARD mode is -244 to 243,
  // we scale down only by one bit, so you might need
  // to adjust your signal to suit
  return MonoOutput::fromAlmostNBit(9, asig).clip();
}


void loop(){
  audioHook();
}
