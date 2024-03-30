/*
 * CapPoll.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2015-2024 Tim Barrass and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
 */

#ifndef RCPOLL_H
#define RCPOLL_H


/**
A class for reading voltage on a digital pin, derived from http://arduino.cc/en/Tutorial/RCtime.
This is designed to be used in updateControl().  Each time it is called, it checks if a capacitor has charged,
and returns an output reflecting how long it took for the most recent charge.
*/

template <unsigned char SENSOR_PIN, unsigned char SEND_PIN>
class CapPoll
{

public:
	/** Constructor.
	*/
	CapPoll():result(0),rc_cued(true), output(0)
	{
		;
	}

	/** Checks whether the capacitor has charged, and returns how long it took for the most recent charge.
	This would preferably be called in updateControl(), but if the resolution isn't fine enough or the 
	pin charges too fast for updateControl() to catch, try it in updateAudio().
	@return the sensor value, reflected in how many checking cycles it took to charge the capacitor.
	 */
	inline
	unsigned int next(){
		if (rc_cued){
			pinMode(SENSOR_PIN, INPUT);        // turn pin into an input and time till pin goes low
			digitalWrite(SENSOR_PIN, LOW);     // turn pullups off - or it won't work
			rc_cued = false;
		}
		if(digitalRead(SENSOR_PIN)){    // wait for pin to go low
			result++;
		}
		else{
			output = result;
			result = 0;
			pinMode(SENSOR_PIN, OUTPUT);  // make pin OUTPUT
			digitalWrite(SENSOR_PIN, HIGH);    // make pin HIGH to discharge capacitor - see the schematic
			rc_cued = true;
		}
		return output;
	}

private:
	unsigned int result;
	boolean rc_cued;
	unsigned int output;

};

#endif        //  #ifndef RCPOLL_H

