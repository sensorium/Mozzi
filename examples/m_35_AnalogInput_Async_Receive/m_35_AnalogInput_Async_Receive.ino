/*
  Demonstrates asynchronous analog input 
 using startAnalogRead() and receiveAnalogRead().
 receiveAnalogRead() returns the most recent input.
 startAnalogRead(pin) starts the next update in the 
 background, for the selected pin.
 */

#include <MozziGuts.h>
#include <mozzi_analog.h>

#define CONTROL_RATE 64 // powers of 2 please

void setup() {
  Serial.begin(115200);
  startMozzi(CONTROL_RATE);
  setupFastAnalogRead(); // optional
}


const char NUM_PINS = 4;
int pin = 0;

void updateControl(){
  // receiveAnalogRead() gets the most recent reading
  Serial.print(receiveAnalogRead());
  Serial.print("   "); 
  if(++pin>=NUM_PINS) {
    pin = 0;
    Serial.println();
  }
  // start the next read cycle in the background
  startAnalogRead(pin);
}


int updateAudio(){
  return 0;
}


void loop(){
  audioHook();
}






