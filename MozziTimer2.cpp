/*
  MozziTimer2.cpp - Using timer 2 (or 4 on ATmega32U4) with a configurable interrupt rate.

  Copyright 2012 Tim Barrass

  This file is part of Mozzi.

  Combines the hardware compatibility of Flexitimer2/MsTimer2 with the
  precision and efficiency of TimerTwo library (unknown, from https://bitbucket.org/johnmccombs, 4/2/2012), 
  with other tweaks.

  FlexiTimer2
  Wim Leers <work@wimleers.com>

  Based on MsTimer2
  Javier Valencia <javiervalencia80@gmail.com>

  History:
    31/Jan/2013 (TB) - added " || defined(__AVR_ATmega644P__)" 
    	wherever __AVR_ATmega328P__ is included in a list
    25/Aug/2012 (TB) - Replaced scheme for setting up timers with
    	one based on TimerTwo library.
  	Kept precompiler directives selecting processor models.
    16/Dec/2011 - Added Teensy/Teensy++ support (bperrybap)
		note: teensy uses timer4 instead of timer2
    25/April/10 - Based on MsTimer2 V0.5 (from 29/May/09)

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

#include <MozziTimer2.h>
#include <stdlib.h>


bool MozziTimer2::start_;
void (*MozziTimer2::f_)();
unsigned MozziTimer2::period_;

unsigned char MozziTimer2::set(unsigned int usec, void (*f)(), bool start)
{
	f_ = f;
	start_ = start;

	// assume F_CPU is a multiple of 1000000
	// number of clock ticks to delay usec microseconds
	unsigned long ticks = usec * (F_CPU/1000000);

	unsigned char num_prescale_factors;

#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || defined (__AVR_ATmega1280__) || defined (__AVR_ATmega2560__) || defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__) || defined(__AVR_ATmega644P__)

	// table by prescale = 2^n where n is the table index
	num_prescale_factors = 11;
	unsigned char * preScale;
	preScale = (unsigned char*) calloc (num_prescale_factors, sizeof(unsigned char));
	preScale[0] = (1 << CS20);
	preScale[1] = 0;
	preScale[2] = 0;
	preScale[3] = (1 << CS21);
	preScale[4] = 0;
	preScale[5] = (1 << CS21) | (1 << CS20);
	preScale[6] = (1 << CS22);
	preScale[7] = (1 << CS22) | (1 << CS20);
	preScale[8] = (1 << CS22) | (1 << CS21);
	preScale[9] = 0;
	preScale[10] = (1 << CS22) | (1 << CS21) | (1 << CS20);

	// determine prescale factor and TOP/OCR2A value
	// use minimum prescale factor
	unsigned char ps, i;
	for (i = 0; i < num_prescale_factors; i++)
	{
		ps = preScale[i];
		if (ps && (ticks >> i) <= 256)
			break;
	}

	//return error if usec is too large
	if (i == num_prescale_factors)
	{
		free (preScale);
		return false;
	}

	free (preScale);

	period_ = ((long)(ticks >> i) * (1 << i))/ (F_CPU /1000000);

	// disable timer 2 interrupts
	TIMSK2 = 0;

	// use system clock (clkI/O).
	ASSR &= ~(1 << AS2);
/* real
	// Clear Timer on Compare Match (CTC) mode
	TCCR2A = (1 << WGM21);
	*/
	// test
		//TCCR2B = _BV(WGM23);        // set mode as phase and frequency correct pwm, stop the timer
		//TCCR2A = 0;                 // clear control register A
	 // TCCR2A = _BV(COM2B1) | _BV(WGM20) | _BV(WGM22);// | _BV(COM2A1); // fast pwm, Clear OC2B on Compare Match when up-counting. Set OC2B on Compare Match when down-counting. TOP = OCRA.  PWM pin cleared on 
	 // TCCR2B = ps;
	// ken sherriff  
  TCCR2A = _BV(COM2A0) | _BV(COM2B1) | _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS22);
	  // end test
	  /*
	// only need prescale bits in TCCR2B
	TCCR2B = ps;
*/



	// set TOP so timer period is (ticks >> i)
	OCR2A = (ticks >> i) - 1;

	return true;

#elif defined (__AVR_ATmega8__) // same table as above, but shorter names and TCCR2 is different

	// table by prescale = 2^n where n is the table index
	num_prescale_factors = 11;
	unsigned char * preScale;
	preScale = (unsigned char*) calloc (num_prescale_factors, sizeof(unsigned char));
	preScale[0] = (1 << CS20);
	preScale[1] = 0;
	preScale[2] = 0;
	preScale[3] = (1 << CS21);
	preScale[4] = 0;
	preScale[5] = (1 << CS21) | (1 << CS20);
	preScale[6] = (1 << CS22);
	preScale[7] = (1 << CS22) | (1 << CS20);
	preScale[8] = (1 << CS22) | (1 << CS21);
	preScale[9] = 0;
	preScale[10] = (1 << CS22) | (1 << CS21) | (1 << CS20);

	// determine prescale factor and TOP/OCR2 value
	// use minimum prescale factor
	unsigned char ps, i;
	for (i = 0; i < num_prescale_factors; i++)
	{
		ps = preScale[i];
		if (ps && (ticks >> i) <= 256)
			break;
	}

	//return error if usec is too large
	if (i == num_prescale_factors)
	{
		free (preScale);
		return false;
	}

	free (preScale);

	period_ = ((long)(ticks >> i) * (1 << i))/ (F_CPU /1000000);

	// disable timer 2 interrupts
	TIMSK = 0;

	// use system clock (clkI/O).
	ASSR &= ~(1 << AS2);

	// Clear Timer on Compare Match (CTC) mode and set prescale bits
	TCCR2 = (1 << WGM21) | ps;

	// set TOP so timer period is (ticks >> i)
	OCR2 = (ticks >> i) - 1;

	return true;

#elif defined (__AVR_ATmega128__) // like 328 timer 0, with short names

	// table by prescale = 2^n where n is the table index
	num_prescale_factors = 11;
	unsigned char * preScale;
	preScale = (unsigned char*) calloc (num_prescale_factors, sizeof(unsigned char));
	preScale[0] = (1 << CS00);
	preScale[1] = 0;
	preScale[2] = 0;
	preScale[3] = (1 << CS01);
	preScale[4] = 0;
	preScale[5] = 0;
	preScale[6] = (1 << CS01) | (1 << CS00);
	preScale[7] = 0;
	preScale[8] = (1 << CS02);
	preScale[9] = 0;
	preScale[10] = (1 << CS02) | (1 << CS00);

	// determine prescale factor and TOP/OCR2 value
	// use minimum prescale factor
	unsigned char ps, i;
	for (i = 0; i < num_prescale_factors; i++)
	{
		ps = preScale[i];
		if (ps && (ticks >> i) <= 256)
			break;
	}

	//return error if usec is too large
	if (i == num_prescale_factors)
	{
		free (preScale);
		return false;
	}

	free (preScale);

	period_ = ((long)(ticks >> i) * (1 << i))/ (F_CPU /1000000);

	// disable timer 2 interrupts
	TIMSK = 0;

	// use system clock (clkI/O).
	ASSR &= ~(1 << AS0);

	// Clear Timer on Compare Match (CTC) mode and set prescale bits
	TCCR2 = (1 << WGM21) | ps;

	// set TOP so timer period is (ticks >> i)
	OCR2 = (ticks >> i) - 1;

	return true;

#elif defined (__AVR_ATmega32U4__)

	TCCR4B = 0;
	TCCR4A = 0;
	TCCR4C = 0;
	TCCR4D = 0;
	TCCR4E = 0;

	// table by prescale = 2^n where n is the table index
	num_prescale_factors = 15;
	unsigned char * preScale;
	preScale = (unsigned char*) calloc (num_prescale_factors, sizeof(unsigned char));
	preScale[0] = (1 << CS40);
	preScale[1] = (1 << CS41);
	preScale[2] = (1 << CS40) | (1 << CS41);
	preScale[3] = (1 << CS42);
	preScale[4] = (1 << CS40) | (1 << CS42);
	preScale[5] = (1 << CS41) | (1 << CS42);
	preScale[6] = (1 << CS40) | (1 << CS41) | (1 << CS42);
	preScale[7] = (1 << CS43);
	preScale[8] = (1 << CS40) | (1 << CS43);
	preScale[9] = (1 << CS41) | (1 << CS43);
	preScale[10] = (1 << CS40) | (1 << CS41) | (1 << CS43);
	preScale[11] = (1 << CS42) | (1 << CS43);
	preScale[12] = (1 << CS40) | (1 << CS42) | (1 << CS43);
	preScale[13] = (1 << CS41) | (1 << CS42) | (1 << CS43);
	preScale[14] = (1 << CS40) | (1 << CS41) | (1 << CS42) | (1 << CS43);

	// determine prescale factor and TOP/OCR4C value
	// use minimum prescale factor
	unsigned char ps, i;
	for (i = 0; i < num_prescale_factors; i++)
	{
		ps = preScale[i];
		if (ps && (ticks >> i) <= 256)
			break;
	}

	//return error if usec is too large
	if (i == num_prescale_factors)
	{
		free (preScale);
		return false;
	}

	free (preScale);

	period_ = ((long)(ticks >> i) * (1 << i))/ (F_CPU /1000000);

	// disable timer 4 interrupts
	TIMSK4 = 0;

	// set prescale bits in TCCR4B (not sure if/why PSR4 is needed, doc says it resets the prescaler)
	TCCR4B = ps | (1<<PSR4);

	// set TOP so timer period is (ticks >> i)
	OCR4A = (ticks >> i) - 1;

	return true;

#else
#error Unsupported CPU type
#endif

}



void MozziTimer2::start()
{
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || defined (__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__) || defined(__AVR_ATmega644P__)

	TIMSK2 |= (1 << OCIE2A);

#elif defined (__AVR_ATmega8__) || (__AVR_ATmega128__)

	TIMSK |= (1 << OCIE2);

#elif defined (__AVR_ATmega32U4__)

	TIMSK4 |= (1 << OCIE4A);

#endif
}



void MozziTimer2::stop()
{
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || defined (__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__) || defined(__AVR_ATmega644P__)

	TIMSK2 &=  ~(1 << OCIE2A);

#elif defined (__AVR_ATmega8__) || (__AVR_ATmega128__)

	TIMSK &= ~(1<<OCIE2);

#elif defined (__AVR_ATmega32U4__)

	TIMSK4 = 0;

#endif
}


#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || defined (__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__) || defined(__AVR_ATmega644P__)

// interrupt service routine that wraps a user defined function supplied by attachInterrupt
ISR(TIMER2_OVF_vect, ISR_NOBLOCK)
{
	  (*MozziTimer2::f_)();
}

/*
ISR(TIMER2_COMPA_vect, ISR_NOBLOCK) // ISR_NOBLOCK so it can be interrupted by Timer 1 (audio)
{
	// disable timer 2 interrupts
	TIMSK2 = 0;
	// call user function
	(*MozziTimer2::f_)();
	// in case f_ enabled interrupts
	cli();
	// clear counter if start_ is true
	if (MozziTimer2::start_)
	{
		// start counter
		TCNT2 = 0;
		// clear possible pending interrupt
		TIFR2 |= (1 << OCF2A);
	}
	// enable timer 2 COMPA interrupt
	TIMSK2 |= (1 << OCIE2A);
}

*/
#elif defined (__AVR_ATmega8__) || (__AVR_ATmega128__)

ISR(TIMER2_COMP_vect, ISR_NOBLOCK)
{
	// disable timer 2 interrupts
	TIMSK = 0;
	// call user function
	(*MozziTimer2::f_)();
	// in case f_ enabled interrupts
	cli();
	// clear counter if start_ is true
	if (MozziTimer2::start_)
	{
		// start counter
		TCNT2 = 0;
		// clear possible pending interrupt
		TIFR |= (1 << OCF2);
	}
	// enable timer 2 COMPA interrupt
	TIMSK |= (1 << OCIE2);
}


#elif defined (__AVR_ATmega32U4__)

ISR(TIMER4_COMPA_vect, ISR_NOBLOCK) // this all needs checking
{
	// disable timer 4 interrupts
	TIMSK4 = 0;
	// call user function
	(*MozziTimer2::f_)();
	// in case f_ enabled interrupts
	cli();
	// clear counter if start_ is true
	if (MozziTimer2::start_)
	{
		// start counter
		TCNT4 = 0;
		// clear possible pending interrupt
		TIFR4 |= (1 << OCF4A);
	}
	// enable timer 4 COMPA interrupt
	TIMSK4 |= (1 << OCIE4A);
}

#endif


