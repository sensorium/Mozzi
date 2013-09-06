/*
 * RollingAutoRange.h
 *
 * Copyright 2013 Tim Barrass.
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
#ifndef ROLLINGAUTORANGE_H
#define ROLLINGAUTORANGE_H

#include "AutoRange.h"
#include "DCFilter.h" // to provide recent upper and lower limits to AutoRange

/** @ingroup sensortools
Keeps a running calculation of the range of recent input values it receives.
*/
//template <class T>
class
	RollingAutoRange {

public:

	/** Constructor.
	@tparam T the type of numbers to to use, eg. int, unsigned int, float etc.
	@param min_expected the minimum possible input value.
	@param max_expected the maximum possible input value.
	*/
	RollingAutoRange(unsigned int min_expected, unsigned int max_expected, float smoothness): filterMax(pole),filterMin(pole),smoothAutoRange(min_expected, max_expected)
	{
	}


	/** Updates the current range.
	@param n the next value to include in the range calculation.
	*/
	void next(int n){
		range = max-min;
		centre = (range/2)+min;
		relative_max = max-centre;
		relative_min = -(centre-min);
		max = centre + filterMax.next(relative_max);
		min = centre + filterMin.next(relative_min);
	}
	
	
private:
	//T range_max, range_min , range;
	DCfilter filterMax;
	DCfilter filterMin;
	AutoRange <unsigned int> recentAutoRange;

};

#endif        //  #ifndef ROLLINGAUTORANGE_H
