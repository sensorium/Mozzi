/*
 *  Interrupt and PWM utilities for 16 bit Timer3 on ATmega168/328
 *  Original code by Jesse Tane for http://labs.ideo.com August 2008
 *  Modified March 2009 by Jérôme Despatis and Jesse Tane for ATmega328 support
 *  Modified June 2009 by Michael Polli and Jesse Tane to fix a bug in setPeriod() which caused the timer to stop
 *  Modified Oct 2009 by Dan Clemens to work with timer3 of the ATMega1280 or Arduino Mega
 *  Modified April 2012 by Paul Stoffregen
 *
 *  This is free software. You can redistribute it and/or modify it under
 *  the terms of Creative Commons Attribution 3.0 United States License.
 *  To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/us/
 *  or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 */

#include "TimerOne.h"

TimerOne Timer1;              // preinstatiate

unsigned int TimerOne::pwmPeriod = 0;
unsigned char TimerOne::clockSelectBits = 0;

// TB 15-2-2013 removed this and added the more direct version in MozziGuts.cpp
// saves about 1us per ISR, at 16384Hz that's 16ms per second saved.  Seems worth it.
/*
// interrupt service routine that wraps a user defined function supplied by attachInterrupt
ISR(TIMER1_OVF_vect)
{  //SET_PIN13_HIGH;
	Timer1.isrCallback();
	//SET_PIN13_LOW; // 2us
}
*/
