/*  Example of pulse width modulation,
    using Mozzi sonification library.

    Based Miller Puckette's j03.pulse.width.mod example
    in the Pure Data documentation, subtracting 2 sawtooth
    waves with slightly different tunings to produce a
    varying phase difference.

    Demonstrates Phasor().

    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.com/Mozzi/

		Mozzi documentation/API
		https://sensorium.github.io/Mozzi/doc/html/index.html

		Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2012, CC by-nc-sa.
*/

#include <MozziGuts.h>
#include <Phasor.h>

Phasor <AUDIO_RATE> aPhasor1;
Phasor <AUDIO_RATE> aPhasor2;

float freq = 55.f;

void setup(){
  aPhasor1.setFreq(freq);
  aPhasor2.setFreq(freq+0.2f);
  startMozzi(); // :)
}


void updateControl(){
}


int updateAudio(){
  char asig1 = (char)(aPhasor1.next()>>24);
  char asig2 = (char)(aPhasor2.next()>>24);
  return ((int)asig1-asig2)/2;
}


void loop(){
  audioHook(); // required here
}
