/*  Example of filtering audio noise with a resonant filter,
    using Mozzi sonification library.

    Demonstrates StateVariable() with acute resonance,
    which in this case requires the input signal level to be reduced
    to avoid distortion which can occur with sharp resonance settings.

    Important:
    This sketch uses MOZZI_OUTPUT_2PIN_PWM (aka HIFI) output mode, which
    is not available on all boards (among others, it works on the
    classic Arduino boards, but not Teensy 3.x and friends).

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

   Copyright 2012-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#include <MozziConfigValues.h>
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_2PIN_PWM

#include <Mozzi.h>
#include <Oscil.h>
#include <tables/whitenoise8192_int8.h>
#include <StateVariable.h>

Oscil <WHITENOISE8192_NUM_CELLS, MOZZI_AUDIO_RATE> aNoise(WHITENOISE8192_DATA); // audio noise
StateVariable <BANDPASS> svf; // can be LOWPASS, BANDPASS, HIGHPASS or NOTCH


void setup(){
  startMozzi();
  aNoise.setFreq(1.27f*(float)MOZZI_AUDIO_RATE/WHITENOISE8192_SAMPLERATE);   // * by an oddish number (1.27) to avoid exact repeating of noise oscil
  svf.setResonance(1); // 0 to 255, 0 is the "sharp" end
  svf.setCentreFreq(3500);
}


void updateControl(){
}


AudioOutput updateAudio(){
  int input = aNoise.next()>>1; // shift down (ie. fast /) to avoid distortion with extreme resonant filter setting
  int output = svf.next(input);
  return MonoOutput::fromNBit(10, output);
}


void loop(){
  audioHook();
}
