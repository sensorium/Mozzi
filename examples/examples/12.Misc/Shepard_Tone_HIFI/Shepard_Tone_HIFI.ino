/*  Plays a basic Shepard tone
    using Mozzi sonification library.

    https://en.wikipedia.org/wiki/Shepard_tone

    Demonstrates audio and control rate updates, Lines, EventDelay.
    More oscillators could be added for a more convincing effect.

    Important:
    #define AUDIO_MODE HIFI in mozzi_config.h

    Circuit: Audio output on digital pin 9 and 10 (on a Uno or similar),
    Check the Mozzi core module documentation for others and more detail

                     3.9k
     pin 9  ---WWWW-----|-----output
                    499k|
     pin 10 ---WWWW---- |
                        |
                  4.7n  ==
                        |
                      ground

		Mozzi documentation/API
		https://sensorium.github.io/Mozzi/doc/html/index.html

		Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2018, CC by-nc-sa.
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/sin8192_int8.h>
#include "triangle512_uint8.h"
#include <mozzi_midi.h>
#include <EventDelay.h>
#include <Line.h>
#include <mozzi_fixmath.h> // for fixed-point fractional numbers

#define CONTROL_RATE 128

// reset and sync vol and freq controls each cycle
EventDelay  kTriggerDelay0;
EventDelay  kTriggerDelay1;

#define NOTE_CENTRE 60
#define NOTE_RANGE 12

#define NOTE_START_FIXEDPOINT float_to_Q16n16(NOTE_CENTRE + NOTE_RANGE) // so Line gliss has enough precision
#define NOTE_END_FIXEDPOINT float_to_Q16n16(NOTE_CENTRE - NOTE_RANGE) // so Line gliss has enough precision

#define GLISS_SECONDS 3.f
#define CONTROL_STEPS_PER_GLISS ((unsigned int)((float)CONTROL_RATE * GLISS_SECONDS))

// use: Line <type> lineName
// audio oscils
Line <Q16n16> kGliss0; // Line to slide frequency
Line <Q16n16> kGliss1; // Line to slide frequency

// harmonics
Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> aCos0(SIN8192_DATA);
Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> aCos1(SIN8192_DATA);

// volume envelope oscils
Oscil<TRIANGLE512_NUM_CELLS, CONTROL_RATE> kVol0(TRIANGLE512_DATA);
Oscil<TRIANGLE512_NUM_CELLS, CONTROL_RATE> kVol1(TRIANGLE512_DATA);

// audio volumes updated each control interrupt and reused in audio
long v0, v1;

void setup() {
  //Serial.begin(115200);

  // set volume change frequencies
  kVol0.setFreq(0.5f / GLISS_SECONDS);
  kVol1.setFreq(0.5f / GLISS_SECONDS);

  kVol0.setPhase(0);
  kVol1.setPhase((unsigned int)TRIANGLE512_NUM_CELLS / 4);

  kTriggerDelay0.start(0); // start trigger before polling in updateControl()
  kTriggerDelay1.start((int)((GLISS_SECONDS * 1000.f) / 2.f));

  startMozzi(CONTROL_RATE);
}


void updateControl() {

  if (kTriggerDelay0.ready()) {
    kGliss0.set(NOTE_START_FIXEDPOINT, NOTE_END_FIXEDPOINT, CONTROL_STEPS_PER_GLISS);
    kVol0.setPhase(0);
    kTriggerDelay0.start((int)(GLISS_SECONDS * 1000.f)); // milliseconds
    Serial.println("gliss1");
  }

  if (kTriggerDelay1.ready()) {
    kGliss1.set(NOTE_START_FIXEDPOINT, NOTE_END_FIXEDPOINT, CONTROL_STEPS_PER_GLISS);
    kVol1.setPhase(0);
    kTriggerDelay1.start((int)(GLISS_SECONDS * 1000.f)); // milliseconds
    Serial.println("\t gliss2");
  }

  // set harmonic frequencies
  Q16n16 gliss0 = kGliss0.next(); // fixed point
  float freq0 = Q16n16_to_float(Q16n16_mtof(gliss0)); // convert fixed point to floating point

  Q16n16 gliss1 = kGliss1.next(); // fixed point
  float freq1 = Q16n16_to_float(Q16n16_mtof(gliss1)); // convert fixed point to floating point

  aCos0.setFreq(freq0);
  aCos1.setFreq(freq1);

  v0 = kVol0.next();
  v1 = kVol1.next();
 // Serial.print((byte)v0); Serial.print("\t"); Serial.println((byte)v1);

  // square for perceptual volume
  v0 *= v0;
  v1 *= v1;
}

int updateAudio() {
  long asig = ((v0 * aCos0.next()) >> 8) +
              ((v1 * aCos1.next()) >> 8);
  asig >>= 1;
  return (int) asig;
}


void loop() {
  audioHook();
}
