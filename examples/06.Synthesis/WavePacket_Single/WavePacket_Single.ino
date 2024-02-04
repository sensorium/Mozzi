/*  Example of Wavepacket synthesis, using Mozzi sonification library.
    This sketch draws on Miller Puckette's
    Pure Data example, F14.wave.packet.pd, with
    one non-overlapping stream of wave packets.

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

    Tim Barrass 2013, CC by-nc-sa.
*/

#include <Mozzi.h>
#include <mozzi_analog.h>
#include <WavePacket.h>
#include <RollingAverage.h>

#define FUNDAMENTAL_PIN 0
#define BANDWIDTH_PIN 1
#define CENTREFREQ_PIN 2

// for smoothing the control signals
// use: RollingAverage <number_type, how_many_to_average> myThing
RollingAverage <int, 32> kAverageF;
RollingAverage <int, 32> kAverageBw;
RollingAverage <int, 32> kAverageCf;

WavePacket <SINGLE> wavey; // SINGLE selects 1 non-overlapping stream


void setup(){
  startMozzi();
}


void updateControl(){
  wavey.set(kAverageF.next(mozziAnalogRead(FUNDAMENTAL_PIN))+1,
    kAverageBw.next(mozziAnalogRead(BANDWIDTH_PIN)),
    kAverageCf.next(2*mozziAnalogRead(CENTREFREQ_PIN)));
}



AudioOutput updateAudio(){
  return MonoOutput::from16Bit(wavey.next());
}



void loop(){
  audioHook(); // required here
}
