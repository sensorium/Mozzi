/*
 * TimerZero.h
 *
 * Copyright 2012 Tim Barrass, adapted from TimerTwo by John McCombs (date?).
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
// TB2012 search/replaced TimerTwo with TimerZero

#ifndef TimerZero_h
#define TimerZero_h

namespace TimerZero
{
extern unsigned period_;
// timer reset flag
extern bool reset_;
// user function
extern void (*f_)();
// call f every usec microseconds if reset == false
// call f after delay of usec microseconds from call return if reset == true
// max delay is 256*1024 clock cycles or 16,384 microseconds for a 16 MHz CPU
unsigned char init(unsigned usec, void (*f)(), bool reset = false);
// period in usec
inline unsigned period()
{
	return period_;
}
// start calls
void start();
// stop calls
void stop();
}

#endif // TimerZero

