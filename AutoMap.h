/*
 * AutoMap.h
/*
 * AutoMap.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2012-2024 Tim Barrass and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
 */

#ifndef AUTOMAP_H_
#define AUTOMAP_H_

// for map - maybe rewrite my own templated map for better efficiency
#include <Arduino.h> // for map

#include "AutoRange.h"

/** @defgroup sensortools Automatic range adjustment
*/

/** @ingroup sensortools
Automatically map an input value to an output range without knowing the precise range of inputs beforehand.
*/

class AutoMap : public AutoRange<int>
{
public:
	/** Constructor.
	@param min_expected the minimum possible input value.
	@param max_expected the maximum possible input value.
	*/
	AutoMap(int min_expected, int max_expected, int map_to_min, int map_to_max)
		: inherited(min_expected,max_expected),map_min(map_to_min), map_max(map_to_max)
	{
	}
	

	/** Process the next value and return it mapped to the range which was set in the constructor.
	Can use the operator instead if you prefer, eg. myMap(n) instead of myMap.next(n).
	@param n the next value to process.
	@return the input value mapped to the range which was set in the constructor.
	*/
	inline
	int next(int n)
  {
		inherited::next(n);
		return map(n,inherited::getMin(),inherited::getMax(),map_min,map_max);
	}

	/** Process the next value and return it mapped to the range which was set in the constructor.
	This is an alternative to next() if you prefer, eg. myMap(n) instead of myMap.next(n).
	@param n the next value to process.
	@return the input value mapped to the range which was set in the constructor.
	*/
	inline
	int operator()(int n)
  {
		return next(n);
	}


private:
	typedef AutoRange <int> inherited;
	int map_min, map_max;
};


/**
@example 03.Sensors/Knob_LDR_x2_WavePacket/Knob_LDR_x2_WavePacket.ino
This example demonstrates the AutoMap class.
*/

#endif        //  #ifndef AUTOMAP_H_
