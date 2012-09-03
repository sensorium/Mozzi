/*
  MozziTimer2.cpp - Using timer 2 (or 4 on ATmega32U4) with a configurable interrupt rate.

  Copyright 2012 Tim Barrass

  This file is part of Mozzi.

  Combines the hardware compatibility of Flexitimer2/MsTimer2 with the
  precision and efficiency of TimerTwo library, with other tweaks.
  (unknown, from https://bitbucket.org/johnmccombs, 4/2/2012).

  FlexiTimer2
  Wim Leers <work@wimleers.com>

  Based on MsTimer2
  Javier Valencia <javiervalencia80@gmail.com>

  History:
    25/Aug/2012 (TB) - Replaced scheme for setting up timers with
    	one based on TImerTwo library.
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

#ifndef MOZZITIMER2_H_
#define MOZZITIMER2_H_

#ifdef __AVR__
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#else
#error MozziTimer2 library only works on AVR architecture
#endif


namespace MozziTimer2
{

// call f every usec microseconds if start == false
// call f after delay of usec microseconds from call return if start == true
// max delay is 256*1024 clock cycles or 16,384 microseconds for a 16 MHz CPU
unsigned char set(unsigned int usec, void (*f)(), bool start = false);
void start();
void stop();

extern unsigned period_;
// timer start flag
extern bool start_;
// user function
extern void (*f_)();

// period in usec
inline unsigned period()
{
	return period_;
}

}

#endif /* MOZZITIMER2_H_ */
