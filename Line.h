/*
 * Line.h
 *
 * Copyright 2012 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */

#ifndef LINE_H_
#define LINE_H_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include <util/atomic.h>

/** For linear changes with a minimum of calculation at each step. For instance,
you can use Line to make an oscillator glide from one frequency to another,
pre-calculating the required phase increments for each end and then letting your
Line change the phase increment with only a simple addition at each step.
@tparam T the type of numbers to use. For example, Line \<int\> myline; makes a
Line which uses ints. 
@note Watch out for underflows in the internal calcualtion of Line() if you're not
using floats (but on the other hand try to avoid lots of floats, they're too slow!). 
If it seems like the Line() is not working, there's a good chance you need to
scale up the numbers you're using, so internal calculations don't get truncated
away. Use Mozzi's fixed-point number types in mozzi_fixmath.h, which enable you to
represent fractional numbers. Google "fixed point arithmetic" if this is new to
you.
*/

template <class T>
class Line
{
private:
	volatile T current_value; // volatile because it could be set in control interrupt and updated in audio
	volatile T step_size;

public:
	/** Constructor. Use the template parameter to set the type of numbers you
	want to use. For example, Line \<int\> myline; makes a Line which uses ints.
	 */
	Line ()
	{
		;
	}

	
		
	/** Increments one step along the line.
	@return the next value.
	 */
	inline
	T next()
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			current_value += step_size;
		}
		//Serial.println(current_value);
		return current_value;
	}



	/** Set the current value of the line. 
	The Line will continue incrementing from this
	value using any previously calculated step size.
	@param value the number to set the Line's current_value to.
	 */
	inline
	void set(T value)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			current_value=value;
		}
	}



	/** Given a target value and the number of steps to take on the way, this calculates the step size needed to get there from the current value.
	@param targetvalue the value to move towards.
	@param num_steps how many steps to take to reach the target.
	 */
	inline
	void set(T targetvalue, T num_steps)
	{
		T numerator;
//		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
//		{
		 numerator = targetvalue-current_value;
//		}
		//float step = (float)numerator/num_steps;
			T step = numerator/num_steps;
//		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
//		{
			step_size= (T)step;
//		}
		//Serial.print("numerator");Serial.print(" \t");Serial.println(numerator);
		//Serial.print("num_steps");Serial.print(" \t");Serial.println(num_steps);
		//Serial.print(step);Serial.print(" \t");Serial.println(step_size);
		//step_size=(T)((((float)targetvalue-current_value)/num_steps));

	}

	/** Given a new starting value, target value and the number of steps to take on the way, this sets the step size needed to get there.
	@param startvalue the number to set the Line's current_value to.
	@param targetvalue the value to move towards.
	@param num_steps how many steps to take to reach the target.
	 */
	inline
	void set(T startvalue, T targetvalue, T num_steps)
	{
		set(startvalue);
		set(targetvalue, num_steps);
	}
};


/* unsigned char specialisation (probably not very useful because step size will likely = 0) */
template <>
class Line <unsigned char>
{
private:
	volatile unsigned char current_value; // volatile because it could be set in control interrupt and updated in audio
	char step_size;

public:
	/** Constructor. Use the template parameter to set the type of numbers you
	want to use. For example, Line \<int\> myline; makes a Line which uses ints.
	 */
	Line ()
	{
		;
	}

	
		
	/** Increments one step along the line.
	@return the next value.
	 */
	inline
	unsigned char next()
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			current_value += step_size;
		}
		return current_value;
	}



	/** Set the current value of the line. 
	The Line will continue incrementing from this
	value using any previously calculated step size.
	@param value the number to set the Line's current_value to.
	 */
	inline
	void set(unsigned char value)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			current_value=value;
		}
	}



	/** Given a target value and the number of steps to take on the way, this calculates the step size needed to get there from the current value.
	@param targetvalue the value to move towards.
	@param num_steps how many steps to take to reach the target.
	 */
	inline
	void set(unsigned char targetvalue, unsigned char num_steps)
	{
		step_size=(char)((((float)targetvalue-current_value)/num_steps));
	}

	/** Given a new starting value, target value and the number of steps to take on the way, this sets the step size needed to get there.
	@param startvalue the number to set the Line's current_value to.
	@param targetvalue the value to move towards.
	@param num_steps how many steps to take to reach the target.
	 */
	inline
	void set(unsigned char startvalue, unsigned char targetvalue, unsigned char num_steps)
	{
		set(startvalue);
		set(targetvalue, num_steps);
	}
	
};
	
	
/* unsigned int specialisation */
template <>
class Line <unsigned int>
{
private:
	volatile unsigned int current_value; // volatile because it could be set in control interrupt and updated in audio
	int step_size;

public:
	/** Constructor. Use the template parameter to set the type of numbers you
	want to use. For example, Line \<int\> myline; makes a Line which uses ints.
	 */
	Line ()
	{
		;
	}

	
		
	/** Increments one step along the line.
	@return the next value.
	 */
	inline
	unsigned int next()
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			current_value += step_size;
		}
		return current_value;
	}



	/** Set the current value of the line. 
	The Line will continue incrementing from this
	value using any previously calculated step size.
	@param value the number to set the Line's current_value to.
	 */
	inline
	void set(unsigned int value)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			current_value=value;
		}
	}



	/** Given a target value and the number of steps to take on the way, this calculates the step size needed to get there from the current value.
	@param targetvalue the value to move towards.
	@param num_steps how many steps to take to reach the target.
	 */
	inline
	void set(unsigned int targetvalue, unsigned int num_steps)
	{
		step_size=(int)((((float)targetvalue-current_value)/num_steps));
	}

	
	/** Given a new starting value, target value and the number of steps to take on the way, this sets the step size needed to get there.
	@param startvalue the number to set the Line's current_value to.
	@param targetvalue the value to move towards.
	@param num_steps how many steps to take to reach the target.
	 */
	inline
	void set(unsigned int startvalue, unsigned int targetvalue, unsigned int num_steps)
	{
		set(startvalue);
		set(targetvalue, num_steps);
	}
};





/* unsigned long specialisation */
template <>
class Line <unsigned long>
{
private:
	volatile unsigned long current_value; // volatile because it could be set in control interrupt and updated in audio
	long step_size;

public:
	/** Constructor. Use the template parameter to set the type of numbers you
	want to use. For example, Line \<int\> myline; makes a Line which uses ints.
	 */
	Line ()
	{
		;
	}

	
		
	/** Increments one step along the line.
	@return the next value.
	 */
	inline
	unsigned long next()
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			current_value += step_size;
		}
		return current_value;
	}



	/** Set the current value of the line. 
	The Line will continue incrementing from this
	value using any previously calculated step size.
	@param value the number to set the Line's current_value to.
	 */
	inline
	void set(unsigned long value)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			current_value=value;
		}
	}



	/** Given a target value and the number of steps to take on the way, this calculates the step size needed to get there from the current value.
	@param targetvalue the value to move towards.
	@param num_steps how many steps to take to reach the target.
	 */
	inline
	void set(unsigned long targetvalue, unsigned long num_steps)
	{
		step_size=(long)((((float)targetvalue-current_value)/num_steps));
	}

	/** Given a new starting value, target value and the number of steps to take on the way, this sets the step size needed to get there.
	@param startvalue the number to set the Line's current_value to.
	@param targetvalue the value to move towards.
	@param num_steps how many steps to take to reach the target.
	 */
	inline
	void set(unsigned long startvalue, unsigned long targetvalue, unsigned long num_steps)
	{
		set(startvalue);
		set(targetvalue, num_steps);
	}
};

/**
@example 02.Control/Control_Tremelo/Control_Tremelo.ino
This example demonstrates the Line class.
*/

#endif /* LINE_H_ */
