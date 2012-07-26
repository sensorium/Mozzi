/*
 * LowPass1stOrder.h
 *
 * Copyright 2012 Tim Barrass
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

/*
 * Adapted from a six-zero low-pass filter implementation of H(z) = ( 1 - z^-20) / (1 - z^-1)
 * Tompkins, W. J. (1993) Biomedical Digital Signal Processing: C Language Examples And Laboratory Experiments for the IBM PC, p.1209
 *
*/


#ifndef LOWPASS1STORDER_H_
#define LOWPASS1STORDER_H_


/** Low-pass filter suited for smoothing control signals. This is an alternatve to
Mozzi's Smooth class, with the advantage that it can handle long numbers.
However, the drawback is that it imparts gain so the output can be an order of magnitude larger than its input.
This renders it unsuitable for some uses, such as controlling frequencies where harmonic results are required.
@todo Find out if there is a way to calculate and predict the output range for a particular input range and filter length,
whether it can be used precisely or requires experimenting to find useful values in each application.
Specialise templates for unsigned types.
*/
template <class T, unsigned char LENGTH>
class LowPass1stOrder
{

public:

	/** Constructor.
	@tparam T the type of numbers being smoothed (int, long, etc.)  This version only works with signed types.
	@tparam LENGTH the length of the feedback delay used in the filter. How much smoothing
	to apply. How many times callis of next() for the output to catch up with the input.
	*/
	LowPass1stOrder()
	{
		x_delay = &x[0];
	}

	/** Calculate the next sample, given an input signal.
	@param in the signal input.
	@return the signal output.  This is likely to be surprisingly large, so you need to check it and adapt your program to suit.
	@note: Timing 3-4 us
	*/
	inline
	T next(T x_current)
	{
		y += (T)(x_current - *x_delay);		/* y(nT)=y(nT-T)+x(nT)-x(nT-20T) */
		*x_delay = x_current;			/* x_current becomes x(nT-T) in FIFO*/
		x_delay = (x_delay == &x[LENGTH-1]) ? &x[0] : ++x_delay; /* increment pointer x_delay along the x array and wrap*/
		return(y);
	}


private:
	T x[LENGTH]; 		/* FIFO buffer of past samples */
	T y;				/* serves as both y(nT) and y(nT-T) */
	T *x_delay;	 	/* pointer to x(nT-20T)*/
};


#endif /* LOWPASS1STORDER_H_ */
