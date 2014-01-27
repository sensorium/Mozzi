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

char rand(char minval, char maxval);
char rand(char maxval);

unsigned char rand(unsigned char minval, unsigned char maxval);
unsigned char rand(unsigned char maxval);

int rand(int minval, int maxval);
int rand(int maxval);

unsigned int rand(unsigned int minval, unsigned int maxval);
unsigned int rand(unsigned int maxval);

unsigned char randMidiNote();

#endif /* MOZZI_RAND_H_ */
