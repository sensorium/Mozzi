
#ifndef UTILS_H_
#define UTILS_H_

#include "fixedMath.h"
#include "Arduino.h"

/** @defgroup util Mozzi utility functions
*/

/** @ingroup util
Set digital pin 13 to output for testing timing with an oscilloscope.*/
#define SET_PIN13_OUT	(DDRB = DDRB | B00100000)
/** @ingroup util
Set pin 13 high for testing timing with an oscilloscope.*/
#define SET_PIN13_HIGH 	(PORTB = PORTB | B00100000)
/** @ingroup util
Set pin 13 low for testing timing with an oscilloscope.*/
#define SET_PIN13_LOW	(PORTB = B00000000)

// macros for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif



float mtof(float x);
Q16n16 Q16n16_mtof(Q16n16 midival);
unsigned char randomMidi();
void randSeed(unsigned long seed);
void xorshiftSeed(unsigned long seed);
unsigned long xorshift96();
char rand(char minval, char maxval);
unsigned char rand(unsigned char minval, unsigned char maxval);
int rand( int minval,  int maxval);
unsigned int rand(unsigned int minval, unsigned int maxval);
char rand(char maxval);
unsigned char rand(unsigned char maxval);
int rand(int maxval);
unsigned int rand(unsigned int maxval);



// hack for Teensy 2 (32u4), pasted from hardware/arduino/variants/leonardo/pins_arduino.h
#if defined(__AVR_ATmega32U4__)
//	__AVR_ATmega32U4__ has an unusual mapping of pins to channels
extern const uint8_t PROGMEM analog_pin_to_channel_PGM[];
#define analogPinToChannel(P)  ( pgm_read_byte( analog_pin_to_channel_PGM + (P) ) )
#endif

void setupFastAnalogRead();
void startAnalogRead(unsigned char pin);
int receiveAnalogRead();
#endif /* UTILS_H_ */