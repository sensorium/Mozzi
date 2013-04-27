/*  Example of Wavepacket synthesis, using Mozzi sonification library.
 *  This sketch draws on Miller Puckette's 
 *  Pure Data example, F14.wave.packet.pd, with
 *  two overlapping streams of wave packets.
 *
 *  IMPORTANT: this sketch requires Mozzi/mozzi_config.h to be
 *  be changed from STANDARD mode to HIFI.
 *  In Mozz/mozzi_config.h, change
 *  #define AUDIO_MODE STANDARD
 *  //#define AUDIO_MODE HIFI
 *  to
 *  //#define AUDIO_MODE STANDARD
 *  #define AUDIO_MODE HIFI
 *
 *  Circuit: Audio output on digital pin 9 and 10 (on a Uno or similar),
 *  Check the Mozzi core module documentation for others and more detail
 *
 *                   3.9k 
 *   pin 9  ---WWWW-----|-----output
 *                     1M           |
 *   pin 10 ---WWWW---- |
 *                                     |
 *                           4.7n  ==
 *                                     |
 *                                 ground
 *
 *  Resistors are ±0.5%  Measure and choose the most precise 
 *  from a batch of whatever you can get.
 *
 *  Mozzi help/discussion/announcements:
 *  https://groups.google.com/forum/#!forum/mozzi-users
 *
 *  Tim Barrass 2012.
 *  This example code is in the public domain.
 */
 
 #include <mozzi_analog.h>
 #include <WavePacket.h>
 #include <RecentAverage.h>
 
#define FUNDAMENTAL_PIN 0
#define BANDWIDTH_PIN 1
#define CENTREFREQ_PIN 2

// for smoothing the control signals
// use: RecentAverage <number_type, how_many_to_average> myThing
RecentAverage <int, 32> kAverageF;
RecentAverage <int, 32> kAverageBw;
RecentAverage <int, 32> kAverageCf;

WavePacket <DOUBLE> wavey; // DOUBLE selects 2 overlapping streams


void setup(){
  startMozzi();
  adcEnableInterrupt();
}


void updateControl(){
  wavey.set(kAverageF.next(adcGetResult(FUNDAMENTAL_PIN))+1, 
    kAverageBw.next(adcGetResult(BANDWIDTH_PIN)), 
    kAverageCf.next(2*adcGetResult(CENTREFREQ_PIN)));
  adcReadAllChannels();
}



int updateAudio(){
  return wavey.next();
}



void loop(){
  audioHook(); // required here
}
