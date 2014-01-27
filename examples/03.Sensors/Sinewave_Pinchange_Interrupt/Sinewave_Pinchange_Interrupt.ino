/*  
  Example of changing a sound in response to pin changes detected 
  in the background with an interrupt, using PinChangeInt library
  with Mozzi sonification library.
  
  Demonstrates using Mozzi with PinChangeInt library from:
  http://code.google.com/p/arduino-pinchangeint/
  
  Circuit:
  
    Audio output on digital pin 9 (on a Uno or similar), or 
    check the README or http://sensorium.github.com/Mozzi/
    
    Pushbutton on digital pin D4
      button from the digital pin to +5V
      10K resistor from the digital pin to ground
     
     
  Mozzi help/discussion/announcements:
  https://groups.google.com/forum/#!forum/mozzi-users
  
  Tim Barrass 2013.
  This example code is in the public domain.
 */

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <PinChangeInt.h>

#define PIN 4  // the pin we are interested in

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);


void setup(){
  aSin.setFreq(440);
  pinMode(PIN, INPUT);     //set the pin to input
  digitalWrite(PIN, HIGH); //use the internal pullup resistor
  PCintPort::attachInterrupt(PIN, changeFreq,RISING); // attach a PinChange Interrupt to our pin on the rising edge
  // (RISING, FALLING and CHANGE all work with this library)
  // and execute the function changeFreq when that pin changes
  startMozzi();
}



void updateControl(){
  // everything is done by the pinchange interrupt
}



void changeFreq()
{
  static int freq = 220;
  if (freq==220){
    freq=330;
  }
  else{
    freq=220;
  }
  aSin.setFreq(freq); // set the frequency
}



int updateAudio(){
  return aSin.next(); // return an int signal centred around 0
}



void loop(){
  audioHook(); // required here
}
