/*
 * Metronome.h
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

#ifndef METRO_H_
#define METRO_H_

#include "EventDelay.h"

/** A metronome class which is like an EventDelay which retriggers itself when the delay time is up, to produce a repeating beat.
Metronome can be set() to a number of milliseconds, then after calling start(), ready() will return true when the time is up.  
Alternatively, start(milliseconds) will call set() and start() together.
This is called Metronome to avoid conflict with the Arduino Metro library.
*/
class Metronome: public EventDelay
{

public:

	/** Constructor.
	Declare a Metronome object.
	@param delay_milliseconds how long between each occasion when ready() returns true.
	*/
	Metronome(unsigned int delay_milliseconds = 0): EventDelay(delay_milliseconds), stopped(false) {
	}

	
	/** Start the metronome.
	@todo have a parameter to set whether it's single or repeating, so start doesn't have to be called for repeats.  
	Pro: simpler user programming. Con: would require an if..then every time ready() is called.
	*/
	inline
	void start()
	{
		deadline=audioTicks()+ticks;
		stopped = false;
	}
	
	
	/** Set the time between beats and start the metronome.
	@param delay_milliseconds delay time in milliseconds.
	*/
	inline
	void start(unsigned int delay_milliseconds)
	{
		set(delay_milliseconds);
		start();
	}
	
	
	
		/** Set the beats per minute.
	@param  bpm beats per minute
	*/
	inline
	void setBPM(float bpm)
	{
		set((unsigned int) (60000.f/bpm));
	}
	
	

	
	/** Call this in updateControl() or updateAudio() to check if it is time for a beat.
	@return true if the time for one is up.
	*/
	inline
	bool ready()
	{
		unsigned long now = audioTicks();
		if ((now<deadline) || stopped) return false;

		deadline=now-(now-deadline)+ticks; // subtract overrun so the timing doesn't slip
		return true;
	}
	
	
	inline
	void stop(){
		stopped = true;
	}

private:
	bool stopped;
};




/**
@example 02.Control/Metronome_SampleHuffman/Metronome_SampleHuffman.ino
This example shows how to use the Metronome class.
*/

#endif /* METRO_H_ */
