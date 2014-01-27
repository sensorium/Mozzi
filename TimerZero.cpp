/*
 * TimerZero.cpp
 *
 * Copyright 2012 Tim Barrass
 *
 * This file is part of TimerZero, a library for Arduino.
 *
 * TimerZero is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TimerZero is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TimerZero.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// Based on TimerTwo,
// downloaded from https://bitbucket.org/johnmccombs, 4/2/2012
//
// TB2012 added Arduino.h include
// TB2012 replaced Timer 2 prescale factors with ones for Timer 0
// TB2012 replaced all Timer 2 register names with ones for timer 0
// TB2012 search/replaced TimerTwo with TimerZero
// TB2012 changed preScale array to suit Timer0

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <TimerZero.h>
#include <Arduino.h>

// allowed prescale factors
/*
#define PS1    (1 << CS20)
#define PS8    (1 << CS21)
#define PS32   (1 << CS21) | (1 << CS20)
#define PS64   (1 << CS22)
#define PS128  (1 << CS22) | (1 << CS20)
#define PS256  (1 << CS22) | (1 << CS21)
#define PS1024 (1 << CS22) | (1 << CS21) | (1 << CS20)
*/
#define PS1    (1 << CS00)
#define PS8    (1 << CS01)
#define PS64   (1 << CS01) | (1 << CS00)
#define PS256   (1 << CS02)
#define PS1024  (1 << CS02) | (1 << CS00)

// table by prescale = 2^n where n is the table index
static const unsigned char __attribute__((section(".progmem.data"))) preScale[] =
        {
                PS1, 0, 0, PS8, 0, 0, PS64, 0, PS256, 0, PS1024
        };

bool TimerZero::reset_;
void (*TimerZero::f_)();
unsigned TimerZero::period_;
//------------------------------------------------------------------------------
// initialize timer 0
unsigned char TimerZero::init(unsigned usec, void (*f)(), bool reset)
{
	f_ = f;
	reset_ = reset;
	// assume F_CPU is a multiple of 1000000
	// number of clock ticks to delay usec microseconds
	unsigned long ticks = usec * (F_CPU/1000000);
	// determine prescale factor and TOP/OCR2A value
	// use minimum prescale factor
	unsigned char ps, i;
	for (i = 0; i < sizeof(preScale); i++)
	{
		ps = pgm_read_byte(&preScale[i]);
		if (ps && (ticks >> i) <= 256)
			break;
	}
	//return error if usec is too large
	if (i == sizeof(preScale))
		return false;
	period_ = ((long)(ticks >> i) * (1 << i))/ (F_CPU /1000000);
	//  Serial.println(i, DEC);
	// disable timer 0 interrupts
	TIMSK0 = 0;
	// use system clock (clkI/O)
	//ASSR &= ~(1 << AS2);
	// Clear Timer on Compare Match (CTC) mode
	TCCR0A = (1 << WGM01);

	// only need prescale bits in TCCR0B
	TCCR0B = ps;

	// set TOP so timer period is (ticks >> i)
	OCR0A = (ticks >> i) - 1;
	return true;
}
//------------------------------------------------------------------------------
// Start timer zero interrupts
void TimerZero::start()
{
	TIMSK0 |= (1 << OCIE0A);
}
//------------------------------------------------------------------------------
// Stop timer 2 interrupts
void TimerZero::stop()
{
	TIMSK0 = 0;
}
//------------------------------------------------------------------------------
// ISR for timer 0 Compare A interrupt
// TB2012 added ISR_NOBLOCK so it can be interrupted by Timer 1 (audio)
ISR(TIMER0_COMPA_vect, ISR_NOBLOCK)
{
	// disable timer 0 interrupts
	TIMSK0 = 0;
	// call user function
	(*TimerZero::f_)();
	// in case f_ enabled interrupts
	cli();
	// clear counter if reset_ is true
	if (TimerZero::reset_)
	{
		// reset counter
		TCNT0 = 0;
		// clear possible pending interrupt
		TIFR0 |= (1 << OCF0A);
	}
	// enable timer 2 COMPA interrupt
	TIMSK0 |= (1 << OCIE0A);
}




