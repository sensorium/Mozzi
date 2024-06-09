/*
 * AutoRange.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2013-2024 Tim Barrass and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
 */

#ifndef AUTORANGE_H
#define AUTORANGE_H

/** @ingroup sensortools
Keeps a running calculation of the range of the input values it receives.
*/
template <class T>
class
	AutoRange {

public:
	/** Constructor.
	@tparam T the type of numbers to to use, eg. int, unsigned int, float etc.
	@param min_expected the minimum possible input value.
	@param max_expected the maximum possible input value.
	*/
	AutoRange(T min_expected, T max_expected):range_min(max_expected),range_max(min_expected),range(0)
	{
	}


	/** Updates the current range.
	@param n the next value to include in the range calculation.
	*/
	void next(T n)
	{
		if (n > range_max)
		{
			range_max = n;
			range = range_max - range_min;
		}
		else
		{
			if (n< range_min)
			{
				range_min = n;
				range = range_max - range_min;
			}
		}
	}

	/** Returns the current minimum.
	@return minimum
	*/
	T getMin()
	{
		return range_min;
	}


	/** Returns the current maximum.
	@return maximum
	*/
	T getMax()
	{
		return range_max;
	}


	/** Returns the current range.
	@return range
	*/
	T getRange()
	{
		return range;
	}

private:
	T range_max, range_min, range;

};

#endif        //  #ifndef AUTORANGE_H
