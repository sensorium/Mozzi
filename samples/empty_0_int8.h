#ifndef EMPTY_0_H_
#define EMPTY_0_H_
 
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include <avr/pgmspace.h>
 
#define EMPTY_0_NUM_CELLS 0
#define EMPTY_0_SAMPLERATE 16384
 
const char __attribute__((section(".progmem.data"))) EMPTY_0_DATA [] = { };
 
 #endif /* EMPTY_0_H_ */
