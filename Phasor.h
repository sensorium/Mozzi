/*
 * Phasor.h
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

#ifndef PHASOR_H_
#define PHASOR_H_

#include "Arduino.h"
#include "mozzi_fixmath.h"
#include <util/atomic.h>

#define PHASOR_MAX_VALUE_UL 4294967295UL

/** Phasor repeatedly generates a high resolution ramp at a variable frequency.
The output of Phasor.next() is an unsigned number between 0 and 4294967295, the
maximum that can be expressed by an unsigned long.
@tparam UPDATE_RATE the rate at which the Phasor will be updated,
usually CONTROL_RATE or AUDIO_RATE.
*/

template <unsigned int UPDATE_RATE>
class Phasor
{
private:
	unsigned long current_value;
	volatile unsigned long step_size;

public:
	/** Constructor. "Phasor <AUDIO_RATE> myphasor;"
	makes a Phasor which updates at AUDIO_RATE.
	*/
	Phasor (){
		;
	}

	/** Increments one step along the phase.
	@return the next value.
	 */
	inline
	unsigned long next()
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			current_value += step_size; // will wrap
		}
		return current_value;
	}

	/** Set the current value of the phasor. The Phasor will continue incrementing from this
	value using any previously calculated step size.
	 */
	inline
	void set(unsigned long value)
	{
		current_value=value;
	}


	/** Set the Phasor frequency with an unsigned int.
	@param frequency is how many times per second to count from
	0 to the maximum unsigned long value 4294967295.
	 */
	inline
	void setFreq(unsigned int frequency)
	{
		step_size = ((((unsigned long)((PHASOR_MAX_VALUE_UL>>8)+1))/(UPDATE_RATE))*frequency)<<8;
	}


	/** Set the Phasor frequency with a float.
	@param frequency is  how many times per second to count from
	0 to the maximum unsigned long value 4294967295.
	 */
	inline
	void setFreq(float frequency)
	{ // 1 us - using float doesn't seem to incur measurable overhead with the oscilloscope
		//ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		//{
		step_size = ((unsigned long)((((unsigned long)((PHASOR_MAX_VALUE_UL>>8)+1))/(UPDATE_RATE))*frequency))<<8;

		//}
	}
};

#endif /* PHASOR_H_ */
