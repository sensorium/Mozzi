/*  Test of audio input and processing
 *  using Mozzi sonification library.
 *
 *  Configuration: requires "#define USE_AUDIO_INPUT true" to be in
 *  Mozzi/mozzi_config.h file.
 *
 *  Audio Circuit: Audio input on pin 0, output on digital pin 9 (on a Uno or similar), or
 *  check the README or http://sensorium.github.com/Mozzi/
 *  
 *  Control circuit: http://arduino.cc/en/Tutorial/RCtime
 *  Values for components which work with this sketch are:
 *  Sensing Capacitor .1uf (104)
 *  Variable resistor 1 MegaOhm
 *  Input resistor 470 Ohms
 *
 *  Mozzi help/discussion/announcements:
 *  https://groups.google.com/forum/#!forum/mozzi-users
 
 */

#include <MozziGuts.h>
#include <LowPassFilter.h>
#include <RCpoll.h> // enables 

#define CONTROL_RATE 128 // powers of 2 please
#define AUDIO_PIN 0              // analog pin for audio input
#define SENSOR_PIN 4            // digital pin for sensor input

LowPassFilter lpf;
RCpoll <SENSOR_PIN> kNob;


void setup(){
  pinMode(AUDIO_PIN,INPUT);
  lpf.setResonance(220);
  startMozzi(CONTROL_RATE);
}



void updateControl(){
  unsigned char cutoff_freq = 60 + 8*kNob.next(); // kNob ranges about 0 to 13
  lpf.setCutoffFreq(cutoff_freq);
}


int updateAudio(){
  // subtracting AUDIO_BIAS moves the unsigned audio data into 0-centred, 
  // signed range required by all Mozzi units
  int asig = (getAudioInput()>>2) - AUDIO_BIAS;
  asig = lpf.next(asig);
  return asig;
}


void loop(){
  audioHook();
}




