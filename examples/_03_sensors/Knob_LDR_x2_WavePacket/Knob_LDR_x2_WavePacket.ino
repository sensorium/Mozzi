/*  
  Example of Wavepacket synthesis, using analog
  inputs to change the fundamental frequency, 
  bandwidth and centre frequency,
  using Mozzi sonification library.

  Demonstrates WavePacket, non-blocking analog reads using
  adcReadAllChannels() and adcGetResult(), and smoothing
  control signals with RecentAverage.
  
  This example goes with a tutorial on the Mozzi site:
  http://sensorium.github.io/Mozzi/Mozzi_Introductory_Tutorial.pdf
  
    The circuit:
  *  Audio output on digital pin 9 (on a Uno or similar), or 
     check the README or http://sensorium.github.com/Mozzi/

  *  Potentiometer connected to analog pin 0.
  *  Center pin of the potentiometer goes to the analog pin.
  *  Side pins of the potentiometer go to +5V and ground
  
  Light dependent resistor (LDR) and 5.1k resistor on analog pin 1:
  * LDR from analog pin to +5V
  * 5.1k resistor from analog pin to ground

   Light dependent resistor (LDR) and 5.1k resistor on analog pin 2:
  * LDR from analog pin to +5V
  * 5.1k resistor from analog pin to ground
  
  Mozzi help/discussion/announcements:
  https://groups.google.com/forum/#!forum/mozzi-users

  Tim Barrass 2013.
  This example code is in the public domain.
 */
 
 #include <mozzi_analog.h>
 #include <WavePacket.h>
 #include <RecentAverage.h>
 
const int KNOB_PIN = 0; // set the input for the knob to analog pin 0
const int LDR1_PIN=1; // set the analog input for fm_intensity to pin 1
const int LDR2_PIN=2; // set the analog input for mod rate to pin 2

// for smoothing the control signals
// use: RecentAverage <number_type, how_many_to_average> myThing
RecentAverage <int, 32> kAverageF;
RecentAverage <int, 32> kAverageBw;
RecentAverage <int, 32> kAverageCf;

WavePacket <DOUBLE> wavey; // DOUBLE selects 2 overlapping streams


void setup(){
  // set up async analog reading for adcReadAllChannels() and adcGetResult()
  adcEnableInterrupt();
  startMozzi();
}


void updateControl(){
  int fundamental = adcGetResult(KNOB_PIN)+1;
  fundamental = kAverageF.next(fundamental);

  int bandwidth = adcGetResult(LDR1_PIN);
  bandwidth = map(bandwidth,300,600,0,1023); // calibrate for sensor
  bandwidth = kAverageBw.next(bandwidth);

  int centre_freq = adcGetResult(LDR2_PIN);
  centre_freq = map(centre_freq,300,600,0,2047); // calibrate for sensor
  centre_freq = kAverageBw.next(centre_freq);
  
  wavey.set(fundamental, bandwidth, centre_freq);
  adcReadAllChannels();
}



int updateAudio(){
  return wavey.next()>>8; // >>8 for AUDIO_MODE STANDARD
}



void loop(){
  audioHook(); // required here
}
