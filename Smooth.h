/*
 * Smooth.h
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

#ifndef SMOOTH_H_
#define SMOOTH_H_

#include "Arduino.h"
#include "fixedMath.h"

/** A simple low pass filter for smoothing control signals. This algorithm comes
from http://en.wikipedia.org/wiki/Low-pass_filter: y[i] := y[i-1] + α * (x[i] -
y[i-1]), translated as out = last_out + a * (in - last_out). It's not calibrated
to any real-world update rate, so if you use it at CONTROL_RATE and you change
CONTROL_RATE, you'll need to adjust the smoothness value to suit.
@tparam T the type of numbers being smoothed.  Watch out for numbers overflowing the
internal calculations. Some experimentation is recommended.
If this can't be overcome using different settings or values, LowPass1stOrder may be another option.
@note Timing: ~5us for 16 bit types, ~1us for 8 bit types.
*/

template <class T>
class Smooth
{
private:
	long last_out;
	Q0n8 a;

public:
	/** Constructor.
	@param smoothness sets how much smoothing the filter will apply to
	its input. Use a float in the range 0~1, where 0 is not very smooth and 0.99 is
	very smooth.
	 */
	Smooth(float smoothness)
	{
		setSmoothness(smoothness);
	}

	/** Filters the input and returns the filtered value.
	@param in the signal to be smoothed.
	@return the filtered signal.
	 */
	inline
	T next(T in)
	{
		long out = (((((long)in - (last_out>>8)) * a)) + last_out);
		last_out = out;
		return (T)(out>>8);
	}

	/** Sets how much smoothing the filter will apply to its input.
	@param smoothness sets how much smoothing the filter will apply to
	its input. Use a float in the range 0~1, where 0 is not very smooth and 0.99 is
	very smooth.
	 */
	inline
	void setSmoothness(float smoothness)
	{
		a=float_to_Q0n8(1.f-smoothness);
	}

};


/** unsigned char specialisation of Smooth template*/
// class template specialization:
template <>
class Smooth <unsigned char>
{
private:
	unsigned int last_out;
	Q0n8 a;

public:
	/** Constructor.
	@param smoothness sets how much smoothing the filter will apply to
	its input. Use a float in the range 0~1, where 0 is not very smooth and 0.99 is
	very smooth.
	 */
	Smooth(float smoothness)
	{
		setSmoothness(smoothness);
	}

	/** Filters the input and returns the filtered value.
	@param in the signal to be smoothed.
	@return the filtered signal.
	 */
	inline
	unsigned char next(unsigned char in)
	{
		unsigned int out = (((((int)in - (last_out>>8)) * a)) + last_out);
		last_out = out;
		return (unsigned char)(out>>8);
	}

	/** Sets how much smoothing the filter will apply to its input.
	@param smoothness sets how much smoothing the filter will apply to
	its input. Use a float in the range 0~1, where 0 is not very smooth and 0.99 is
	very smooth.
	 */
	inline
	void setSmoothness(float smoothness)
	{
		a=float_to_Q0n8(1.f-smoothness);
	}

};

/** char specialisation of Smooth template*/
// class template specialization:
template <>
class Smooth <char>
{
private:
	int last_out;
	Q0n8 a;

public:
	/** Constructor.
	@param smoothness sets how much smoothing the filter will apply to
	its input. Use a float in the range 0~1, where 0 is not very smooth and 0.99 is
	very smooth.
	 */
	Smooth(float smoothness)
	{
		setSmoothness(smoothness);
	}

	/** Filters the input and returns the filtered value.
	@param in the signal to be smoothed.
	@return the filtered signal.
	 */
	inline
	char next(char in)
	{
		int out = (((((int)in - (last_out>>8)) * a)) + last_out);
		last_out = out;
		return (char)(out>>8);
	}

	/** Sets how much smoothing the filter will apply to its input.
	@param smoothness sets how much smoothing the filter will apply to
	its input. Use a float in the range 0~1, where 0 is not very smooth and 0.99 is
	very smooth.
	 */
	inline
	void setSmoothness(float smoothness)
	{
		a=float_to_Q0n8(1.f-smoothness);
	}

};

#endif /* SMOOTH_H_ */