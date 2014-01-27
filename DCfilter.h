/*
 * DCfilter.h
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
 
#ifndef DCFILTER_H
#define DCFILTER_H

/*
tb2010 adapted from: 
robert bristow-johnson, DSP Trick: Fixed-Point DC Blocking Filter with Noise-Shaping
http://www.dspguru.com/book/export/html/126

y[n] = x[n] - x[n-1] + a * y[n-1]

Where y[n] is the output at the current time n, and x[n] is the input at the current time n. 

also, see DC Blocker Algorithms, http://www.ingelec.uns.edu.ar/pds2803/materiales/articulos/04472252.pdf
 */
 
/**
A DC-blocking filter useful for highlighting changes in control signals.
The output of the filter settles to 0 if the incoming signal stays constant.  If the input changes, the
filter output swings to track the change and eventually settles back to 0.
*/
class DCfilter {
public:
/** 
Instantiate a DC-blocking filter.
@param pole sets the responsiveness of the filter, 
how long it takes to settle to 0 if the input signal levels out at a constant value.
*/
	DCfilter(float pole):acc(0),prev_x(0),prev_y(0)
	{
		A = (int)(32768.0*(1.0 - pole));
	}
	
/*  almost original
	// timing: 20us
	int next(int x) 
	{
		setPin13High();
		acc   -= prev_x;
		prev_x = (long)x<<15;
		acc   += prev_x;
		acc   -= A*prev_y;
		prev_y = acc>>15;               // quantization happens here
		int filtered = (int)prev_y;
		// acc has y[n] in upper 17 bits and -e[n] in lower 15 bits
		setPin13Low();
		return filtered;
	}
	*/

	/** 
	Filter the incoming value and return the result.
	@param x the value to filter
	@return filtered signal
	*/
	// timing :8us
	inline
	int next(int x) 
	{
		acc += ((long)(x-prev_x)<<16)>>1;
		prev_x = x;
		acc   -= (long)A*prev_y; 	// acc has y[n] in upper 17 bits and -e[n] in lower 15 bits
		prev_y = (acc>>16)<<1; // faster than >>15 but loses bit 0	 
		if (acc & 32784) prev_y += 1; // adds 1 if it was in the 0 bit position lost in the shifts above  
		return prev_y;
	}

private:
	long acc;
	int prev_x, prev_y,A;
};

/**
@example 05.Control_Filters/DCFilter/DCFilter.ino
This example demonstrates the DCFilter class.
*/

#endif        //  #ifndef DCFILTER_H

