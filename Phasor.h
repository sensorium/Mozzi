/*
 * Phasor.h
 *
 * Copyright 2012 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
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

#define PHASOR_MAX_VALUE_UL 4294967295UL

/** Phasor repeatedly generates a high resolution ramp at a variable frequency.
The output of Phasor.next() is an unsigned number between 0 and 4294967295, the
maximum that can be expressed by an unsigned 32 bit integer.
@tparam UPDATE_RATE the rate at which the Phasor will be updated,
usually CONTROL_RATE or AUDIO_RATE.
*/

template <unsigned int UPDATE_RATE>
class Phasor
{
private:
	uint32_t current_value;
	volatile uint32_t step_size;

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
	uint32_t next()
	{
		current_value += step_size; // will wrap
		return current_value;
	}

	/** Set the current value of the phasor. The Phasor will continue incrementing from this
	value using any previously calculated step size.
	 */
	inline
	void set(uint32_t value)
	{
		current_value=value;
	}


	/** Set the Phasor frequency with an unsigned int.
	@param frequency is how many times per second to count from
	0 to the maximum uint32_t value 4294967295.
	@note Timing 8us
	 */
	inline
	void setFreq( int frequency)
	{
		step_size = ((((uint32_t)((PHASOR_MAX_VALUE_UL>>8)+1))/(UPDATE_RATE))*frequency)<<8;
	}


	/** Set the Phasor frequency with a float.
	@param frequency is  how many times per second to count from
	0 to the maximum uint32_t value 4294967295.
	 */
	inline
	void setFreq(float frequency)
	{ // 1 us - using float doesn't seem to incur measurable overhead with the oscilloscope
		step_size = (uint32_t)(((float)PHASOR_MAX_VALUE_UL/UPDATE_RATE)*frequency);
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
	uint32_t phaseIncFromFreq(int frequency)
	{
		return ((((uint32_t)((PHASOR_MAX_VALUE_UL>>8)+1))/(UPDATE_RATE))*frequency)<<8;
	}


	/** Set a specific phase increment.  See phaseIncFromFreq().
	@param stepsize a phase increment value as calculated by phaseIncFromFreq().
	 */
	inline
	void setPhaseInc(uint32_t stepsize)
	{
		step_size = stepsize;
	}

};

/**
@example 06.Synthesis/PWM_Phasing/PWM_Phasing.ino
This example demonstrates the Phasor class.
*/

#endif /* PHASOR_H_ */
