/* Example crudely panning noise to test stereo output,
   using Mozzi sonification library.

   Tests stereo output.

   NOTE: Stereo output is not supported on all platforms / configurations!

   Circuit: Audio outputs on digital pins 9 and 10 (on classic Arduino).

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2012-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 */

// Configure Mozzi for Stereo output. This must be done before #include <Mozzi.h>
// MozziConfigValues.h provides named defines for available config options.
#include <MozziConfigValues.h>
#define MOZZI_AUDIO_CHANNELS MOZZI_STEREO

#include <Mozzi.h>
#include <Phasor.h> // for controlling panning position
#include <Oscil.h> // oscil for audio sig
#include <tables/pinknoise8192_int8.h> // table for oscillator

// use: Oscil <table_size, update_rate> oscilName (wavetable)
Oscil <PINKNOISE8192_NUM_CELLS, MOZZI_AUDIO_RATE> aNoise(PINKNOISE8192_DATA);

//Phasor for panning
Phasor <MOZZI_CONTROL_RATE> kPan; // outputs an unsigned long 0-max 32 bit positive number
unsigned int pan; // convey pan from updateControl() to updateAudioStereo();


void setup(){
  aNoise.setFreq(2.111f); // set the frequency with an unsigned int or a float
  kPan.setFreq(0.25f); // take 4 seconds to move left-right
  startMozzi(); // :)
  Serial.begin(115200);
}


void updateControl(){
  pan = kPan.next()>>16;
  Serial.println(pan);
}

AudioOutput updateAudio(){
  int asig = aNoise.next();
  return StereoOutput::fromNBit(24, (long)pan*asig, (long)(65535-pan)*asig);
}


void loop(){
  audioHook(); // required here
}
