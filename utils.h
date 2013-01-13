
#ifndef UTILS_H_
#define UTILS_H_


#include "Arduino.h"

/** @defgroup util Mozzi utility macros and definitions
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


#endif /* UTILS_H_ */
