#ifndef OVERSAMPLE_H
#define OVERSAMPLE_H

/*
 * OverSample.h
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

 #include "RollingAverage.h"


/** @ingroup sensortools
 	Enables the resolution of analog inputs to be increased by oversampling and decimation.
 	Noise should be added to the input before it's digitised, then a succession of input readings are summed and 
 	finally divided to give a number with greater resolution than the ADC.  
 	Often, noise in the Arduino system will be enough, but there are other practical methods described in 
 	[Enhancing ADC Resolution by Oversampling](http://www.atmel.com/images/doc8003.pdf‎),
 	as well as an explanation of the overall approach.
 	@tparam RESOLUTION_INCREASE_BITS how many extra bits of resolution to produce.
	The window length and the memory it needs increases quickly as the oversampling resolution increases.  
	1 bit = 4 unsigned ints (analog input between 0-1023) = 8 bytes,
	2 bits = 16 unsigned ints = 32 bytes,  
	3 bits = 64 unsigned ints = 128 bytes,
	More than 3 bits increase in resolution would require either using longs to store the readings,
	which would need 1024 bytes for a 4 bit increase and 4096 bytes for 5 bits (do any avr's have that much room?),
	or the average reading would have to be no more than 128 (for 4 bits increase), because 256 readings would be needed,
	and the sum of all 256 readings would have to fit into an int.  (32767 / 256 = 128).
	Changing OverSample to use unsigned ints could enable an average reading of 256, but isn't tested properly yet.
	@note The higher the resolution, the more lag there will be.
	It's almost a RollingAverage filter, with the difference that 
	OverSample doesn't divide by as much as you would for an average.
*/ 


template <const unsigned char RESOLUTION_INCREASE_BITS>
class OverSample: public RollingAverage<unsigned int, (1<<(RESOLUTION_INCREASE_BITS*2))>
{

public:
	using RollingAverage<unsigned int, (1<<(RESOLUTION_INCREASE_BITS*2))>::add;

	/** Oversample and decimate the input to increase resolution by RESOLUTION_INCREASE_BITS;
	@param input an analog input to oversample.
	@return the higher resolution result.
	@note timing 5.7us
	*/
	unsigned int next(unsigned int input)
	{
		return (unsigned)add(input)>>RESOLUTION_INCREASE_BITS;
	}

};


/**
@example 05.Control_Filters/Thermistor_OverSample/Thermistor_OverSample.ino
This is an example demonstrating the OverSample class.
*/

#endif        //  #ifndef OVERSAMPLE_H
