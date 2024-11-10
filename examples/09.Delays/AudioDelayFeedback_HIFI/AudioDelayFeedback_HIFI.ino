/*  Example of flanging,
    using Mozzi sonification library.

    Demonstrates AudioDelayFeedback.

    This sketch uses MOZZI_OUTPUT_2PIN_PWM (aka HIFI) output mode, which
    is not available on all boards (among others, it works on the
    classic Arduino boards, but not Teensy 3.x and friends).

    Important:
    The MOZZI_AUDIO_RATE (sample rate) is additionally configured at 32768 Hz,
    which is the default on most platforms, but twice the standard rate on
    AVR-CPUs. Try chaging it back to hear the difference.

    Circuit: Audio output on digital pin 9 and 10 (on a Uno or similar),
    Check the Mozzi core module documentation for others and more detail

                     3.9k
     pin 9  ---WWWW-----|-----output
                    499k           |
     pin 10 ---WWWW---- |
                                       |
                             4.7n  ==
                                       |
                                   ground

    Resistors are ±0.5%  Measure and choose the most precise
    from a batch of whatever you can get.  Use two 1M resistors
    in parallel if you can't find 499k.
    Alternatively using 39 ohm, 4.99k and 470nF components will
    work directly with headphones.

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2013-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#include <MozziConfigValues.h>
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_2PIN_PWM
#define MOZZI_AUDIO_RATE 32768
#define MOZZI_CONTROL_RATE 128 // Hz, powers of 2 are most reliable

#include <Mozzi.h>
#include <Oscil.h>
#include <tables/triangle_analogue512_int8.h> // wavetable
#include <tables/triangle512_int8.h> // wavetable
#include <AudioDelayFeedback.h>
#include <mozzi_midi.h> // for mtof

Oscil<TRIANGLE_ANALOGUE512_NUM_CELLS, MOZZI_AUDIO_RATE> aTriangle(TRIANGLE_ANALOGUE512_DATA); // audio oscillator
Oscil<TRIANGLE512_NUM_CELLS, MOZZI_CONTROL_RATE> kDelSamps(TRIANGLE512_DATA); // for modulating delay time, measured in audio samples

AudioDelayFeedback <128> aDel;

// the delay time, measured in samples, updated in updateControl, and used in updateAudio
byte del_samps;
Q16n16 del_samps_fractional;

void setup(){
  startMozzi();
  aTriangle.setFreq(mtof(48.f));
  kDelSamps.setFreq(.163f); // set the delay time modulation frequency (ie. the sweep frequency)
  aDel.setFeedbackLevel(-111); // can be -128 to 127
}


void updateControl(){
  // delay time range from 0 to 127 samples, @ 16384 samps per sec = 0 to 7 milliseconds
  //del_samps = 64+kDelSamps.next();

  // delay time range from 1 to 33 samples, @ 16384 samps per sec = 0 to 2 milliseconds
  //del_samps = 17+kDelSamps.next()/8;

  del_samps_fractional = Q8n0_to_Q16n16(17) + ((long)kDelSamps.next()<<12);

}


AudioOutput updateAudio(){
  int8_t asig = aTriangle.next(); // get this so it can be used twice without calling next() again
  //return asig/8 + aDel.next(asig, del_samps); // mix some straight signal with the delayed signal
  //return aDel.next(aTriangle.next(), del_samps); // instead of the previous 2 lines for only the delayed signal
  return MonoOutput::fromAlmostNBit(13, asig + (aDel.next(asig, del_samps_fractional)<<4)); // mix some straight signal with the delayed signal
}


void loop(){
  audioHook();
}
