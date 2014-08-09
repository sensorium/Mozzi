/*
 * IntMap.h
 *
 * Copyright 2012 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */

#ifndef INTMAP_H_
#define INTMAP_H_

/** A faster version of Arduino's map() function.
This uses ints instead of longs internally and does some of the maths at compile time.
*/
class IntMap {

public:
	/** Constructor.
	@param in_min the minimum of the input range.
	@param in_max the maximum of the input range.
	@param out_min the minimum of the output range.
	@param out_max the maximum of the output range.
	*/
	IntMap(int in_min, int in_max, int out_min, int out_max)
			: _IN_MIN(in_min),_IN_MAX(in_max),_OUT_MIN(out_min),_OUT_MAX(out_max),
			_MULTIPLIER((256L * (out_max-out_min)) / (in_max-in_min))
	{
		;
	}

	/** Process the next input value.
	@param n the next integer to process.
	@return the input integer mapped to the output range.
	*/
	int operator()(int n) const {
		return (int) (((_MULTIPLIER*(n-_IN_MIN))>>8) + _OUT_MIN);
	}

	
private:
	const int _IN_MIN, _IN_MAX, _OUT_MIN, _OUT_MAX;
	const long _MULTIPLIER;
};

#endif /* INTMAP_H_ */
