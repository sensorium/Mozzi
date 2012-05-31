// this file seems pointless since it only works if .c file is also included

#ifndef UTILS_H_
#define UTILS_H_

#include "Arduino.h" // for pow function

// macros for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

float mtof(float x);
unsigned long xorshift96();

byte _mod(byte n, byte d);
byte _div(byte n, byte d);
byte _rnd(byte min, byte max);

void setupFastADC();


#endif /* UTILS_H_ */