/*
  Plays a fluctuating ambient wash in response to light and temperature sensors.

  Pairs of audio oscillators are detuned in response to light on an LDR and
  the base frequencies are offset depending on the temperature.

  It's a pretty dumb sonification, but at this stage
  it's just to demonstrate the possibility of reading sensors while
  lots of audio oscillators are running.

  Circuit:
    Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.io/Mozzi/

    Temperature dependent resistor (Thermistor) and 5.1k resistor on analog pin 1:
      Thermistor from analog pin to +5V (3.3V on Teensy 3.1)
      5.1k resistor from analog pin to ground

    Light dependent resistor (LDR) and 5.1k resistor on analog pin 2:
      LDR from analog pin to +5V (3.3V on Teensy 3.1)
      5.1k resistor from analog pin to ground

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2012-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#include <Mozzi.h>
#include <Oscil.h>
#include <tables/cos8192_int8.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>

#define THERMISTOR_PIN 1
#define LDR_PIN 2

// harmonics
Oscil<COS8192_NUM_CELLS, MOZZI_AUDIO_RATE> aCos1(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, MOZZI_AUDIO_RATE> aCos2(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, MOZZI_AUDIO_RATE> aCos3(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, MOZZI_AUDIO_RATE> aCos4(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, MOZZI_AUDIO_RATE> aCos5(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, MOZZI_AUDIO_RATE> aCos6(COS8192_DATA);

// duplicates but slightly off frequency for adding to originals
Oscil<COS8192_NUM_CELLS, MOZZI_AUDIO_RATE> aCos1b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, MOZZI_AUDIO_RATE> aCos2b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, MOZZI_AUDIO_RATE> aCos3b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, MOZZI_AUDIO_RATE> aCos4b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, MOZZI_AUDIO_RATE> aCos5b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, MOZZI_AUDIO_RATE> aCos6b(COS8192_DATA);

// base pitch frequencies
float f0, f1,f2,f3,f4,f5,f6;

// to map light input to frequency divergence of the b oscillators
const float DIVERGENCE_SCALE = 0.01; // 0.01*1023 = 10.23 Hz max divergence

// to map temperature to base freq drift
const float OFFSET_SCALE = 0.1; // 0.1*1023 = 102.3 Hz max drift

void setup(){
  startMozzi();

  // select base frequencies using mtof
  // C F♯ B♭ E A D the "Promethean chord"
  f1 = mtof(48.f);
  f2 = mtof(54.f);
  f3 = mtof(58.f);
  f4 = mtof(64.f);
  f5 = mtof(69.f);
  f6 = mtof(74.f);


  // set Oscils with chosen frequencies
  aCos1.setFreq(f1);
  aCos2.setFreq(f2);
  aCos3.setFreq(f3);
  aCos4.setFreq(f4);
  aCos5.setFreq(f5);
  aCos6.setFreq(f6);

  // set frequencies of duplicate oscillators
  aCos1b.setFreq(f1);
  aCos2b.setFreq(f2);
  aCos3b.setFreq(f3);
  aCos4b.setFreq(f4);
  aCos5b.setFreq(f5);
  aCos6b.setFreq(f6);
}


void loop(){
  audioHook();
}


void updateControl(){
  // read analog inputs
  int temperature = mozziAnalogRead<10>(THERMISTOR_PIN); // not calibrated to degrees! Simply a 10-bit voltage reading (0-1023)
  int light_input = mozziAnalogRead<10>(LDR_PIN);

  float base_freq_offset = OFFSET_SCALE*temperature;
  float divergence = DIVERGENCE_SCALE*light_input;

  float freq;

  // change frequencies of the oscillators, randomly choosing one pair each time to change
  switch (rand(6)+1){

    case 1:
      freq = f1+base_freq_offset;
      aCos1.setFreq(freq);
      aCos1b.setFreq(freq+divergence);
    break;

    case 2:
      freq = f2+base_freq_offset;
      aCos2.setFreq(freq);
      aCos2b.setFreq(freq+divergence);
    break;

    case 3:
      freq = f3+base_freq_offset;
      aCos3.setFreq(freq);
      aCos3b.setFreq(freq+divergence);
    break;

    case 4:
      freq = f4+base_freq_offset;
      aCos4.setFreq(freq);
      aCos4b.setFreq(freq+divergence);
    break;

    case 5:
      freq = f5+base_freq_offset;
      aCos5.setFreq(freq);
      aCos5b.setFreq(freq+divergence);
    break;

    case 6:
      freq = f6+base_freq_offset;
      aCos6.setFreq(freq);
      aCos6b.setFreq(freq+divergence);
    break;
  }
}


AudioOutput updateAudio(){

  int asig =
    aCos1.next() + aCos1b.next() +
    aCos2.next() + aCos2b.next() +
    aCos3.next() + aCos3b.next() +
    aCos4.next() + aCos4b.next() +
    aCos5.next() + aCos5b.next() +
    aCos6.next() + aCos6b.next();

  return MonoOutput::fromAlmostNBit(12, asig);
}
