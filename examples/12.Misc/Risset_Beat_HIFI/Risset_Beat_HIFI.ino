/*  Example of Risset rhythm generated with Mozzi audio synthesis library.

    https://music.stackexchange.com/questions/4708/how-does-the-risset-rhythm-work

    Demonstrates Sample(), EventDelay(), Line(), fixed pint numbers and bit-shifting

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

    Resistors are ±0.5%
    Circuit: Audio output on digital pins 9 and 10 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.com/Mozzi/

		Mozzi documentation/API
		https://sensorium.github.io/Mozzi/doc/html/index.html

		Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2018, CC by-nc-sa.
*/

#include <MozziGuts.h>
#include <Sample.h> // Sample template
#include <samples/abomb16384_int8.h> // table for Sample
#include <EventDelay.h>
#include <Line.h>
#include <mozzi_fixmath.h> // for fixed-point fractional numbers

#define CONTROL_RATE 128

// use: Sample <table_size, update_rate> SampleName (wavetable)
Sample <ABOMB_NUM_CELLS, AUDIO_RATE> aSample0(ABOMB_DATA);
Sample <ABOMB_NUM_CELLS, AUDIO_RATE> aSample1(ABOMB_DATA);

// for scheduling changes
EventDelay  kTriggerDelay;

// use: Line <type> lineName
Line <Q16n16> kGliss;
Line <Q15n16> kLevel0;
Line <Q15n16> kLevel1;

#define FULL_VOLUME_FIXEDPOINT Q0n8_to_Q15n16(255)
#define ZERO_VOLUME_FIXEDPOINT Q0n8_to_Q15n16(1)

#define NUM_LOOPS_IN_GLISS 8

#define SAMPLE_LENGTH_SECONDS ((float) ABOMB_SAMPLERATE / (float) ABOMB_NUM_CELLS)

#define SAMPLE_FREQ (1.f/SAMPLE_LENGTH_SECONDS)

#define SAMPLE_FREQ_FIXEDPOINT float_to_Q16n16(SAMPLE_FREQ) // so Line gliss has enough precision

#define GLISS_SECONDS (0.666f*SAMPLE_LENGTH_SECONDS*NUM_LOOPS_IN_GLISS)
#define CONTROL_STEPS_PER_GLISS ((unsigned int)((float)CONTROL_RATE * GLISS_SECONDS))

void setup() {
  kTriggerDelay.start(0); // start trigger before polling in updateControl()
  aSample0.setLoopingOn();
  aSample1.setLoopingOn();
  startMozzi(CONTROL_RATE);
}

byte alevel0, alevel1;

void updateControl() {
  if (kTriggerDelay.ready()) {
    kGliss.set(SAMPLE_FREQ_FIXEDPOINT, SAMPLE_FREQ_FIXEDPOINT * 2, CONTROL_STEPS_PER_GLISS);
    kLevel0.set(ZERO_VOLUME_FIXEDPOINT, FULL_VOLUME_FIXEDPOINT, CONTROL_STEPS_PER_GLISS);
    kLevel1.set(FULL_VOLUME_FIXEDPOINT, ZERO_VOLUME_FIXEDPOINT, CONTROL_STEPS_PER_GLISS);
    aSample0.start();
    aSample1.start();
    kTriggerDelay.start((int)(GLISS_SECONDS * 1000.f)); // milliseconds
  }
  Q16n16 gliss = kGliss.next(); // fixed point
  float freq = Q16n16_to_float(gliss); // convert fixed point to floating point

  aSample0.setFreq(freq);
  aSample1.setFreq(freq * 2.f);

  alevel0 = Q15n16_to_Q0n8(kLevel0.next());
  alevel1 = Q15n16_to_Q0n8(kLevel1.next());
}


int updateAudio() {
  return (((long)aSample0.next() * alevel0) + ((long)aSample1.next() * alevel1)) >> 2;
}


void loop() {
  audioHook();
}
