#ifndef AUTORANGE_H
#define AUTORANGE_H

/*
 * AutoRange.h
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


/**
Keeps a running calculation of the range of the input values it receives.
*/
template <class T>
class
	AutoRange {

public:
	/** Constructor.
	@tparam T the type of numbers to to use, eg. int, unsigned int, float etc.
	*/
	AutoRange(T min_expected, T max_expected):range_min(max_expected),range_max(min_expected),range(0)
	{}


	/** Updates and returns the current range.
	@param n the next value to include in the range.
	*/
T next(T n){
  if (n > range_max) {
    range_max = n;
    range = range_max - range_min;
  }
  }else{
    if (n< range_min) {
      range_min = n;
      range = range_max - range_min;
    }
  }
  return range;
}

private:
T range_max, range_min , range;

};

#endif        //  #ifndef AUTORANGE_H

