/*  Example of constant power panning to test stereo output hack,
    using Mozzi sonification library.

    Tests stereo output hack.  This requires #define STEREO_HACK true in mozzi_config.h

    Circuit: Audio outputs on digital pins 9 and 10.

		Mozzi documentation/API
		https://sensorium.github.io/Mozzi/doc/html/index.html

		Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2017.
    This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Oscil.h> // oscil for audio sig
#include <tables/pinknoise8192_int8.h> // table for audio oscillator
#include <tables/sin2048_int8.h> // sine table for pan oscillator

// use: Oscil <table_size, update_rate> oscilName (wavetable)
Oscil <PINKNOISE8192_NUM_CELLS, AUDIO_RATE> aNoise(PINKNOISE8192_DATA);

Oscil <SIN2048_NUM_CELLS, CONTROL_RATE> kPan(SIN2048_DATA);

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


// needed for stereo output
int audio_out_1, audio_out_2;

void updateAudio() {
  int asig = aNoise.next();
  audio_out_1 = (asig * ampA) >> 8;
  audio_out_2 = (asig * ampB) >> 8;
}


void loop() {
  audioHook(); // required here
}
