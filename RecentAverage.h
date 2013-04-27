#ifndef RECENTAVERAGE_H
#define RECENTAVERAGE_H

/*
 * RecentAverage.h
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



/**
  RecentAverage reads repeatedly from an input, calculating a running average over a 
  specified number of the most recent readings.  
  Like Smooth(), this is good for smoothing analog inputs in updateControl().
*/
template <class T, int NUM_READINGS>
class
	RecentAverage {

public:
	/** Constructor.
	@tparam T the type of numbers to average, eg. int, unsigned int, float etc.  It will be relatively slow with
	floating point numbers, as it will use a divide operation for the averaging.
	Nevertheless, there might be a time when it's useful.
	@tparam NUM_READINGS the number of readings to keep track of. It must be a power of two (unless
	you're averaging floats). The higher the number, the more the readings will be
	smoothed, but the slower the output will respond to the input.
	@note Watch out for overflows if you are averaging a long number types!
	*/
	RecentAverage():index(0),total(0)
	{
		// initialize all the readings to 0:
		for (int thisReading = 0; thisReading < NUM_READINGS; thisReading++)
			readings[thisReading] = 0;
		switch(NUM_READINGS)
		{
		case 256:
			num_readings_as_rshift = 8;
			break;
		case 128:
			num_readings_as_rshift = 7;
			break;
		case 64:
			num_readings_as_rshift = 6;
			break;
		case 32:
			num_readings_as_rshift = 5;
			break;
		case 16:
			num_readings_as_rshift = 4;
			break;
		case 8:
			num_readings_as_rshift = 3;
			break;
		case 4:
			num_readings_as_rshift = 2;
			break;
		case 2:
			num_readings_as_rshift = 1;
			break;
			//default:
			// if nothing else matches, do the default
			// default is optional
		}
	}



	/** Give the average of the last NUM_READINGS.
	@param a control signal such as an analog input which needs smoothing.
	@return the smoothed result.
	*/
	T next(T input)
	{
		// out with the old
		total -= readings[index];
		// in with the new
		total +=  input;
		readings[index] = input;

		// advance and wrap index
		++index &= NUM_READINGS -1;

		// calculate the average:
		return total>>num_readings_as_rshift;
	}


private:
	T readings[NUM_READINGS];	// the readings from the analog input
	unsigned char index;	// the index of the current reading
	long total;	// the running total
	unsigned char num_readings_as_rshift;

};




/** @cond */ // doxygen can ignore the specialisations


// not sure why this template is neccessary now..

/** unsigned int partial specialisation of RecentAverage template*/
template <int NUM_READINGS>
class RecentAverage <unsigned int, NUM_READINGS>
{
public:
	/** Constructor.
	@tparam NUM_READINGS A power of two, the number of readings to keep track of.    
	The higher the number, the more the readings will be smoothed, but the slower the output will
	respond to the input. 
	@note The internal total of all the values being averaged is held in a long (4 byte) integer, to avoid overflowing.
	However, watch out for overflows if you are averaging a long number types!
	*/
	RecentAverage():index(0),total(0)
	{
		// initialize all the readings to 0:
		for (int thisReading = 0; thisReading < NUM_READINGS; thisReading++)
			readings[thisReading] = 0;
		switch(NUM_READINGS)
		{
		case 256:
			num_readings_as_rshift = 8;
			break;
		case 128:
			num_readings_as_rshift = 7;
			break;
		case 64:
			num_readings_as_rshift = 6;
			break;
		case 32:
			num_readings_as_rshift = 5;
			break;
		case 16:
			num_readings_as_rshift = 4;
			break;
		case 8:
			num_readings_as_rshift = 3;
			break;
		case 4:
			num_readings_as_rshift = 2;
			break;
		case 2:
			num_readings_as_rshift = 1;
			break;
			//default:
			// if nothing else matches, do the default
			// default is optional
		}
	}

	/** Give the average of the last NUM_READINGS.
	@param a control signal such as an analog input which needs smoothing.
	@return the smoothed result.
	*/
	unsigned int next(unsigned int input)
	{
		// out with the old
		total -= readings[index];
		// in with the new
		total +=  input;
		readings[index] = input;

		// advance and wrap index
		++index &= NUM_READINGS -1;

		// calculate the average:
		return (unsigned) total>>num_readings_as_rshift;
	}


private:
	unsigned int readings[NUM_READINGS];      // the readings from the analog input
	unsigned char index;                  // the index of the current reading
	long total;               // the running total
	unsigned char num_readings_as_rshift;

};




/** float partial specialisation of RecentAverage template*/
template <int NUM_READINGS>
class RecentAverage <float, NUM_READINGS>
{
public:
	/** Constructor.
	@tparam NUM_READINGS A power of two, the number of readings to keep track of.    
	The higher the number, the more the readings will be smoothed, but the slower the output will
	respond to the input. 
	@note The internal total of all the values being averaged is held in a long (4 byte) integer, to avoid overflowing.
	However, watch out for overflows if you are averaging a long number types!
	*/
	RecentAverage():index(0),total(0)
	{
		// initialize all the readings to 0:
		for (int thisReading = 0; thisReading < NUM_READINGS; thisReading++)
			readings[thisReading] = 0;
	}

	/** Give the average of the last NUM_READINGS.
	@param a control signal such as an analog input which needs smoothing.
	@return the smoothed result.
	*/
	unsigned int next(unsigned int input)
	{
		// out with the old
		total -= readings[index];
		// in with the new
		total +=  input;
		readings[index] = input;

		// advance and wrap index
		++index &= NUM_READINGS -1;

		// calculate the average:
		// this unsigned cast is the only difference between the int and unsigned int specialisations
		// it tells the shift not to sign extend in from the left
		return total/NUM_READINGS;
	}


private:
	float readings[NUM_READINGS];      // the readings from the analog input
	unsigned char index;                  // the index of the current reading
	float total;               // the running total

};


#endif        //  #ifndef RECENTAVERAGE_H
