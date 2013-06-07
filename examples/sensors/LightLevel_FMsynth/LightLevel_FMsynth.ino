/* 
  Example using a light dependent resistor (LDR) to change 
  an FM synthesis parameter, and a knob for volume,
  using Mozzi sonification library.

  Demonstrates analog input, audio oscillators, and phase modulation.
  There might be clicks in the audio from rapid control changes, which
  could be smoothed with Line or Smooth objects.
  
    The circuit:
  *  Audio output on digital pin 9 (on a Uno or similar), or 
     check the README or http://sensorium.github.com/Mozzi/

  *  Potentiometer connected to analog pin 0.
  *  Center pin of the potentiometer goes to the analog pin.
  *  Side pins of the potentiometer go to +5V and ground
 
  Mozzi help/discussion/announcements:
  https://groups.google.com/forum/#!forum/mozzi-users

  Tim Barrass 2013.
  This example code is in the public domain.
 */

#include <MozziGuts.h>
#include <Oscil.h> // oscillator 
#include <tables/cos2048_int8.h> // table for Oscils to play
#include <mozzi_analog.h> // fast functions for reading analog inputs 

const int KNOB_PIN = 0; // set the input for the knob to analog pin 0
const int LDR_PIN = 1; // set the input for the Light Dependent Resistor to analog pin 1

Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCarrier(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModulator(COS2048_DATA);

int carrier_freq = 55; // fundamental frequency
int mod_ratio = 6; // harmonics
int mod_freq = carrier_freq * mod_ratio;
long deviation; // carries control info from updateControl to updateAudio

unsigned char volume;

void setup(){
  Serial.begin(115200); // set up the Serial output so we can look at the light level
  setupFastAnalogRead(); // one way of increasing the speed of reading the input
  aCarrier.setFreq(carrier_freq); 
  aModulator.setFreq(mod_freq);
  startMozzi(); // :))
}


void updateControl(){
    // read the light dependent resistor
  int knob_value = analogRead(KNOB_PIN); // value is 0-1023
  
  // map it to an 8 bit range (0-255) for efficient calculations in updateAudio
  volume = knob_value>> 2; // shift right by 2 is a fast way to divide by 4
  
  // read the light dependent resistor on the Analog input pin
  unsigned int light_level= analogRead(LDR_PIN); // value is 0-1024
  // print the value to the Serial monitor for debugging
  Serial.print("Light level = "); 
  Serial.print(light_level); 

  deviation = light_level - 400; // calibrate for sensor
  //Serial.print("   deviation = ");
  //Serial.print(deviation);
  Serial.println(); // print a carraige return for the next line of debugging info

}


int updateAudio(){
  // cast deviation to long to make room for int * char multiply
  long modulation = (long)deviation * aModulator.next(); 
  char out = aCarrier.phMod(modulation);
  // change the volume and shift back into 8 bit range after multiply
  return ((int)out * volume)>>8; 
}


void loop(){
  audioHook();
}





