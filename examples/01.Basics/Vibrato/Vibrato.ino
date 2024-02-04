/*  Example playing a sinewave with vibrato,
    using Mozzi sonification library.

    Demonstrates simple FM using phase modulation.

    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.io/Mozzi/

		Mozzi documentation/API
		https://sensorium.github.io/Mozzi/doc/html/index.html

		Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2012, CC by-nc-sa.
*/

#define MOZZI_CONTROL_RATE 64 // Hz, powers of 2 are most reliable
#include <Mozzi.h>
#include <Oscil.h>
#include <tables/cos2048_int8.h> // table for Oscils to play
#include <mozzi_midi.h> // for mtof
#include <mozzi_fixmath.h>

Oscil<COS2048_NUM_CELLS, MOZZI_AUDIO_RATE> aCos(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, MOZZI_AUDIO_RATE> aVibrato(COS2048_DATA);

const byte intensity = 255;

void setup(){
  startMozzi();
  aCos.setFreq(mtof(84.f));
  aVibrato.setFreq(15.f);
}


void updateControl(){
}


AudioOutput updateAudio(){
    Q15n16 vibrato = (Q15n16) intensity * aVibrato.next();
    return MonoOutput::from8Bit(aCos.phMod(vibrato)); // phase modulation to modulate frequency
}

void loop(){
  audioHook();
}
