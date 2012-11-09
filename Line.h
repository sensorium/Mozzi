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

#include "Arduino.h"

/** For linear changes with a minimum of calculation at each step. For instance,
you can use Line to make an oscillator glide from one frequency to another,
pre-calculating the required phase increments for each end and then letting your
Line change the phase increment with only a simple addition at each step.
@tparam T the type of numbers to use. For example, Line <int> myline; makes a
Line which uses ints.
*/

template <class T>
class Line
{
private:
	T current_value;
	T step_size;

public:
	/** Constructor. Use the template parameter to set the type of numbers you
	want to use. For example, Line <int> myline; makes a Line which uses ints.
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
		current_value += step_size;
		return current_value;
	}

	/** Set the current value of the line. The Line will continue incrementing from this
	value using any previously calculated step size.
	 */
	inline
	void set(T value)
	{
		current_value=value;
	}

	/** Given a target value and the number of steps to take on the way, this calculates
	the step size needed to get there from the current value.
	 */
	inline
	void set(T targetvalue, T num_steps)
	{
		step_size=(T)((((float)targetvalue-current_value)/num_steps));
	}

	/** Given a new starting value, target value and the number of steps to take on the way,
	this sets the step size needed to get there.
	 */
	inline
	void set(T startvalue, T targetvalue, T num_steps)
	{
		set(startvalue);
		set(targetvalue, num_steps);
	}
};

#endif /* LINE_H_ */


