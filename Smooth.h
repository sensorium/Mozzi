/*
 * Smooth.h
 *
 * Copyright 2012 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */

#ifndef SMOOTH_H_
#define SMOOTH_H_

#include "Arduino.h"
#include "mozzi_fixmath.h"

/** A simple infinite impulse response low pass filter for smoothing control or audio signals.
This algorithm comes from http://en.wikipedia.org/wiki/Low-pass_filter:
y[i] := y[i-1] + α * (x[i] - y[i-1]),
translated as
out = last_out + a * (in - last_out).
It's not calibrated to any real-world update rate, so if you use it at
CONTROL_RATE and you change CONTROL_RATE, you'll need to adjust the smoothness
value to suit.
@tparam T the type of numbers being smoothed.  Watch out for numbers overflowing the
internal calculations. Some experimentation is recommended.
@note Timing: ~5us for 16 bit types, ~1us for 8 bit types.
@todo Check if 8 bit templates can work efficiently with a higher res smoothness -
	as is they don't have enough resolution to work well at audio rate.  See if Line might be
	more useful in most cases.
*/

template <class T>
class Smooth
{
private:
	long last_out;
	Q0n16 a;

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

	/** Constructor.
	This constructor which doesn't take a smoothness parameter is useful when you incorporate Smooth into another class definition.
	You need to call setSmoothness(float) for your object before using Smooth.
	@note there's probably a better way to do this...
	 */
	Smooth()
	{}


	/** Filters the input and returns the filtered value.  You can also use the operator() function, eg. something like mySmoother(input-value).
	@param in the signal to be smoothed.
	@return the filtered signal.
	 */
	inline
	T next(T in)
	{
		long out = ((((((long)in - (last_out>>8)) * a))>>8) + last_out);
		last_out = out;
		return (T)(out>>8);
	}


	/** Filters the input and returns the filtered value.  Same as next(input-value).
	@param in the signal to be smoothed.
	@return the filtered signal.
	 */
	inline
	T operator()(T n) {
		return next(n);
	}


	/** Sets how much smoothing the filter will apply to its input.
	@param smoothness sets how much smoothing the filter will apply to
	its input. Use a float in the range 0~1, where 0 is not very smooth and 0.99 is
	very smooth.
	 */
	inline
	void setSmoothness(float smoothness)
	{
		a=float_to_Q0n16(1.f-smoothness);
	}

};


/** @cond */ // doxygen can ignore the specialisations

/** uint8_t specialisation of Smooth template*/
template <>
class Smooth <uint8_t>
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

	/** Filters the input and returns the filtered value.  You can also use the operator() function, eg. something like mySmoother(input-value).
	@param in the signal to be smoothed.
	@return the filtered signal.
	 */
	inline
	uint8_t next(uint8_t in)
	{
		unsigned int out = (((((int)in - (last_out>>8)) * a)) + last_out);
		last_out = out;
		return (uint8_t)(out>>8);
	}


	/** Filters the input and returns the filtered value.  Same as next(input-value).
	@param in the signal to be smoothed.
	@return the filtered signal.
	 */
	inline
	uint8_t  operator()(uint8_t  n) {
		return next(n);
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


/** int8_t specialisation of Smooth template*/
template <>
class Smooth <int8_t>
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


	/** Filters the input and returns the filtered value.  You can also use the operator() function, eg. something like mySmoother(input-value).
	@param in the signal to be smoothed.
	@return the filtered signal.
	 */
	inline
	int8_t next(int8_t in)
	{
		int out = (((((int)in - (last_out>>8)) * a)) + last_out);
		last_out = out;
		return (int8_t)(out>>8);
	}


	/** Filters the input and returns the filtered value.  Same as next(input-value).
	@param in the signal to be smoothed.
	@return the filtered signal.
	 */
	inline
	int8_t operator()(int8_t n) {
		return next(n);
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

/** float specialisation of Smooth template*/
template <>
class Smooth <float>
{
private:
	float last_out;
	float a;

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

	/** Filters the input and returns the filtered value.  You can also use the operator() function, eg. something like mySmoother(input-value).
	@param in the signal to be smoothed.
	@return the filtered signal.
	 */
	inline
	float next(float in)
	{
		float out = last_out + a * (in - last_out);
		//float out = (in - last_out * a) + last_out;
		last_out = out;
		return out;
	}


	/** Filters the input and returns the filtered value.  Same as next(input-value).
	@param in the signal to be smoothed.
	@return the filtered signal.
	 */
	inline
	float operator()(float n) {
		return next(n);
	}


	/** Sets how much smoothing the filter will apply to its input.
	@param smoothness sets how much smoothing the filter will apply to
	its input. Use a float in the range 0~1, where 0 is not very smooth and 0.99 is
	very smooth.
	 */
	inline
	void setSmoothness(float smoothness)
	{
		a=1.f-smoothness;
	}

};


/** @endcond */

/**
@example 05.Control_Filters/Smooth/Smooth.ino
This example demonstrates the Smooth class.
*/

#endif /* SMOOTH_H_ */
