/*  Plays a basic Shepard tone
    using Mozzi sonification library.

    https://en.wikipedia.org/wiki/Shepard_tone

    Demonstrates audio and control rate updates, Lines, EventDelay.
    More oscillators could be added for a more convincing effect.

    Important:
    This sketch uses MOZZI_OUTPUT_2PIN_PWM (aka HIFI) output mode, which
    is not available on all boards (among others, it works on the
    classic Arduino boards, but not Teensy 3.x and friends).

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

   Copyright 2018-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/


#include <MozziConfigValues.h>
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_2PIN_PWM
#define MOZZI_CONTROL_RATE 128

#include <Mozzi.h>
#include <Oscil.h>
#include <tables/sin8192_int8.h>
#include <tables/triangle512_int8.h>
#include <mozzi_midi.h>
#include <EventDelay.h>
#include <Line.h>
#include <FixMath.h>

// reset and sync vol and freq controls each cycle
EventDelay  kTriggerDelay0;
EventDelay  kTriggerDelay1;


const UFix<7,0> NOTE_CENTRE = 60, NOTE_RANGE = 12;
const UFix<7,0> NOTE_START_FIXEDPOINT = NOTE_CENTRE + NOTE_RANGE;
const UFix<7,0> NOTE_END_FIXEDPOINT = NOTE_CENTRE - NOTE_RANGE;

#define GLISS_SECONDS 3.f
//#define CONTROL_STEPS_PER_GLISS ((unsigned int)((float)MOZZI_CONTROL_RATE * GLISS_SECONDS))
#define CONTROL_STEPS_PER_GLISS ((unsigned int)((float)MOZZI_CONTROL_RATE * GLISS_SECONDS))


// use: Line <type> lineName
// The lines needs more precision than the notes, as they interpolate in between
// here we optimize by choosing the max precision to stay in 16bits (7+9=16)
// note that for an SFix, we would aim for 15 (plus a sign bit).
Line <UFix<7,9>> kGliss0; // Line to slide frequency
Line <UFix<7,9>> kGliss1; // Line to slide frequency

// harmonics
Oscil<SIN8192_NUM_CELLS, MOZZI_AUDIO_RATE> aCos0(SIN8192_DATA);
Oscil<SIN8192_NUM_CELLS, MOZZI_AUDIO_RATE> aCos1(SIN8192_DATA);

// volume envelope oscils
Oscil<TRIANGLE512_NUM_CELLS, MOZZI_CONTROL_RATE> kVol0(TRIANGLE512_DATA);
Oscil<TRIANGLE512_NUM_CELLS, MOZZI_CONTROL_RATE> kVol1(TRIANGLE512_DATA);

// audio volumes updated each control interrupt and reused in audio
SFix<0,14> v0, v1; 
//SFix<0,7> v0, v1; // micro-optimization

void setup() {
//Serial.begin(115200);

  kVol0.setFreq(0.5f / GLISS_SECONDS);
  kVol1.setFreq(0.5f / GLISS_SECONDS);

  kVol0.setPhase(0);
  kVol1.setPhase((unsigned int)TRIANGLE512_NUM_CELLS / 4);

  kTriggerDelay0.start(0); // start trigger before polling in updateControl()
  kTriggerDelay1.start((int)((GLISS_SECONDS * 1000.f) / 2.f));

  startMozzi();
}

void updateControl()
{
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

  auto gliss0 = kGliss0.next(); // fixed point
  auto gliss1 = kGliss1.next(); // fixed point

  aCos0.setFreq(mtof(gliss0));
  aCos1.setFreq(mtof(gliss1));

  v0 = toSFraction(kVol0.next()); // convert as a pure fractionnal between -1 and 1
  v1 = toSFraction(kVol1.next());

  // square for perceptual volume (also makes it positive...)
  v0 = v0 * v0;
  v1 = v1 * v1;
}

AudioOutput updateAudio() {
  auto asig = v0 * toSInt(aCos0.next()) + v1 * toSInt(aCos1.next());
  return AudioOutput::fromSFix(asig); // auto-scaling of the output with SFix
}

void loop() {
  audioHook();
}
