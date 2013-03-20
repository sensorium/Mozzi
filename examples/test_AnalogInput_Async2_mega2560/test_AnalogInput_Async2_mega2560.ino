/*
  Demonstrates asynchronous analog input 
  using initADC(), getSensor() and startRead().
  getSensor() fetches the most recent inputs from an array which
  is updated in the background when startRead() is called.
  Tested with different combinations of pins on a Nano w/ 328
  and etherMega2560.
 */

#include <MozziGuts.h>
#include <utils.h>
#include <mozzi_analog.h>

#define CONTROL_RATE 64 // powers of 2 please

void setup() {
  Serial.begin(115200);
  startMozzi(CONTROL_RATE);
  initADC();
}


void updateControl(){
  Serial.print(getSensor(5));
  Serial.print("   ");

  Serial.print(getSensor(6));
  Serial.print("   ");

  Serial.print(getSensor(7));
  Serial.print("   ");

  Serial.println(getSensor(8));

  startRead();
}


int updateAudio(){
  return 0;
}


void loop(){
  audioHook();
}





