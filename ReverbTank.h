#ifndef REVERBTANK_H
#define REVERBTANK_H

/*
 * ReverbTank.h
 *
 * Copyright 2013 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */

#include "AudioDelay.h"
/**
A reverb which sounds like the inside of a tin can.
ReverbTank is small enough to fit on the Arduino Nano, which for some reason
wasn't able to fit a larger version which did fit on other 328 based boards. For
simplicity, ReverbTank has hardcoded maximum delay sizes but also has default
delay times which can be changed in the constructor or by setting during run
time to allow live tweaking.
This is a highly simplified design drawing on and probably misinterpreting 
Miller Puckette's G08.reverb recirculating reverb example for Pure Data.

The room size according to the maximum delay lengths corresponds to:

early reflections and recirculating delay 1: 128/16384 seconds * 340.29 m/s speed of sound = 3.5 metres
recirculating delay 2: 7 metres
It looks bigger on paper than it sounds.
*/
class
	ReverbTank {

public:
	/** Constructor.  This has default values for the early reflection times, recirculating delay lengths and feedback level, 
	which can be changed here in the constructor or set with other functions during run time.
	@param early_reflection1 how long in delay cells till the first early reflection, from 0 to 127
	@param early_reflection2 how long in delay cells till the second early reflection, from early_reflection1 to 127
	@param early_reflection3 how long in delay cells till the third early reflection, from early_reflection2 to 127
	@param loop1_delay how long in delay cells for the first recirculating delay, form 0 to 127
	@param loop2_delay how long in delay cells for the first recirculating delay, form 0 to 255
	@param feedback_level how much recirculation, from -128 to 127
	*/
	ReverbTank(
	  int8_t early_reflection1 = 37,
	  int8_t early_reflection2 = 77,
	  int8_t early_reflection3 = 127,
	  int8_t loop1_delay=117,
	  uint8_t loop2_delay=255,
	  int8_t feedback_level = 85):
			_early_reflection1(early_reflection1),_early_reflection2(early_reflection3),_early_reflection3(early_reflection3),
			_feedback_level(feedback_level)
	{
		aLoopDel1.set(loop1_delay);
		aLoopDel2.set(loop2_delay);
	}


	/** Process the next audio sample and return the reverbed signal.  This returns only the "wet" signal,
	which can be combined with the dry input signal in the sketch.
	@param input the audio signal to process
	@return the processed signal
	*/
	int next(int input){
		static int recycle1, recycle2;

		// early reflections
		int asig = aLoopDel0.next(input, _early_reflection1);
		asig += aLoopDel0.read(_early_reflection2);
		asig += aLoopDel0.read(_early_reflection3);
		asig >>= 2;

		// recirculating delays
		int8_t feedback_sig1 = (int8_t) min(max(((recycle1 * _feedback_level)>>7),-128),127); // feedback clipped
		int8_t feedback_sig2 = (int8_t) min(max(((recycle2 * _feedback_level)>>7),-128),127); // feedback clipped
		int sig3 = aLoopDel1.next(asig+feedback_sig1);
		int sig4 = aLoopDel2.next(asig+feedback_sig2);
		recycle1 = sig3 + sig4;
		recycle2 = sig3 - sig4;

		return recycle1;
	}


	/** Set the early reflection times in terms of delay cells.
	@param early_reflection1 how long in delay cells till the first early reflection, from 0 to 127
	@param early_reflection2 how long in delay cells till the second early reflection, from early_reflection1 to 127
	@param early_reflection3 how long in delay cells till the third early reflection, from early_reflection2 to 127
	*/
	void setEarlyReflections(int8_t early_reflection1, int8_t early_reflection2, int8_t early_reflection3){
		_early_reflection1=early_reflection1;
		_early_reflection2=early_reflection2;
		_early_reflection3=early_reflection3;
	}


	/** Set the loop delay times in terms of delay cells.
	@param loop1_delay how long in delay cells for the first recirculating delay, form 0 to 127
	@param loop2_delay how long in delay cells for the first recirculating delay, form 0 to 255
	*/
	void setLoopDelays(int8_t loop1_delay, uint8_t loop2_delay){
		aLoopDel1.set(loop1_delay);
		aLoopDel2.set(loop2_delay);
	}

	/** Set the feedback level for the recirculating delays.
	@param feedback_level how much recirculation, from -128 to 127
	*/
	void setFeebackLevel(int8_t feedback_level){
		_feedback_level=feedback_level;
	}


private:
	int8_t _early_reflection1;
	int8_t _early_reflection2;
	int8_t _early_reflection3;

	int8_t _feedback_level;

	AudioDelay <128> aLoopDel0; // 128/16384 seconds * 340.29 m/s speed of sound = 3.5 metres
	AudioDelay <128,int> aLoopDel1;
	AudioDelay <256,int> aLoopDel2; // 7 metres

};

/**
@example 09.Delays/ReverbTank_STANDARD/ReverbTank_STANDARD.ino
This example demonstrates the ReverbTank class.
*/

#endif        //  #ifndef REVERBTANK_H
