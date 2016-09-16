#ifndef MOZZI_RAND_H_
#define MOZZI_RAND_H_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif


unsigned long xorshift96();

void xorshiftSeed(unsigned long seed);
void randSeed(unsigned long seed);
void randSeed();

int8_t rand(int8_t minval, int8_t maxval);
int8_t rand(int8_t maxval);

uint8_t rand(uint8_t minval, uint8_t maxval);
uint8_t rand(uint8_t maxval);

int rand(int minval, int maxval);
int rand(int maxval);

unsigned int rand(unsigned int minval, unsigned int maxval);
unsigned int rand(unsigned int maxval);

uint8_t randMidiNote();

#endif /* MOZZI_RAND_H_ */
