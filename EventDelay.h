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


/** A non-blocking replacement for Arduino's delay() function (which is disabled by Mozzi). 
EventDelay can be set() to a number of milliseconds, then after calling start(), ready() will return true when the time is up.  
Alternatively, start(milliseconds) will call set() and start() together.
*/
class EventDelay
{

public:
	
	/** Constructor.
	Declare an EventDelay object.
	@param delay_milliseconds how long until ready() returns true, after calling start().  Defaults to 0 if no parameter is supplied.
	*/
	EventDelay(unsigned int delay_milliseconds = 0): AUDIO_TICKS_PER_MILLISECOND((float)AUDIO_RATE/1000.0f)
	{
		set(delay_milliseconds);
	}	
	
	
	/** Set the delay time.  This setting is persistent, until you change it by using set() again.
	@param delay_milliseconds delay time in milliseconds.
	@note timing: 12us
	*/
	inline
	void set(unsigned int delay_milliseconds)
	{
		ticks = (unsigned long)(AUDIO_TICKS_PER_MILLISECOND*delay_milliseconds); // 12us
	}


	/** Start the delay.
	@todo have a parameter to set whether it's single or repeating, so start doesn't have to be called for repeats.  
	Pro: simpler user programming. Con: would require an if..then every time ready() is called.
	*/
	inline
	void start()
	{
		deadline=audioTicks()+ticks;
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
	@note timing: 1us.
	*/
	inline
	bool ready()
	{
		return(audioTicks()>=deadline); // 1us
	}


protected:
	// Metronome accesses these
	unsigned long deadline; 
	unsigned long ticks;
	
private:
	const float AUDIO_TICKS_PER_MILLISECOND; 
};

/**
@example 02.Control/EventDelay/EventDelay.ino
This example shows how to use the EventDelay class.
*/

#endif /* EVENTDELAY_H_ */
