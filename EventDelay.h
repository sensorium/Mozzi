/*
 * EventDelay.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2012-2024 Tim Barrass and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
 */

#ifndef EVENTDELAY_H_
#define EVENTDELAY_H_


/** A non-blocking replacement for Arduino's delay() function. 
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
	EventDelay(unsigned int delay_milliseconds = 0): AUDIO_TICKS_PER_MILLISECOND((float)MOZZI_AUDIO_RATE/1000.0f)
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
