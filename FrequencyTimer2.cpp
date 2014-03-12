/*
  FrequencyTimer2.h - A frequency generator and interrupt generator library
  Author: Jim Studt, jim@federated.com
  Copyright (c) 2007 David A. Mellis.  All right reserved.

  http://www.arduino.cc/playground/Code/FrequencyTimer2

  Version 2 - updated by Paul Stoffregen, paul@pjrc.com
  for compatibility with newer hardware and Arduino 1.0
  
  Modified by Tim Barrass 2013, 
  -added support for ATMEGA32U4 processors (Leonardo,Teensy2.0)
  	using Timer 4 instead of Timer 2
  - commented out ISR for use with Mozzi which defines its own
 Modified by Tim Barrass 2014,
  - changed setPeriod() to setPeriodMicroSeconds() and added setPeriodCPUCycles() for more accuracy
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include <FrequencyTimer2.h>

#include <avr/interrupt.h>
#include "Arduino.h"

void (*FrequencyTimer2::onOverflow)() = 0;
uint8_t FrequencyTimer2::enabled = 0;

/*
#if defined(TIMER2_COMPA_vect)
ISR(TIMER2_COMPA_vect)
#elif defined(TIMER2_COMP_vect)
ISR(TIMER2_COMP_vect)
#elif defined(TIMER4_COMPA_vect)
ISR(TIMER4_COMPA_vect)
#else
#error "This board does not have a hardware timer which is compatible with FrequencyTimer2"
void dummy_function(void)
#endif
{
    static uint8_t inHandler = 0; // protect us from recursion if our handler enables interrupts

    if ( !inHandler && FrequencyTimer2::onOverflow) {
	inHandler = 1;
	(*FrequencyTimer2::onOverflow)();
	inHandler = 0;
    }
}
*/

void FrequencyTimer2::setOnOverflow( void (*func)() )
{
	FrequencyTimer2::onOverflow = func;
#if defined(TIMSK2)
	// enable compare match interrupt
	if ( func) TIMSK2 |= _BV(OCIE2A);
	else TIMSK2 &= ~_BV(OCIE2A);
#elif defined(TIMSK)
	if ( func) TIMSK |= _BV(OCIE2);
	else TIMSK &= ~_BV(OCIE2);
#elif defined(TIMSK4)
	// output compare interrupt enable 4A
	if ( func) TIMSK4 = _BV(OCIE4A);
	else TIMSK4 = 0;
#endif
}


void FrequencyTimer2::setPeriodMicroSeconds(unsigned long period) {
	period *= clockCyclesPerMicrosecond();
	setPeriodCPUCycles(period);
}


void FrequencyTimer2::setPeriodCPUCycles(unsigned long period){
	uint8_t pre, top;

	if ( period == 0) period = 1;
	//period *= clockCyclesPerMicrosecond();
	//period /= 2;            // we work with half-cycles before the toggle

	Serial.println(period);
#if defined(TCCR2A) || defined(TCCR2)
	if ( period <= 256) {
		pre = 1;
		top = period-1;
	} else if ( period <= 256L*8) { // this for AUDIO_RATE 16384, pre=2 is a bitfield 010 which means prescaler = 8
		pre = 2;
		top = period/8-1;
	} else if ( period <= 256L*32) {
		pre = 3;
		top = period/32-1;
	} else if ( period <= 256L*64) {
		pre = 4;
		top = period/64-1;
	} else if ( period <= 256L*128) {
		pre = 5;
		top = period/128-1;
	} else if ( period <= 256L*256) {
		pre = 6;
		top = period/256-1;
	} else if ( period <= 256L*1024) {
		pre = 7;
		top = period/1024-1;
	} else {
		pre = 7;
		top = 255;
	}
#elif defined(TCCR4A)
	unsigned long prescaler = 1;
	for(int i=1; i<=16; i++){
		if ( period <= 256*prescaler){
			pre = i;
			top = period/prescaler -1;
			break;
		}
		prescaler *= 2;
	}

	//pre = 4; // this for AUDIO_RATE 16384, pre=4 is a bitfield 100 which means prescaler = 8
	//top = period/8-1;
#endif

#if defined(TCCR2A)
	TCCR2B = 0;
	TCCR2A = 0;
	TCNT2 = 0;
#if defined(ASSR) && defined(AS2)
	ASSR &= ~_BV(AS2);    // use clock, not T2 pin
#endif
	OCR2A = top;
	//Clear Timer on Compare Match (CTC) mode ..... TB2014 p158, looks like Toggle OC2A on Compare Match
	TCCR2A = (_BV(WGM21) | ( FrequencyTimer2::enabled ? _BV(COM2A0) : 0)); 
	TCCR2B = pre;
#elif defined(TCCR2)
	TCCR2 = 0;
	TCNT2 = 0;
	ASSR &= ~_BV(AS2);    // use clock, not T2 pin
	OCR2 = top;
	TCCR2 = (_BV(WGM21) | ( FrequencyTimer2::enabled ? _BV(COM20) : 0)  | pre);
#elif defined(TCCR4A) // TB2013 for 32u4 (leonardo,teensy)
	TCCR4A = 0;
	TCCR4B = 0;
	TCCR4C = 0;
	TCCR4D = 0;
	TCCR4E = 0;
	TCNT4 = 0;
	OCR4C= top; // Table 15-19
	//TCCR4A = _BV(COM4A1);
	TCCR4B = pre ;
	TIMSK4 = FrequencyTimer2::enabled ? _BV(OCIE4A) : 0;
#endif
}


unsigned long  FrequencyTimer2::getPeriod()
{
#if defined(TCCR2B)
	uint8_t p = (TCCR2B & 7);
	unsigned long v = OCR2A;
#elif defined(TCCR)
	uint8_t p = (TCCR2 & 7);
	unsigned long v = OCR2;
#elif defined(TCCR4B)
	uint8_t p = (TCCR4B & 7);
	unsigned long v = OCR4C;
#endif
	uint8_t shift;

	switch(p) {
	case 0 ... 1:
		shift = 0;
		break;
	case 2:
		shift = 3;
		break;
	case 3:
		shift = 5;
		break;
	case 4:
		shift = 6;
		break;
	case 5:
		shift = 7;
		break;
	case 6:
		shift = 8;
		break;
	case 7:
		shift = 10;
		break;
	}
	return (((v+1) << (shift+1)) + 1) / clockCyclesPerMicrosecond();   // shift+1 converts from half-period to period
}


void FrequencyTimer2::enable()
{
	FrequencyTimer2::enabled = 1;
#if defined(TCCR2A)
	//Toggle OC2A on Compare Match
	TCCR2A |= _BV(COM2A0);
#elif defined(TCCR2)
	TCCR2 |= _BV(COM20);
#elif defined(TCCR4A)
	//TCCR4A |= _BV(COM4A0);
	TIMSK4 = _BV(OCIE4A);
#endif
}

void FrequencyTimer2::disable()
{
	FrequencyTimer2::enabled = 0;
#if defined(TCCR2A)
	TCCR2A &= ~_BV(COM2A0);
#elif defined(TCCR2)
	TCCR2 &= ~_BV(COM20);
#elif defined(TCCR4A)
	//TCCR4A &= ~_BV(COM4A0);
	TIMSK4 = 0;
#endif
}
