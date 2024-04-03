/*
 * Line.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2012-2024 Tim Barrass and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
 */


#ifndef LINE_H_
#define LINE_H_

#include <Arduino.h>

#include<FixMath.h>

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
	T current_value;
	T step_size;

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
		current_value += step_size;
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
		current_value=value;
	}



	/** Given a target value and the number of steps to take on the way, this calculates the step size needed to get there from the current value.
	@param targetvalue the value to move towards.
	@param num_steps how many steps to take to reach the target.
	 */
	inline
	  void set(T targetvalue, T num_steps)
	{
	  if(num_steps) {
	    T numerator = targetvalue-current_value;
	    step_size= numerator/num_steps;
	  } else {
	    step_size = 0;
	    current_value = targetvalue;
	  }
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
	unsigned char current_value;
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
		current_value += step_size;
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
		current_value=value;
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
	unsigned int current_value;
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
		current_value += step_size;
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
		current_value=value;
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
	unsigned long current_value;
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
		current_value += step_size;
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
		current_value=value;
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


/* UFix specialisation */
template<int8_t NI, int8_t NF>
class Line<UFix<NI, NF>>
{
private:
  typedef UFix<NI, NF> internal_type;
  internal_type current_value;
  SFix<NI,NF> step_size;

public:
  /** Constructor. Use the template parameter to set the type of numbers you
      want to use. For example, Line \<int\> myline; makes a Line which uses ints.
  */
  Line (){;}

  /** Increments one step along the line.
      @return the next value.
  */
  inline
  internal_type next()
  {
    current_value = current_value + step_size;
    return current_value;
  }

  /** Set the current value of the line. 
      The Line will continue incrementing from this
      value using any previously calculated step size.
      @param value the number to set the Line's current_value to.
  */
  inline
  void set(internal_type value)
  {
    current_value=value;
  }

  /** Given a target value and the number of steps to take on the way, this calculates the step size needed to get there from the current value.
      @param targetvalue the value to move towards.
      @param num_steps how many steps to take to reach the target as a UFix<_NI,0>
  */
  template<int8_t _NI>
  void set(internal_type targetvalue, UFix<_NI,0> num_steps)
  {
    if(num_steps.asRaw()) {
      auto numerator = targetvalue-current_value;
      step_size = numerator*num_steps.invAccurate();
    } else {
      step_size = 0;
      current_value = targetvalue;
    }
  }


  /** Given a target value and the number of steps to take on the way, this calculates the step size needed to get there from the current value.
      @param targetvalue the value to move towards.
      @param num_steps how many steps to take to reach the target.
  */
  template<typename T>
  void set(internal_type targetvalue, T num_steps)
  {
    if(num_steps) {
      auto numerator = targetvalue-current_value;
      step_size = internal_type(numerator.asRaw()/num_steps,true);
    } else {
      step_size = 0;
      current_value = targetvalue;
    }
  }

  /** Given a new starting value, target value and the number of steps to take on the way, this sets the step size needed to get there.
      @param startvalue the number to set the Line's current_value to.
      @param targetvalue the value to move towards.
      @param num_steps how many steps to take to reach the target.
  */
  template<typename T>
  void set(internal_type startvalue, internal_type targetvalue, T num_steps)
  {
    set(startvalue);
    set(targetvalue, num_steps);
  }  
};


/* SFix specialisation (if someone has an idea to avoid duplication with UFix) */
template<int8_t NI, int8_t NF>
class Line<SFix<NI, NF>>
{
private:
  typedef SFix<NI, NF> internal_type;
  internal_type current_value;
  SFix<NI+1, NF> step_size;

public:
  /** Constructor. Use the template parameter to set the type of numbers you
      want to use. For example, Line \<int\> myline; makes a Line which uses ints.
  */
  Line (){;}

  /** Increments one step along the line.
      @return the next value.
  */
  inline
  internal_type next()
  {
    current_value = current_value + step_size;
    return current_value;
  }

  /** Set the current value of the line. 
      The Line will continue incrementing from this
      value using any previously calculated step size.
      @param value the number to set the Line's current_value to.
  */
  inline
  void set(internal_type value)
  {
    current_value=value;
  }

  /** Given a target value and the number of steps to take on the way, this calculates the step size needed to get there from the current value.
      @param targetvalue the value to move towards.
      @param num_steps how many steps to take to reach the target as a UFix<_NI,0>
  */
  template<int8_t _NI>
  void set(internal_type targetvalue, UFix<_NI,0> num_steps)
  {
    if(num_steps.asRaw()) {
      auto numerator = targetvalue-current_value;
      step_size = numerator*num_steps.invAccurate();
    } else {
      step_size = 0;
      current_value = targetvalue;
    }
  }


  /** Given a target value and the number of steps to take on the way, this calculates the step size needed to get there from the current value.
      @param targetvalue the value to move towards.
      @param num_steps how many steps to take to reach the target.
  */
  template<typename T>
  void set(internal_type targetvalue, T num_steps)
  {
    if(num_steps) {
      auto numerator = targetvalue-current_value;
      step_size = internal_type(numerator.asRaw()/num_steps,true);
    } else {
      step_size = 0;
      current_value = targetvalue;
    }
  }

  /** Given a new starting value, target value and the number of steps to take on the way, this sets the step size needed to get there.
      @param startvalue the number to set the Line's current_value to.
      @param targetvalue the value to move towards.
      @param num_steps how many steps to take to reach the target.
  */
  template<typename T>
  void set(internal_type startvalue, internal_type targetvalue, T num_steps)
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
