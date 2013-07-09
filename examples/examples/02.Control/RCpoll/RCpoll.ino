/*  Sets the frequency of an oscillator with a resistive sensor on a digital pin,
 *  using Mozzi sonification library.
 *
 *  Demonstrates RCpoll to read a resistive sensor on a digital pin,
 *  without blocking audio output. 
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
#include <Oscil.h>
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <RCpoll.h>

#define CONTROL_RATE 128 // powers of 2 please
#define SENSOR_PIN 4            // digital pin for sensor input

Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);
RCpoll <SENSOR_PIN> kNob;


void setup(){
  Serial.begin(115200);
  startMozzi(CONTROL_RATE);
}



void updateControl(){
  int freq = 60 + 8*kNob.next(); // kNob ranges about 0 to 13
  Serial.println(freq);
  aSin.setFreq(freq);
}


int updateAudio(){
  return aSin.next(); // 8 bits scaled up to 14 bits
}


void loop(){
  audioHook();
}




