/*  Example of a modulating timbre with a rhythmic
    volume envelope, using Mozzi sonification library.


    Demonstrates phase modulation and modifying
    the volume of a sound with an envelope setd in a table.

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2012-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#define MOZZI_CONTROL_RATE 640 // quite fast, keeps modulation smooth
#include <Mozzi.h>
#include <Oscil.h>
#include <tables/cos8192_int8.h>
#include <tables/envelop2048_uint8.h>

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <COS8192_NUM_CELLS, MOZZI_AUDIO_RATE> aCarrier(COS8192_DATA);
Oscil <COS8192_NUM_CELLS, MOZZI_AUDIO_RATE> aModulator(COS8192_DATA);
Oscil <COS8192_NUM_CELLS, MOZZI_AUDIO_RATE> aModWidth(COS8192_DATA);
Oscil <COS8192_NUM_CELLS, MOZZI_CONTROL_RATE> kModFreq1(COS8192_DATA);
Oscil <COS8192_NUM_CELLS, MOZZI_CONTROL_RATE> kModFreq2(COS8192_DATA);
Oscil <ENVELOP2048_NUM_CELLS, MOZZI_AUDIO_RATE> aEnvelop(ENVELOP2048_DATA);


void setup() {
  startMozzi();
  aCarrier.setFreq(220);
  kModFreq1.setFreq(1.78f);
  kModFreq2.setFreq(0.1757f);
  aModWidth.setFreq(0.1434f);
  aEnvelop.setFreq(9.0f);
}


void updateControl() {
  aModulator.setFreq(277.0f + 0.4313f*kModFreq1.next() + kModFreq2.next());
}


AudioOutput updateAudio(){
  int asig = aCarrier.phMod((int)aModulator.next()*(260u+aModWidth.next()));
  return MonoOutput::from16Bit(asig*(byte)aEnvelop.next());
}


void loop() {
  audioHook();
}
