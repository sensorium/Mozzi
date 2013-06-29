/*
 *  Example of synchronous analog input,
 *  using Mozzi sonification library.
 *  
 *  Demonstrates adcEnableInterrupt(), adcGetResult() and adcReadAllChannels().
 *  adcGetResult() fetches the most recent inputs from an array which
 *  is updated in the background when adcReadAllChannels() is called.
 *  Tested with different combinations of pins on a Nano w/ 328
 *  and etherMega2560.
 *   
 *  Mozzi help/discussion/announcements:
 *  https://groups.google.com/forum/#!forum/mozzi-users
 *
 *  Tim Barrass 2013.
 *  This example code is in the public domain.
 *   
 */

#include <MozziGuts.h>
#include <mozzi_utils.h>
#include <mozzi_analog.h>

#define CONTROL_RATE 64 // powers of 2 please

void setup() {
  Serial.begin(115200);
  startMozzi(CONTROL_RATE);
  adcEnableInterrupt();
}


void updateControl(){
  // adcGetResult(n) gets the most recent reading for analog channel n
  Serial.print(adcGetResult(0));
  Serial.print("   ");

  Serial.print(adcGetResult(1));
  Serial.print("   ");

  Serial.print(adcGetResult(2));
  Serial.print("   ");

  Serial.println(adcGetResult(3));

  // start the next read cycle in the background
  adcReadAllChannels();
}


int updateAudio(){
  return 0;
}


void loop(){
  audioHook();
}






