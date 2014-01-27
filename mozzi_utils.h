
#ifndef UTILS_H_
#define UTILS_H_


#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

// macros for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


/** @ingroup util
Set digital pin 13 to output for testing timing with an oscilloscope.*/
inline
void setPin13Out()
{	
		DDRB |= B00100000;
}


/** @ingroup util
Set pin 13 high for testing timing with an oscilloscope.*/
inline
void setPin13High()
{	
 	PORTB |= B00100000;
}


/** @ingroup util
Set pin 13 low for testing timing with an oscilloscope.*/
inline
void setPin13Low()
{		
	PORTB &= B11011111;
}


long trailingZeros(unsigned long v);
unsigned int BPMtoMillis(float bpm);

#endif /* UTILS_H_ */
