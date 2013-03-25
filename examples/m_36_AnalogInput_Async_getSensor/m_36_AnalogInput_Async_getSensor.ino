/*
  Demonstrates asynchronous analog input 
 using adcEnableInterrupt(), adcGetChannel() and adcReadAllChannels().
 adcGetChannel() fetches the most recent inputs from an array which
 is updated in the background when adcReadAllChannels() is called.
 Tested with different combinations of pins on a Nano w/ 328
 and etherMega2560.
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
  // adcGetChannel(n) gets the most recent reading for analog channel n
  Serial.print(adcGetChannel(0));
  Serial.print("   ");

  Serial.print(adcGetChannel(1));
  Serial.print("   ");

  Serial.print(adcGetChannel(2));
  Serial.print("   ");

  Serial.println(adcGetChannel(3));

  // start the next read cycle in the background
  adcReadAllChannels();
}


int updateAudio(){
  return 0;
}


void loop(){
  audioHook();
}






