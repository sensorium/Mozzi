
#ifndef UTILS_H_
#define UTILS_H_

#include "fixedMath.h"

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
unsigned long xorshift96();


void setupFastAnalogRead();


#endif /* UTILS_H_ */