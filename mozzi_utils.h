
#ifndef UTILS_H_
#define UTILS_H_


#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

// macros for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_UINT8_T(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_UINT8_T(sfr) |= _BV(bit))
#endif


/** @ingroup util
Set digital pin 13 to output for testing timing with an oscilloscope.*/
inline
void setPin13Out()
{
#if defined(__AVR__)
	DDRB |= B00100000;
#else
	pinMode(13, OUTPUT);
#endif
}


/** @ingroup util
Set pin 13 high for testing timing with an oscilloscope.*/
inline
void setPin13High()
{
#if defined(__AVR__)
	PORTB |= B00100000;
#else
	digitalWrite(13, HIGH);
#endif
}


/** @ingroup util
Set pin 13 low for testing timing with an oscilloscope.*/
inline
void setPin13Low()
{
#if defined(__AVR__)
	PORTB &= B11011111;
#else
	digitalWrite(13, LOW);
#endif
}


long trailingZeros(unsigned long v);
unsigned int BPMtoMillis(float bpm);

#endif /* UTILS_H_ */
