#ifndef ROLLINGAVERAGE_H
#define ROLLINGAVERAGE_H

/*
 * RollingAverage.h
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
/*
  Draws on Arduino Smoothing example,
  Created 22 April 2007
  By David A. Mellis  <dam@mellis.org>
  modified 9 Apr 2012
  by Tom Igoe
  http://www.arduino.cc/en/Tutorial/Smoothing
*/

#include "mozzi_utils.h" // for trailingZeros()


/** @ingroup sensortools
  Calculates a running average over a 
  specified number of the most recent readings.  
  Like Smooth(), this is good for smoothing analog inputs in updateControl().
  @tparam WINDOW_LENGTH the number of readings to include in the rolling average.
   It must be a power of two (unless you're averaging floats). The higher the
   number, the more the readings will be smoothed, but the slower the output
   will respond to the input.
*/ 

template <class T, int WINDOW_LENGTH>
class
	RollingAverage {

public:

	/** Constructor.
	@tparam T the type of numbers to average, eg. int, unsigned int, float etc.  It will be relatively slow with
	floating point numbers, as it will use a divide operation for the averaging.
	Nevertheless, there might be a time when it's useful.
	@tparam WINDOW_LENGTH the number of readings to keep track of. It must be a power of two (unless
	you're averaging floats). The higher the number, the more the readings will be
	smoothed, but the slower the output will respond to the input.
	@note Watch out for overflows!
	*/
	RollingAverage():index(0),total(0), WINDOW_LENGTH_AS_RSHIFT((unsigned char)trailingZeros((unsigned long)WINDOW_LENGTH))
	{
		// initialize all the readings to 0:
		for (int thisReading = 0; thisReading < WINDOW_LENGTH; thisReading++)
			readings[thisReading] = 0;
	}

		
	/** Give the average of the last WINDOW_LENGTH.
	@param input a control signal such as an analog input which needs smoothing.
	@return the smoothed result.
	@note unsigned int timing 5.7us
	*/
	T next(T input)
	{
		return add(input)>>WINDOW_LENGTH_AS_RSHIFT;
	}


protected:
	
	inline 
	T add(T input){
		// out with the old
		total -= readings[index];
		// in with the new
		total +=  input;
		readings[index] = input;

		// advance and wrap index
		++index &= WINDOW_LENGTH -1;
		return total;
	}
		
	
private:
	T readings[WINDOW_LENGTH];	// the readings from the analog input
	unsigned int index;	// the index of the current reading
	long total;	// the running total
	const unsigned char WINDOW_LENGTH_AS_RSHIFT;
	
};

// no need to show the specialisations
/** @cond  */

/** unsigned int partial specialisation of RollingAverage template.
This is needed because unsigned types need to remind (unsigned) for rshift.
*/
template <int WINDOW_LENGTH>
class RollingAverage <unsigned int, WINDOW_LENGTH>
{
public:
	/** Constructor.
	@tparam WINDOW_LENGTH A power of two, the number of readings to keep track of.    
	The higher the number, the more the readings will be smoothed, but the slower the output will
	respond to the input. 
	@note The internal total of all the values being averaged is held in a long (4 byte) integer, to avoid overflowing.
	However, watch out for overflows if you are averaging a long number types!
	*/
	RollingAverage():index(0),total(0), WINDOW_LENGTH_AS_RSHIFT((unsigned char)trailingZeros((unsigned long)WINDOW_LENGTH))
	{
		// initialize all the readings to 0:
		for (int thisReading = 0; thisReading < WINDOW_LENGTH; thisReading++)
			readings[thisReading] = 0;
	}

	/** Give the average of the last WINDOW_LENGTH.
	@param a control signal such as an analog input which needs smoothing.
	@return the smoothed result.
	@note timing for int 6us
	*/
	unsigned int next(unsigned int input)
	{
		// calculate the average:
		// this unsigned cast is the only difference between the int and unsigned int specialisations
		// it tells the shift not to sign extend in from the left
		return (unsigned)add(input)>>WINDOW_LENGTH_AS_RSHIFT;
	}

protected:
	
	
	inline 
	unsigned int  add(unsigned int input){
		// out with the old
		total -= readings[index];
		// in with the new
		total +=  input;
		readings[index] = input;

		// advance and wrap index
		++index &= WINDOW_LENGTH -1;
		return total;
	}
	
	
private:
	unsigned int readings[WINDOW_LENGTH];      // the readings from the analog input
	unsigned int index;                  // the index of the current reading
	long total;               // the running total
	const unsigned char WINDOW_LENGTH_AS_RSHIFT;

};



/** float partial specialisation of RollingAverage template*/
template <int WINDOW_LENGTH>
class RollingAverage <float, WINDOW_LENGTH>
{
public:
	/** Constructor.
	@tparam WINDOW_LENGTH A power of two, the number of readings to keep track of.    
	The higher the number, the more the readings will be smoothed, but the slower the output will
	respond to the input. 
	@note The internal total of all the values being averaged is held in a long (4 byte) integer, to avoid overflowing.
	However, watch out for overflows if you are averaging a long number types!
	*/
	RollingAverage():index(0),total(0.0)
	{
		// initialize all the readings to 0:
		for (int thisReading = 0; thisReading < WINDOW_LENGTH; thisReading++)
			readings[thisReading] = 0.0;
	}

	/** Give the average of the last WINDOW_LENGTH.
	@param a control signal such as an analog input which needs smoothing.
	@return the smoothed result.
	@note timing for float 37us
	*/
	float next(float input)
	{
		// out with the old
		total -= readings[index];
		// in with the new
		total +=  input;
		readings[index] = input;

		// advance and wrap index
		++index &= WINDOW_LENGTH -1;

		// calculate the average:
		return total/WINDOW_LENGTH;
	}

private:
	float readings[WINDOW_LENGTH];      // the readings from the analog input
	unsigned int index;                  // the index of the current reading
	float total;               // the running total
	
};


// no need to show the specialisations 
/** @endcond 
*/

/**
@example 03.Sensors/Knob_LDR_x2_WavePacket/Knob_LDR_x2_WavePacket.ino
This example demonstrates the RollingAverage class.
*/

#endif        //  #ifndef ROLLINGAVERAGE_H
