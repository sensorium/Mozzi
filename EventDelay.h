/*
 * EventDelay.h
 *
 * Copyright 2012 Tim Barrass.
 *
 * This file is part of Cuttlefish.
 *
 * Cuttlefish is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cuttlefish is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cuttlefish.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef EVENTDELAY_H_
#define EVENTDELAY_H_

#include "Arduino.h"

/** A non-blocking replacement for Arduino's delay() function (which is disabled by Cuttlefish). EventDelay can be
set() to wait for a number of milliseconds, then after calling start(), calling ready() will return true when the time is up.*/

class EventDelay
{

public:
	/** Constructor.
	@param update_rate is how frequently you'll check if the EventDelay is ready().
	This would be CONTROL_RATE if ready() is used simply in updateControl().
	*/
	EventDelay(unsigned int update_rate);


	/** Set the delay time.  This setting is persistent, until you change it by using set() again.
	@param delay_milliseconds delay time in milliseconds.
	*/
	inline
	void set(unsigned int delay_milliseconds)
	{
		counter_start_value = ((long)delay_milliseconds*1000)/micros_per_update;
	}


	/** Start the delay.
	*/
	inline
	void start()
	{
		counter = counter_start_value;
	}


	/** Call this in updateControl() or updateAudio() to check if the delay time is up.
	@return true if the time is up.
	*/
	inline
	boolean ready()
	{
		return --counter<0;
	}


private:

	long counter; // long so even at a control rate of 2048 you can have >15seconds
	long counter_start_value;
	const unsigned int micros_per_update;

};


#endif /* EVENTDELAY_H_ */