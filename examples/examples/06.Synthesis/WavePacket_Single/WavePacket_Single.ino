/*  Example of Wavepacket synthesis, using Mozzi sonification library.
    This sketch draws on Miller Puckette's 
    Pure Data example, F14.wave.packet.pd, with
    one non-overlapping stream of wave packets.
  
    Circuit: Audio output on digital pin 9 (for STANDARD mode on a Uno or similar), or 
    check the README or http://sensorium.github.com/Mozzi/
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2013.
    This example code is in the public domain.
*/
 
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



int updateAudio(){
  return wavey.next()>>8; // >>8 for AUDIO_MODE STANDARD
}



void loop(){
  audioHook(); // required here
}
