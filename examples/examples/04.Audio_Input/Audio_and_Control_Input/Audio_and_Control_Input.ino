/*  Test of audio and control rate analog input using Mozzi sonification library.

  An audio input using the range between 0 to 5V on analog pin A0
  is sampled and output on digital pin 9.
  All the other analog channels are sampled at control rate and printed to Serial.

  Demonstrates mozziAnalogRead(pin), which reads analog inputs in the background,
  and getAudioInput(), which samples audio on MOZZI_AUDIO_INPUT_PIN, configured below.

  NOTE: MOZZI_AUDIO_INPUT_STANDARD is not available as an option on all
  platforms.

  Circuit:
  Audio cable centre wire on pin A0, outer shielding to Arduino Ground.
  Audio output on DAC/A14 on Teensy 3.0, 3.1, or digital pin 9 on a Uno or similar, or
  check the README or http://sensorium.github.io/Mozzi/
  Analog sensor inputs on any other analog input pins.
  The serial printing might cause glitches, so try commenting
  them out to test if this is a problem.

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


void setup() {
  //Serial.begin(9600); // for Teensy 3.1, beware printout can cause glitches
  Serial.begin(115200);
  Serial.print("num_analog_inputs = ");
  Serial.println(NUM_ANALOG_INPUTS);
  startMozzi();
}


void updateControl(){
  for (int i=1;i<NUM_ANALOG_INPUTS;i++) { // analog 0 is configured for audio
    Serial.print(mozziAnalogRead16(i)); // mozziAnalogRead is better than analogRead
    Serial.print("\t"); // tab
  }
  Serial.println();
}


AudioOutput updateAudio(){
  int asig = getAudioInput(); // range 0-1023
  asig = asig - 512; // now range is -512 to 511
  return MonoOutput::fromAlmostNBit(9, asig).clip();
}


void loop(){
  audioHook();
}
