/*
 *  Example reading 4 analog inputs and printing the readings,
 *  using Mozzi sonification library.
 *  
 *  Demonstrates asynchronous analog input 
 *  with adcStartConversion() and adcGetResult(),
 *  using a round-robin scheme in successive updateControl() calls.
 *  adcGetResult() returns the most recent analog conversion.
 *  adcStartConversion(pin) starts the next single conversion
 *   for the selected pin or channel.
 *
 *  This scheme may be more efficient than using adcReadAllChannels()
 *  since only the required channels are actually converted and any
 *  processing load is distributed across multiple updateControl() calls,
 *  instead of concentrated in a rapid series of interrupts when using
 *  adcReadAllChannels.
 *  
 *  Mozzi help/discussion/announcements:
 *  https://groups.google.com/forum/#!forum/mozzi-users
 *
 *  Tim Barrass 2013.
 *  This example code is in the public domain.
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
  Serial.print(adcGetResult());// gets the most recent reading
  Serial.print("   "); 
  if(++pin>=NUM_PINS) {
    pin = 0;
    Serial.println();
  }
  // start the next read cycle in the background
  adcStartConversion(pin);
}


int updateAudio(){
  return 0;
}


void loop(){
  audioHook();
}






