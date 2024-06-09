/*  Test of audio input using Mozzi sonification library.

 An audio input using the range between 0 to 5V on analog pin A0 (or as
 set in MOZZI_AUDIO_INPUT_PIN) is sampled and output on digital pin 9.

 NOTE: MOZZI_AUDIO_INPUT_STANDARD is not available as an option on all
 platforms.

 Circuit:
 Audio cable centre wire on pin A0, outer shielding to Arduino Ground.
 Audio output on DAC/A14 on Teensy 3.0, 3.1, or digital pin 9 on a Uno or similar, or
 check the README or http://sensorium.github.io/Mozzi/

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2013-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#include <MozziConfigValues.h>
#define MOZZI_AUDIO_INPUT MOZZI_AUDIO_INPUT_STANDARD
#define MOZZI_AUDIO_INPUT_PIN 0

#include <Mozzi.h>

void setup(){
  startMozzi();
}


void updateControl(){
}


AudioOutput updateAudio(){
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
