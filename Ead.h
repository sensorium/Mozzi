/*
 * Ead.h
 *
 * Adapted from extlib_util.h:
 * Prototypes for utility functions used in pd externals
 * Copyright (c) 2000-2003 by Tom Schouten
 *
 * Copyright 2012 sweatsonics@sweatsonics.com, 2000-2003 Tom Schouten
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

#ifndef EAD_H_
#define EAD_H_

#include "math.h"
#include <fixed8n8.h>

/** Exponential attack decay envelope.
*/
class Ead
{

public:
	Ead(const unsigned int UPDATE_RATE);


	inline
	void setAttack(unsigned int milliseconds)
	{
		attack = float2fix(milliseconds_2_one_minus_realpole(milliseconds));
	}


	inline
	void setDecay(unsigned int milliseconds)
	{
		decay = float2fix(milliseconds_2_one_minus_realpole(milliseconds));
	}


	inline
	void set(unsigned int attackms,unsigned int decayms)
	{
		setAttack(attackms);
		setDecay(decayms);
	}


	inline
	void start()
	{
		state = 0;
		target = true;
	}


	inline
	unsigned int next()
	{
		switch(target)
		{
		case true: /* attack phase */
			state += attack * (float2fix(1) - state);
			target = (state <= ENVELOPE_MAX);
			break;
		default: /* decay phase */
			state -= decay*state;
			break;
		}
		return state; // 8n8
	}



	inline
	unsigned int  getAttack()
	{
		return attack;
	}

	inline
	unsigned int  getDecay()
	{
		return decay;
	}

	inline
	unsigned int  getState()
	{
		return state;
	}


private:
	unsigned int  attack;
	unsigned int  decay;
	unsigned int state; // fixed 8n8 format
	bool target;
	const unsigned int UPDATE_RATE;

	/* convert milliseconds to 1-p, with p a real pole */
	inline
	float milliseconds_2_one_minus_realpole(unsigned int milliseconds)
	{
		float r;

		//if (milliseconds < 0) milliseconds = 0;
		r = -expm1(1000.0f * log(ENVELOPE_RANGE) / (UPDATE_RATE * milliseconds));
		if (!(r < 1.0f))
			r = 1.0f;

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

	/* exponential range for envelopes is ??dB */
	static const unsigned char ENVELOPE_RANGE; // use #define instead?
	static const unsigned int ENVELOPE_MAX;
};

#endif /* EAD_H_ */
