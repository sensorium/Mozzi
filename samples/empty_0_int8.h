#ifndef EMPTY_0_H_
#define EMPTY_0_H_
 
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include "mozzi_pgmspace.h"
 
#define EMPTY_0_NUM_CELLS 0
#define EMPTY_0_SAMPLERATE 16384
 
CONSTTABLE_STORAGE(int8_t) EMPTY_0_DATA [] = { };
 
 #endif /* EMPTY_0_H_ */
