/* 
  Example using 2 light dependent resistors (LDRs) to change 
  FM synthesis parameters, and a knob for fundamental frequency,
  using Mozzi sonification library.

  Demonstrates analog input, audio and control oscillators, phase modulation
  and smoothing a control signal at audio rate to avoid clicks.
  
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

#include <MozziGuts.h>
#include <Oscil.h> // oscillator 
#include <tables/cos2048_int8.h> // table for Oscils to play
#include <mozzi_analog.h> // fast functions for reading analog inputs 
#include <Smooth.h>

const int KNOB_PIN = 0; // set the input for the knob to analog pin 0
const int LDR1_PIN=1; // set the analog input for fm_intensity to pin 1
const int LDR2_PIN=2; // set the analog input for mod rate to pin 2

Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCarrier(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModulator(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kIntensityMod(COS2048_DATA);

int mod_ratio = 5; // brightness (harmonics)
long fm_intensity; // carries control info from updateControl to updateAudio

// smoothing for intensity to remove clicks on transitions
float smoothness = 0.95f;
Smooth <long> aSmoothIntensity(smoothness);


void setup(){
  Serial.begin(115200); // set up the Serial output so we can look at the light level
  setupFastAnalogRead(); // one way of increasing the speed of reading the input
  startMozzi(); // :))
}


void updateControl(){
  // read the knob
  int knob_value = analogRead(KNOB_PIN); // value is 0-1023
  
  // map knob to carrier frequency
  int carrier_freq = map(knob_value, 0, 1023, 22, 440);
  
  //calculate the modulation frequency to stay in ratio
  int mod_freq = carrier_freq * mod_ratio;
  
  // set the FM oscillator frequencies
  aCarrier.setFreq(carrier_freq); 
  aModulator.setFreq(mod_freq);
  
  // read the light dependent resistor on the width Analog input pin
  int LDR1_value= analogRead(LDR1_PIN); // value is 0-1023
  // print the value to the Serial monitor for debugging
  Serial.print("LDR1 = "); 
  Serial.print(LDR1_value);
  Serial.print("\t"); // prints a tab

  int LDR1_calibrated = map(LDR1_value,350,800,700,10); // calibrate for sensor
  Serial.print("LDR1_calibrated = ");
  Serial.print(LDR1_calibrated);
  Serial.print("\t"); // prints a tab
  
 // calculate the fm_intensity
  fm_intensity = ((long)LDR1_calibrated * (kIntensityMod.next()+128))>>8; // shift back to range after 8 bit multiply
  Serial.print("fm_intensity = ");
  Serial.print(fm_intensity);
  Serial.print("\t"); // prints a tab
  
  // read the light dependent resistor on the speed Analog input pin
  int LDR2_value= analogRead(LDR2_PIN); // value is 0-1023
  Serial.print("LDR2 = "); 
  Serial.print(LDR2_value);
  Serial.print("\t"); // prints a tab
  
  // use a float here for low frequencies
  float mod_speed = (float)map(LDR2_value,600,800,10000,1)/1000; // calibrate for sensor
  Serial.print("   mod_speed = ");
  Serial.print(mod_speed);
  kIntensityMod.setFreq(mod_speed);
  
  Serial.println(); // finally, print a carraige return for the next line of debugging info
}


int updateAudio(){
  long modulation = aSmoothIntensity.next(fm_intensity) * aModulator.next();
  return aCarrier.phMod(modulation);
}


void loop(){
  audioHook();
}





