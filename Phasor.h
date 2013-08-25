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

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
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
	@note Timing 8us
	 */
	inline
	void setFreq( int frequency)
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
		step_size = (unsigned long)(((float)PHASOR_MAX_VALUE_UL/UPDATE_RATE)*frequency);

		//}
	}

	/** phaseIncFromFreq() and setPhaseInc() are for saving processor time when sliding between frequencies.
	Instead of recalculating the phase increment for each frequency in between, you
	can just calculate the phase increment for each end frequency with
	phaseIncFromFreq(), then use a Line to interpolate on the fly and use
	setPhaseInc() to set the phase increment at each step. (Note: I should really
	profile this with the oscilloscope to see if it's worth the extra confusion!)
	@param frequency for which you want to calculate a phase increment value.
	@return the phase increment value which will produce a given frequency.
	*/
	inline
	unsigned long phaseIncFromFreq(int frequency)
	{
		return ((((unsigned long)((PHASOR_MAX_VALUE_UL>>8)+1))/(UPDATE_RATE))*frequency)<<8;
	}


	/** Set a specific phase increment.  See phaseIncFromFreq().
	@param stepsize a phase increment value as calculated by phaseIncFromFreq().
	 */
	inline
	void setPhaseInc(unsigned long stepsize)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			step_size = stepsize;
		}
	}

};

/**
@example 06.Synthesis/PWM_Phasing/PWM_Phasing.ino
This example demonstrates the Phasor class.
*/

#endif /* PHASOR_H_ */
