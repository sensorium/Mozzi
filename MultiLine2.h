/*
 * MultiLine.h
 *
 * Copyright 2012-2015 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */

#ifndef MultiLine_H_
#define MultiLine_H_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include "Line.h"
#include "mozzi_fixmath.h"


/** A simple MultiLine envelope generator.  This implementation has separate update() and next()
methods, where next() interpolates values between each update().
The "normal" way to use this would be with update() in updateControl(), where it calculates a new internal state each control step,
and then next() is in updateAudio(), called much more often, where it interpolates between the control values.
This also allows the MultiLine updates to be made even more sparsely if desired, eg. every 3rd control update.
@tparam CONTROL_UPDATE_RATE The frequency of control updates.
Ordinarily this will be CONTROL_RATE, but an alternative (amongst others) is
to set this as well as the LERP_RATE parameter to AUDIO_RATE, and call both update() and next() in updateAudio().
Such a use would allow accurate envelopes with finer resolution of the control points than CONTROL_RATE.
@tparam LERP_RATE Sets how often next() will be called, to interpolate between updates set by CONTROL_UPDATE_RATE.
This will produce the smoothest results if it's set to AUDIO_RATE, but if you need to save processor time and your
envelope changes slowly or controls something like a filter where there may not be problems with glitchy or clicking transitions,
LERP_RATE could be set to CONTROL_RATE (for instance).  Then update() and next() could both be called in updateControl(),
greatly reducing the amount of processing required compared to calling next() in updateAudio().
@todo Test whether using the template parameters makes any difference to speed,
and rationalise which units  do and don't need them.
Template objects are messy when you try to use pointers to them,
you have to include the whole template shebang in the pointer handling.
*/

template <unsigned int NUM_PHASES, unsigned int CONTROL_UPDATE_RATE, unsigned int LERP_RATE>
class MultiLine
{
private:

	const unsigned int LERPS_PER_CONTROL;

	unsigned int update_step_counter;
	unsigned int num_update_steps;

	//enum {LINE1,LINE2,LINE3,LINE4,IDLE};


	struct phase{
		byte phase_type;
		unsigned int update_steps;
		long lerp_steps; // signed, to match params to transition (line) type Q15n16, below
		Q15n16 level;
	};//target1,target2,target3,target4,idle;

	phase phases[NUM_PHASES];
	unsigned int current_phase_num;

	phase * current_phase;

	Line <Q15n16> transition;


	inline
	unsigned int convertMsecToControlUpdateSteps(unsigned int msec){
		return (uint16_t) (((uint32_t)msec*CONTROL_UPDATE_RATE)>>10); // approximate /1000 with shift
	}


	inline
	void setPhase(phase * next_phase) {
		update_step_counter = 0;
		num_update_steps = next_phase->update_steps;
		transition.set(next_phase->level, next_phase->lerp_steps);
		current_phase = next_phase;
	}



	inline
	void checkForAndSetNextPhase() {
		if (++update_step_counter >= num_update_steps){
			if(current_phase_num < NUM_PHASES) current_phase_num++;
			setPhase(&(phases[current_phase_num]));
		}
	}



	inline
	void setTime(phase * p, unsigned int msec)
	{
		p->update_steps = convertMsecToControlUpdateSteps(msec);
		p->lerp_steps = (long) p->update_steps * LERPS_PER_CONTROL;
	}


	inline
	void setUpdateSteps(phase * p, unsigned int steps)
	{
		p->update_steps = steps;
		p->lerp_steps = (long) steps * LERPS_PER_CONTROL;
	}



public:

	/** Constructor.
	 */
	MultiLine():LERPS_PER_CONTROL(LERP_RATE/CONTROL_UPDATE_RATE)
	{
		//for(int i=0;i++;i<NUM_PHASES-1) phases[i].phase_type =
		// target1.phase_type = LINE1;
		// target2.phase_type = LINE2;
		// target3.phase_type = LINE3;
		// target4.phase_type = LINE4;
		// idle.phase_type = IDLE;
		// target4.level = 0;
		current_phase_num = NUM_PHASES-1;
		current_phase = &phases[current_phase_num];
	}



	/** Updates the internal controls of the MultiLine.
		Call this in updateControl().
		*/
	void update(){ // control rate
		checkForAndSetNextPhase();
}



	/** Advances one step along the MultiLine and returns the level.
	Call this in updateAudio().
	@return the next value, as a Q15n16 fixed-point number.
	 */
	inline
	Q15n16 next()
	{
		return transition.next();
	}



	/** Start the target1 phase of the MultiLine.  THis will restart the MultiLine no matter what phase it is up to.
	*/
	inline
	void start(){
		current_phase_num = 0;
		setPhase(&(phases[current_phase_num]));
	}



	/** Jump to to the final phase in the sequence
	*/
	inline
	void stop(){
		setPhase(&(phases[NUM_PHASES-1]));
	}



	/** Set the target level of one of the phases.
  @param phase_num which phase to edit.
	@param value the target level of the phase being edited.
	 */
	inline
	void setTargetLevel(uint8_t phase_num, Q15n16 value)
	{
		phases[phase_num].level=value;
	}


	/** Set the time taken to reach the target level of a phase, in milliseconds.
	The actual time taken will be resolved within the resolution of CONTROL_RATE.
  @param phase_num which phase to edit.
  @param msec the target time in milliseconds.
	@note Beware of low time values in case internal step size gets calculated as 0, which would mean nothing happens.
	 */
	inline
	void setTargetTime(uint8_t phase_num, unsigned int msec)
	{
		setTime(&(phases[phase_num]), msec);
	}


	/** Set the target1 time of the MultiLine, expressed as the number of update steps (not MultiLine::next() interpolation steps) in the target1 phase.
	@param steps the number of times MultiLine::update() will be called in the target1 phase.
	 */
	inline
	void setTargetUpdateSteps(uint8_t phase_num, unsigned int steps)
	{
		setUpdateSteps(&(phases[phase_num]), steps);
	}


	/** Tells if the MultiLine is currently playing.
	@return true if playing, false if in the final state
	*/
	inline
	bool playing()
	{
		return current_phase_num != NUM_PHASES-1;
	}


};


/** @example 07.Envelopes/MultiLine_Envelope/MultiLine_Envelope.ino
This is an example of how to use the MultiLine class.
*/

#endif /* MultiLine_H_ */
