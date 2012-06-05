/*
 * EadFloat.h
 *
 * Adapted from extlib_util.h:
 * Prototypes for utility functions used in pd externals
 * Copyright (c) 2000-2003 by Tom Schouten
 *
 * Copyright 2012 unbackwards@gmail.com, 2000-2003 Tom Schouten
 *
 * This file is part of Cuttlefish.
 *
 * Cuttlefish is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cuttlefish is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cuttlefish.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef EADFLOAT_H_
#define EADFLOAT_H_

#include "math.h"
//#include <fixed8n8.h>


/** Exponential attack decay envelope. This produces a natural sounding
envelope. It calculates a new value each time next() is called, which can be
used to change the amplitude or timbre of a sound.  The equations used are:
 attack: y = a*pow((1+r),x)
 decay: y = a*pow((1-r),x)
*/

class EadFloat
{

public:

	/** Constructor.
	UPDATE_RATE would usually be CONTROL_RATE or AUDIO_RATE, unless
	you design another scheme for updating, for instance by taking turns for various
	control changes in a rotating schedule to spread out calculations made in
	successive updateControl() routines. */
	EadFloat(const unsigned int UPDATE_RATE);


	/** Set the attack time in milliseconds. The attack time means how long it takes
	for the values returned by successive calls of the next() method to change from
	0 to 1. */
	inline
	void setAttack(unsigned int milliseconds)
	{
		attack = milliseconds_2_one_minus_realpole(milliseconds);
	}


	/** Set the decay time in milliseconds. The decay time means how long
	the decay phase takes to change from 1 to 0. */
	inline
	void setDecay(unsigned int milliseconds)
	{
		decay = milliseconds_2_one_minus_realpole(milliseconds);
	}


	/** Set attack and decay times in milliseconds. */
	inline
	void set(unsigned int attackms, unsigned int decayms)
	{
		setAttack(attackms);
		setDecay(decayms);
	}


	/** Start the envelope from the beginning. This can be used at any
	time, even if the previous envelope is not finished. */
	inline
	void start()
	{
		state = 0;
		target = true;
	}


	/** Calculate and return the next envelope value. */
	// 30us
	inline
	float next()
	{
		switch(target)
		{
		case true: /* attack phase */
			//attack: state = a*pow((1+r),state)
			state += attack * (1 - state);
			target = (state <= ENVELOPE_MAX);
			break;
		default: /* decay phase */
			state -= decay*state;
			break;
		}
		return state;
	}


	/** Return the current state value.
	*/
	inline
	float  getState()
	{
		return state;
	}


	//private:

	float  attack;
	float  decay;
	float state;
	bool target;
	const unsigned int UPDATE_RATE;


	/* convert milliseconds to 1-p, with p a real pole */
	inline
	float milliseconds_2_one_minus_realpole(unsigned int milliseconds)
	{
		float r = -expm1(NUMERATOR / (UPDATE_RATE * milliseconds));
		//if (!(r < 1.0f))
		//	r = 1.0f;
		return r;
	}


	// Compute exp(x) - 1 without loss of precision for small values of x.
	inline
	float expm1(float x)
	{
		if (fabs(x) < 1e-5)
		{
			return x + 0.5*x*x;
		}
		else
		{
			return exp(x) - 1.0;
		}
	}


	/* exponential range for envelopes is 60dB */
	static const float ENVELOPE_RANGE;
	static const float ENVELOPE_MAX;
	static const float NUMERATOR;

};

#endif /* EADFLOAT_H_ */
