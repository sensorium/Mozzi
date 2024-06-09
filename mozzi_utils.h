/*
 * mozzi_utils.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2012-2024 Tim Barrass and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
 */

#ifndef UTILS_H_
#define UTILS_H_


#include <Arduino.h>

#include "hardware_defines.h"

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
#if IS_AVR()
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
#if IS_AVR()
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
#if IS_AVR()
	PORTB &= B11011111;
#else
	digitalWrite(13, LOW);
#endif
}


/** @ingroup util
Given a power of 2, work out the number to shift right by to do a divide by the number, or shift left to multiply.
@param a power of 2, or any other number for that matter
@return the number of trailing zeros on the right hand end
*/
constexpr uint8_t trailingZerosConst(unsigned long v) { return ((v % 2) ? 0 : 1+trailingZerosConst(v >> 1)); }
/* NOTE: previous version of the above is super-nifty, but pulls in software float code on AVR (the platform where it makes a difference), leading to bloat and a compile time warning.
 * Sine the only use-case I could find works on a compile-time constant (template-parameter), I added a constexpr function in mozzi_utils.h, instead. I renamed it, too,
 * so, we'll learn about any use-case outside of this scope. If there are no reports of breakage, the following can probably be removed for good.
long trailingZeros(unsigned long v) {
	// find the number of trailing zeros in v, from http://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightFloatCast
	// there are faster methods on the bit twiddling site, but this is short
	float f = (float)(v & -v); // cast the least significant bit in v to a float
	return (*(uint32_t *)&f >> 23) - 0x7f;
}

Here's an alternate, trivial version:
uint8_t trailingZeros(uint16_t v) {
	uint8_t ret = 0;
	while ((v % 2) == 0) {
		v = v >> 1;
		++ret;
	}
	return ret;
} */


/** Convert BPM to milliseconds, which can be used to set the delay between beats for Metronome.
@param bpm beats per minute
*/
constexpr uint16_t BPMtoMillis(float bpm){
	//float seconds_per_beat = 60.f/bpm;
	return (uint16_t) (((float) 60.f/bpm)*1000);
}

#endif /* UTILS_H_ */
