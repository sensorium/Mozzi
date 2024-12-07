/*  Example of constant power panning to test stereo output hack,
    using Mozzi sonification library.

    Tests stereo output.

    NOTE: Stereo output is not supported on all platforms / configurations!

    Circuit: Audio outputs on digital pins 9 and 10 (on classic Arduino).

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2017-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

// Configure Mozzi for Stereo output. This must be done before #include <Mozzi.h>
// MozziConfigValues.h provides named defines for available config options.
#include <MozziConfigValues.h>
#define MOZZI_AUDIO_CHANNELS MOZZI_STEREO

#include <Mozzi.h>
#include <Oscil.h> // oscil for audio sig
#include <tables/pinknoise8192_int8.h> // table for audio oscillator
#include <tables/sin2048_int8.h> // sine table for pan oscillator

// use: Oscil <table_size, update_rate> oscilName (wavetable)
Oscil <PINKNOISE8192_NUM_CELLS, MOZZI_AUDIO_RATE> aNoise(PINKNOISE8192_DATA);

Oscil <SIN2048_NUM_CELLS, MOZZI_CONTROL_RATE> kPan(SIN2048_DATA);

byte ampA, ampB; // convey amplitudes from updateControl() to updateAudioStereo();

void setup()
{
  kPan.setFreq(0.25f);
  aNoise.setFreq(2.111f); // set the frequency with an unsigned int or a float
  startMozzi(); // :)
  Serial.begin(115200);
}


void updateControl()
{
  // angle 0-90 deg (in rads) (https://dsp.stackexchange.com/questions/21691/algorithm-to-pan-audio/21736)
  float pan_angle = (float)(kPan.next() + 128) * (1.571f / 256.f);
  // cheap equal power panning from above site, put into 0-255 range for fast audio calcs
  ampA = (char)(sin(pan_angle) * 255);
  ampB = (char)(cos(pan_angle) * 255);
  Serial.print(ampA);
  Serial.print("  ");
  Serial.println(ampB);
}


AudioOutput updateAudio() {
  int asig = aNoise.next();
  return StereoOutput::from16Bit(asig*ampA, asig*ampB);
}


void loop() {
  audioHook(); // required here
}
