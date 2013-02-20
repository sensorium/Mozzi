/*
 * EventDelay.h
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

#ifndef EVENTDELAY_H_
#define EVENTDELAY_H_


/** A non-blocking replacement for Arduino's delay() function (which is disabled by Mozzi). EventDelay can be
set() to wait for a number of milliseconds, then after calling start(), calling ready() will return true when the time is up.
@tparam update_rate is how frequently you'll check if the EventDelay is ready().
This would be CONTROL_RATE if ready() is used simply in updateControl().
*/
template <unsigned int UPDATE_RATE>
class EventDelay
{

public:
	/** Constructor.
	Declare an EventDelay object with UPDATE_RATE template parameter.
	UPDATE_RATE is how frequently you'll check if the EventDelay is ready().
	This would be CONTROL_RATE if ready() is used simply in updateControl().
	*/
	EventDelay(): counter(0), micros_per_update(1000000/UPDATE_RATE)
	{
		;
	}	
	

	/** Set the delay time.  This setting is persistent, until you change it by using set() again.
	@param delay_milliseconds delay time in milliseconds.
	*/
	inline
	void set(unsigned int delay_milliseconds)
	{
		counter_start_value = ((long)delay_milliseconds*1000)/micros_per_update;
	}


	/** Start the delay.
	@todo have a parameter to set whether it's single or repeating, so start doesn't have to be called for repeats.  
	Pro: simpler user programming. Con: would require an if..then every time ready() is called.
	*/
	inline
	void start()
	{
		counter = counter_start_value;
	}
	
	
	/** Set the delay time and start the delay.
	@param delay_milliseconds delay time in milliseconds.
	*/
	inline
	void start(unsigned int delay_milliseconds)
	{
		set(delay_milliseconds);
		start();
	}


	/** Call this in updateControl() or updateAudio() to check if the delay time is up.
	@return true if the time is up.
	*/
	inline
	bool ready()
	{
		return (--counter<0);
	}


private:

	long counter; // long so even at a control rate of 2048 you can have >15seconds
	long counter_start_value;
	const unsigned int micros_per_update;

};


#endif /* EVENTDELAY_H_ */
