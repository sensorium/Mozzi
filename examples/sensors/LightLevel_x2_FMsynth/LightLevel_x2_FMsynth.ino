/* 
  Example using 2 light dependent resistors to change 
  FM synthesis parameters, and a volume knob,
  using Mozzi sonification library.

  Demonstrates analog input, audio and control oscillators, phase modulation
  and smoothing control signals to avoid clicks.
  
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
#include <Smooth.h>

const int KNOB_PIN = 0; // set the input for the knob to analog pin 0
const int WIDTH_PIN=1; // set the analog input for deviation to pin 0
const int SPEED_PIN=2; // set the analog input for mod rate to pin 5

Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCarrier(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModulator(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kDeviationMod(COS2048_DATA);

int carrier_freq = 55; // fundamental frequency
int mod_ratio = 5; // brightness
int mod_freq = carrier_freq * mod_ratio;
long deviation; // carries control info from updateControl to updateAudio

unsigned char volume;

float smoothness = 0.95f;
Smooth <long> aSmooth(smoothness);

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
  
  // read the light dependent resistor on the width Analog input pin
  unsigned int width_light_level= analogRead(WIDTH_PIN); // value is 0-1023
  // print the value to the Serial monitor for debugging
  Serial.print("width light level = "); 
  Serial.print(width_light_level); 

  deviation = width_light_level - 400; // calibrate for sensor
  Serial.print("   deviation = ");
  Serial.print(deviation);

  // read the light dependent resistor on the speed Analog input pin
  unsigned int speed_light_level= analogRead(SPEED_PIN); // value is 0-1023
  Serial.print("    speed light level = "); 
  Serial.print(speed_light_level);
  
  int mod_speed = (1023-speed_light_level)>>6; // calibrate (shift is a fast way to divide)
  Serial.print("   mod speed = ");
  Serial.print(mod_speed);
  
  kDeviationMod.setFreq(mod_speed);

  // modulate the deviation
  deviation = (deviation * (kDeviationMod.next()+128))>>8; // shift back to range after 8 bit multiply
  Serial.print("   final deviation = ");
  Serial.print(deviation);
  
  Serial.println(); // finally, print a carraige return for the next line of debugging info
}


int updateAudio(){
  long modulation = aSmooth.next(deviation) * aModulator.next();
  char out = aCarrier.phMod(modulation);
  // change the volume and shift back into 8 bit range after multiply
  return ((int)out * volume)>>8; 
}


void loop(){
  audioHook();
}





