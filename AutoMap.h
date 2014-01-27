/*
 * AutoMap.h
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

#ifndef AUTOMAP_H_
#define AUTOMAP_H_

// for map - maybe rewrite my own templated map for better efficiency
#if ARDUINO >= 100
 #include "Arduino.h" // for map
#else
 #include "WProgram.h"
#endif

#include "AutoRange.h"

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
	int next(int n){
		inherited::next(n);
		return map(n,inherited::getMin(),inherited::getMax(),map_min,map_max);
	}
	
	/** Process the next value and return it mapped to the range which was set in the constructor.
	This is an alternative to next() if you prefer, eg. myMap(n) instead of myMap.next(n).
	@param n the next value to process.
	@return the input value mapped to the range which was set in the constructor.
	*/
	inline
	int operator()(int n) {
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

