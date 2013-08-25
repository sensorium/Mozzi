/*
 * Line.h
 *
 * Copyright 2012 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mozzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mozzi.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef LINE_H_
#define LINE_H_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include <util/atomic.h>

/** For linear changes with a minimum of calculation at each step. For instance,
you can use Line to make an oscillator glide from one frequency to another,
pre-calculating the required phase increments for each end and then letting your
Line change the phase increment with only a simple addition at each step.
@tparam T the type of numbers to use. For example, Line \<int\> myline; makes a
Line which uses ints. 
@note Watch out for underflows in the internal calcualtion of Line() if you're not
using floats (but on the other hand try to avoid lots of floats, they're too slow!). 
If it seems like the Line() is not working, there's a good chance you need to
scale up the numbers you're using, so internal calculations don't get truncated
away. Use Mozzi's fixed-point number types in mozzi_fixmath.h, which enable you to
represent fractional numbers. Google "fixed point arithmetic" if this is new to
you.
*/

template <class T>
class Line
{
private:
	volatile T current_value; // volatile because it could be set in control interrupt and updated in audio
	T step_size;

public:
	/** Constructor. Use the template parameter to set the type of numbers you
	want to use. For example, Line \<int\> myline; makes a Line which uses ints.
	 */
	Line ()
	{
		;
	}

	/** Increments one step along the line.
	@return the next value.
	 */
	inline
	T next()
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			current_value += step_size;
		}
		return current_value;
	}



	/** Set the current value of the line. 
	The Line will continue incrementing from this
	value using any previously calculated step size.
	@param value the number to set the Line's current_value to.
	 */
	inline
	void set(T value)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			current_value=value;
		}
	}



	/** Given a target value and the number of steps to take on the way, this calculates the step size needed to get there from the current value.
	@param targetvalue the value to move towards.
	@param num_steps how many steps to take to reach the target.
	 */
	inline
	void set(T targetvalue, T num_steps)
	{
		step_size=(T)((((float)targetvalue-current_value)/num_steps));
	}

	/** Given a new starting value, target value and the number of steps to take on the way, this sets the step size needed to get there.
	@param startvalue the number to set the Line's current_value to.
	@param targetvalue the value to move towards.
	@param num_steps how many steps to take to reach the target.
	 */
	inline
	void set(T startvalue, T targetvalue, T num_steps)
	{
		set(startvalue);
		set(targetvalue, num_steps);
	}
};


/**
@example 02.Control/Control_Tremelo/Control_Tremelo.ino
This example demonstrates the Line class.
*/

#endif /* LINE_H_ */
