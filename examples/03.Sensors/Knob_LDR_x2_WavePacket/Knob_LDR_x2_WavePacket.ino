/*
  Example of Wavepacket synthesis, using analog
  inputs to change the fundamental frequency,
  bandwidth and centre frequency,
  using Mozzi sonification library.

  Demonstrates WavePacket, mozziAnalogRead(), and smoothing
  control signals with RollingAverage.
  Also demonstrates AutoMap, which maps unpredictable inputs to a set range.

  This example goes with a tutorial on the Mozzi site:
  http://sensorium.github.io/Mozzi/learn/introductory-tutorial/

  The circuit:
     Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
     check the README or http://sensorium.github.com/Mozzi/

     Potentiometer connected to analog pin 0.
       Center pin of the potentiometer goes to the analog pin.
       Side pins of the potentiometer go to +5V and ground

     Light dependent resistor (LDR) and 5.1k resistor on analog pin 1:
       LDR from analog pin to +5V (3.3V on Teensy 3.1)
       5.1k resistor from analog pin to ground

     Light dependent resistor (LDR) and 5.1k resistor on analog pin 2:
       LDR from analog pin to +5V (3.3V on Teensy 3.1)
       5.1k resistor from analog pin to ground

  Mozzi documentation/API
  https://sensorium.github.io/Mozzi/doc/html/index.html

  Mozzi help/discussion/announcements:
  https://groups.google.com/forum/#!forum/mozzi-users

  Tim Barrass 2013, CC by-nc-sa.
*/

#include <WavePacket.h>
#include <RollingAverage.h>
#include <AutoMap.h>

const int KNOB_PIN = 0; // set the input for the knob to analog pin 0
const int LDR1_PIN=1; // set the analog input for fm_intensity to pin 1
const int LDR2_PIN=2; // set the analog input for mod rate to pin 2

// min and max values of synth parameters to map AutoRanged analog inputs to
const int MIN_F = 20;
const int MAX_F = 150;

const int MIN_BW = 20;
const int MAX_BW = 600;

const int MIN_CF = 60;
const int MAX_CF = 600;


// for smoothing the control signals
// use: RollingAverage <number_type, how_many_to_average> myThing
RollingAverage <int, 16> kAverageF;
RollingAverage <int, 16> kAverageBw;
RollingAverage <int, 16> kAverageCf;
// AutoMap adapts to range of input as it arrives
AutoMap kMapF(0,1023,MIN_F,MAX_F);
AutoMap kMapBw(0,1023,MIN_BW,MAX_BW);
AutoMap kMapCf(0,1023,MIN_CF,MAX_CF);

WavePacket <DOUBLE> wavey; // DOUBLE selects 2 overlapping streams

void setup(){
  //Serial.begin(9600); // for Teensy 3.1, beware printout can cause glitches
  Serial.begin(115200);
  // wait before starting Mozzi to receive analog reads, so AutoRange will not get 0
  delay(200);
  startMozzi();
}


void updateControl(){
  int fundamental = mozziAnalogRead(KNOB_PIN)+1;
  fundamental = kMapF(fundamental);
  Serial.print("f=");
  Serial.print(fundamental);

  int bandwidth = mozziAnalogRead(LDR1_PIN);
  bandwidth = kMapBw(bandwidth);
  Serial.print("\t bw=");
  Serial.print(bandwidth);

  int centre_freq = mozziAnalogRead(LDR2_PIN);
  centre_freq = kMapCf(centre_freq);
  Serial.print("\t cf=");
  Serial.print(centre_freq);

  Serial.println();

  wavey.set(fundamental, bandwidth, centre_freq);
}



int updateAudio(){
  return wavey.next()>>8; // >>8 for AUDIO_MODE STANDARD
}



void loop(){
  audioHook(); // required here
}
