/*  Example of Wavepacket synthesis, using Mozzi sonification library.
    This sketch draws on Miller Puckette's
    Pure Data example, F14.wave.packet.pd, with
    two overlapping streams of wave packets.

    Demonstrates the WavePacketSample object, which enables a
    custom sound table to be used as the audio source for wavepackets.

    Circuit:
    	Audio output on DAC/A14 on Teensy 3.0, 3.1,
    	or digital pin 9 on a Uno or similar, or
    	check the README or http://sensorium.github.io/Mozzi/

    	Potentiometer connected to analog pin 0.
      Center pin of the potentiometer goes to the analog pin.
      Side pins of the potentiometer go to +5V and ground

      Potentiometer connected to analog pin 1.
      Center pin of the potentiometer goes to the analog pin.
      Side pins of the potentiometer go to +5V and ground

      Potentiometer connected to analog pin 2.
      Center pin of the potentiometer goes to the analog pin.
      Side pins of the potentiometer go to +5V and ground
      
   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2013-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#include <Mozzi.h>
#include <mozzi_analog.h>
#include <WavePacketSample.h>
#include <RollingAverage.h>
#include <samples/raven_arh_int8.h>

#define FUNDAMENTAL_PIN 0
#define BANDWIDTH_PIN 1
#define CENTREFREQ_PIN 2

// for smoothing the control signals
// use: RollingAverage <number_type, how_many_to_average> myThing
RollingAverage <int, 32> kAverageF;
RollingAverage <int, 32> kAverageBw;
RollingAverage <int, 32> kAverageCf;

WavePacketSample <DOUBLE> wavey; // DOUBLE selects 2 overlapping streams


void setup(){
  wavey.setTable(RAVEN_ARH_DATA);
  pinMode(11,OUTPUT);
  digitalWrite(11,LOW);
  startMozzi();
}


void updateControl(){
  int f = kAverageF.next(mozziAnalogRead<10>(FUNDAMENTAL_PIN))+1;
  int b = kAverageBw.next(mozziAnalogRead<10>(BANDWIDTH_PIN));
  int cf = kAverageCf.next(mozziAnalogRead<11>(CENTREFREQ_PIN));
  wavey.set(f, b, cf);
}


AudioOutput updateAudio(){
  return MonoOutput::from16Bit(wavey.next());
}


void loop(){
  audioHook(); // required here
}
