/*
 * RollingStat.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2013-2024 Tim Barrass and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
 * WARNING: this code is incomplete, it doesn't work yet
 */

#ifndef ROLLINGSTAT_H
#define ROLLINGSTAT_H

#include "RollingAverage.h"
#include "mozzi_fixmath.h"

// adapted from RunningStat, http://www.johndcook.com/standard_deviation.html
/** @ingroup sensortools
WARNING: this class is work in progress, don't use it yet.
Calculates an approximation of the variance and standard deviation for a window of recent inputs.
@tparam T the type of numbers to use.  Choose unsigned int, int , uint8_t, int8_t, or float
@tparam WINDOW_LENGTH how many recent input values to include in the calculations.
*/
template <class T, int WINDOW_LENGTH>
class RollingStat
{
public:

	/** Constructor */
	RollingStat() : _previous_mean(0), _mean(0), _variance(0), WINDOW_LENGTH_AS_RSHIFT(trailingZerosConst(WINDOW_LENGTH))
	{}


	/** Update the mean and variance given a new input value.
	@param x the next input value
	@note timing for unsigned int 10us, int 22us
	*/
	void update(T x) {
		_mean = rollingMean.next(x);
		_variance = (((long)x - _previous_mean)*((long)x - _mean))>>WINDOW_LENGTH_AS_RSHIFT; // should really be div by (WINDOW_LENGTH-1), but exact values are not important for this application
		_previous_mean = _mean;
	}


	/** Update the mean and variance given a new input value.
	@param x the next input value
	*/
	void update(int8_t x) {
		_mean = rollingMean.next(x);
		_variance = (((int)x - _previous_mean)*((int)x - _mean))>>WINDOW_LENGTH_AS_RSHIFT; // should really be div by (WINDOW_LENGTH-1), but exact values are not important for this application
		_previous_mean = _mean;
	}


	/** Return the mean of the last WINDOW_LENGTH number of inputs.
	@return  mean
	*/
	T getMean() const {
		return _mean;
	}


	/** Return the approximate variance of the last WINDOW_LENGTH number of inputs.
	@return  variance
	@note This should really be calculated using WINDOW_LENGTH-1, but sacrificing accuracy for speed
	we use the power of two value of WINDOW_LENGTH.
	*/
	T getVariance() const {
		return _variance;
	}

	/** Return the approximate standard deviation of the last WINDOW_LENGTH number of inputs.
	@return  standard deviation.
	*/
	T getStandardDeviation() const {
		return isqrt16(_variance);
	}



private:
	T _previous_mean, _mean, _variance;
	RollingAverage <T, WINDOW_LENGTH> rollingMean;
	const uint8_t WINDOW_LENGTH_AS_RSHIFT;
};

// no need to show the specialisations
/** @cond  */

/** float specialisation of RollingStat template */
template <int WINDOW_LENGTH>
class RollingStat <float, WINDOW_LENGTH>
{
public:

	/** Constructor */
	RollingStat() : _previous_mean(0), _mean(0), _variance(0), WINDOW_LENGTH_AS_RSHIFT(trailingZerosConst(WINDOW_LENGTH))
	{}


	/** Update the mean and variance given a new input value.
	@param x the next input value
	@note timing for float: 60 to 90us
	*/
	void update(float x) {
		_mean = rollingMean.next(x);
		_variance = ((x - _previous_mean)*(x - _mean))/(WINDOW_LENGTH-1);
		_previous_mean = _mean;
	}


	/** Return the mean of the last WINDOW_LENGTH number of inputs.
	@return  mean
	*/
	float getMean() const {
		return _mean;
	}


	/** Return the approximate variance of the last WINDOW_LENGTH number of inputs.
	@return  variance
	*/
	float getVariance() const {
		return _variance;
	}

	/** Calculate and return the approximate standard deviation of the last WINDOW_LENGTH number of inputs.
	@return  standard deviation.
	@note this is probably too slow to use!
	*/
	float getStandardDeviation() const {
		return sqrt(_variance);
	}



private:
	float _previous_mean, _mean, _variance;
	RollingAverage <float, WINDOW_LENGTH> rollingMean;
	const uint8_t WINDOW_LENGTH_AS_RSHIFT;
};

// no need to show the specialisations
/** @endcond  */

#endif        //  #ifndef ROLLINGSTAT_H
