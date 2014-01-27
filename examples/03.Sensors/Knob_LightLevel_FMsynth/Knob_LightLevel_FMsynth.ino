/* 
  Example using a light dependent resistor (LDR) to change 
  an FM synthesis parameter, and a knob for fundamental frequency,
  using Mozzi sonification library.

  Demonstrates analog input, audio oscillators, and phase modulation.
  Also demonstrates AutoMap, which maps unpredictable inputs to a set range.
  There might be clicks in the audio from rapid control changes, which
  could be smoothed with Line or Smooth objects.
    
  This example goes with a tutorial on the Mozzi site:
  http://sensorium.github.io/Mozzi/learn/Mozzi_Introductory_Tutorial.pdf
  
  The circuit:
     Audio output on digital pin 9 (on a Uno or similar), or 
     check the README or http://sensorium.github.com/Mozzi/

     Potentiometer connected to analog pin 0.
       Center pin of the potentiometer goes to the analog pin.
       Side pins of the potentiometer go to +5V and ground
  
     Light dependent resistor (LDR) and 5.1k resistor on analog pin 1:
       LDR from analog pin to +5V
       5.1k resistor from analog pin to ground
  
  Mozzi help/discussion/announcements:
  https://groups.google.com/forum/#!forum/mozzi-users

  Tim Barrass 2013.
  This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Oscil.h> // oscillator 
#include <tables/cos2048_int8.h> // table for Oscils to play
#include <AutoMap.h> // maps unpredictable inputs to a range
 
// desired carrier frequency max and min, for AutoMap
const int MIN_CARRIER_FREQ = 22;
const int MAX_CARRIER_FREQ = 440;

// desired intensity max and min, for AutoMap, note they're inverted for reverse dynamics
const int MIN_INTENSITY = 700;
const int MAX_INTENSITY = 10;

AutoMap kMapCarrierFreq(0,1023,MIN_CARRIER_FREQ,MAX_CARRIER_FREQ);
AutoMap kMapIntensity(0,1023,MIN_INTENSITY,MAX_INTENSITY);

const int KNOB_PIN = 0; // set the input for the knob to analog pin 0
const int LDR_PIN = 1; // set the input for the LDR to analog pin 1

Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCarrier(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModulator(COS2048_DATA);

int mod_ratio = 3; // harmonics
long fm_intensity; // carries control info from updateControl() to updateAudio()


void setup(){
  Serial.begin(115200); // set up the Serial output for debugging
  startMozzi(); // :))
}


void updateControl(){
  // read the knob
  int knob_value = mozziAnalogRead(KNOB_PIN); // value is 0-1023

  // map the knob to carrier frequency
  int carrier_freq = kMapCarrierFreq(knob_value);
  
  //calculate the modulation frequency to stay in ratio
  int mod_freq = carrier_freq * mod_ratio;
  
  // set the FM oscillator frequencies to the calculated values
  aCarrier.setFreq(carrier_freq); 
  aModulator.setFreq(mod_freq);
  
  // read the light dependent resistor on the Analog input pin
  int light_level= mozziAnalogRead(LDR_PIN); // value is 0-1023
  
  // print the value to the Serial monitor for debugging
  Serial.print("light_level = "); 
  Serial.print(light_level); 
  Serial.print("\t"); // prints a tab
  
  fm_intensity = kMapIntensity(light_level);
  
  Serial.print("fm_intensity = ");
  Serial.print(fm_intensity);
  Serial.println(); // print a carraige return for the next line of debugging info

}


int updateAudio(){
  long modulation = fm_intensity * aModulator.next(); 
  return aCarrier.phMod(modulation); // phMod does the FM
}


void loop(){
  audioHook();
}





