
/*
  Example of oversampling analog input from a thermistor
  for increased resolution.  It's a basic attempt at a biofeedback
  device used as an ineffective treatment for migraines.  The idea
  is that if you can focus on making your hands warm, increased blood
  flow to the extremities is associated with a reduced stress response.
  Anyway, the bleeps sweep up if the temperature increases, down for decrease,
  and level for no change.  The tremelo rate increases with the temperature.
  Using Mozzi sonification library.

  Demonstrates OverSample object.

  The circuit:
     Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
     check the README or http://sensorium.github.io/Mozzi/

  Temperature dependent resistor (Thermistor) and 5.1k resistor on analog pin 1:
    Thermistor from analog pin to +5V (3.3V on Teensy 3.1)
    5.1k resistor from analog pin to ground

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2013-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 */

#include <Mozzi.h>
#include <Oscil.h> // oscillator template
#include <Line.h>
#include <tables/sin2048_int8.h> // SINe table for oscillator
#include <OverSample.h>
#include <ControlDelay.h>

// use: Oscil <table_size, update_rate> oscilName (wavetable)
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aTremelo(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aEnvelope(SIN2048_DATA);

Line <float> freqLine;

OverSample <unsigned int, 3> overSampler; // will give 10+3=13 bits resolution, 0->8191, using 128 bytes

const byte INPUT_PIN = 1;

float ENVELOPE_DURATION = 1.f;

const byte LINE_LENGTH = (byte)((float)MOZZI_CONTROL_RATE*ENVELOPE_DURATION*0.5); // 0.5 seconds per line

// adjustments to get tremelo in useful range from oversampled temperature input
const int TREMOLO_OFFSET = 4000;
const float TREMOLO_SCALE = 0.002;

void setup(){
  pinMode(INPUT_PIN,INPUT);
  //Serial.begin(9600); // for Teensy 3.1, beware printout can cause glitches
  Serial.begin(115200);
  aEnvelope.setFreq(ENVELOPE_DURATION);
  startMozzi();
}


void updateControl(){
  float start_freq, end_freq;
  static int counter, old_oversampled;

  // read the variable resistor
  int sensor_value = mozziAnalogRead<10>(INPUT_PIN); // value is 0-1023

  // get the next oversampled sensor value
  int oversampled = overSampler.next(sensor_value);

  // modulate the amplitude of the sound in proportion to the magnitude of the oversampled sensor
  float tremeloRate = TREMOLO_SCALE*(oversampled-TREMOLO_OFFSET);
  tremeloRate = tremeloRate*tremeloRate*tremeloRate*tremeloRate*tremeloRate;
  aTremelo.setFreq(tremeloRate);

  // every half second
   if (--counter<0){

    if (oversampled>old_oversampled){ // high tweet up if temp rose
      start_freq = 550.f;
      end_freq = 660.f;
    }else if(oversampled<old_oversampled){ // low tweet down if temp fell
      start_freq = 330.f;
      end_freq = 220.f;
    } else { // flat beep if no change
      start_freq = 440.f;
      end_freq = 440.f;
    }
    old_oversampled = oversampled;
    counter = LINE_LENGTH-1; // reset counter

    // set the line to change the main frequency
    freqLine.set(start_freq,end_freq,LINE_LENGTH);

    // print out for debugging
    Serial.print(oversampled);Serial.print("\t");Serial.print(start_freq);Serial.print("\t");Serial.println(end_freq);
  }

  // update the main frequency of the sound
  aSin.setFreq(freqLine.next());

}


AudioOutput updateAudio(){
  return MonoOutput::from16Bit((((int)aSin.next()*(128+aTremelo.next()))>>8)*aEnvelope.next());
}


void loop(){
  audioHook(); // required here
}
