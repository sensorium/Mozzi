/**
 * @file speedInfo.ino
 * @author Phil Schatzmann
 * @brief We calculate the sample generation speed from a Oscil and the max write speed to update the pins
 * @version 0.1
 * @date 2021-05-15
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 64 // Hz, powers of 2 are most reliable
uint32_t count=0;
uint64_t timeout;
int value=0;

AudioOutput_t updateAudio(){
  return MonoOutput::from8Bit(aSin.next()); // return an int signal centred around 0
}

void measureOutput() {
  timeout = millis() + 10000;
  while (timeout>millis()){
    value += value + 1;
    audioOutput(value);
    count++;
    if (value>100){
      value = 0;
    }  
  }
  
  Serial.print("PWM writes per second: ");
  Serial.print(count / 10);
  Serial.print(" for ");
  Serial.print(CHANNELS);
  Serial.print(" channels ");
}

void measureSin() {
  timeout = millis() + 1000;
  while (timeout>millis()){
    updateAudio();    
    count++;
  }

  Serial.print("Sound updates per second: ");
  Serial.println(count / 1);  
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Starting...");

  startMozzi(CONTROL_RATE); // :)
  aSin.setFreq(440); // set the frequency

  measureSin();
  measureOutput();

  Serial.println("DONE!");

}

void loop() {

}