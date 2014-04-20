/*
 * ADSR.h
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

#ifndef ADSR_H_
#define ADSR_H_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
//#include <util/atomic.h>
#include "Line.h"
#include "mozzi_fixmath.h"


/** A simple ADSR envelope generator.  This implementation has separate update() and next()
methods, where next() interpolates values between each update().
The "normal" way to use this would be with update() in updateControl(), where it calculates a new internal state each control step,
and then next() is in updateAudio(), called much more often, where it interpolates between the control values.
This also allows the ADSR updates to be made even more sparsely if desired, eg. every 3rd control update.
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
and rationalise which units which do and don't need them.  
Template objects are messy when you try to use pointers to them, 
you have to include the whole template shebang in the pointer handling.
*/
template <unsigned int CONTROL_UPDATE_RATE, unsigned int LERP_RATE>
class ADSR
{
private:

	const unsigned int LERPS_PER_CONTROL;

	unsigned int phase_control_step_counter;
	unsigned int phase_num_control_steps;

	enum {ATTACK,DECAY,SUSTAIN,RELEASE,IDLE};
	
	
	struct phase{
		byte phase_type;
		unsigned int control_steps;
		unsigned long lerp_steps;
		Q8n0 level;
	}attack,decay,sustain,release,idle;
	
	phase * current_phase;
	
	// Linear audio rate transitions for envelope
	Line <unsigned long> transition;

	inline
	unsigned int convertMsecToControlSteps(unsigned int msec){
		return (uint) (((ulong)msec*CONTROL_UPDATE_RATE)>>10); // approximate /1000 with shift
	}

	
	inline
	void setPhase(phase * next_phase) {
		phase_control_step_counter = 0;
		phase_num_control_steps = next_phase->control_steps;
		transition.set(Q8n0_to_Q16n16(next_phase->level),next_phase->lerp_steps);
		current_phase = next_phase;
	}
	
	
	
	inline
	void checkForAndSetNextPhase(phase * next_phase) {
		if (++phase_control_step_counter >= phase_num_control_steps){
			setPhase(next_phase);
		}
	}
				
	
	inline
	void checkForAndSetIdle() {
		if (++phase_control_step_counter >= phase_num_control_steps){
			transition.set(0,0,1);
			current_phase = &idle;
		}
	}
	
	
	
inline
	void setTime(phase * p, unsigned int msec)
	{
		p->control_steps=convertMsecToControlSteps(msec);
		p->lerp_steps = (ulong) p->control_steps * LERPS_PER_CONTROL;
	}
	
	
public:

	/** Constructor.
	 */
	ADSR():LERPS_PER_CONTROL(LERP_RATE/CONTROL_UPDATE_RATE)
	{
		attack.phase_type = ATTACK;
		decay.phase_type = DECAY;
		sustain.phase_type = SUSTAIN;
		release.phase_type = RELEASE;
		idle.phase_type = IDLE;
		release.level = 0;
	}


	
/** Updates the internal controls of the ADSR.
	Call this in updateControl().
	*/
	void update(){ // control rate
		
		switch(current_phase->phase_type) {
			
			case ATTACK:
				checkForAndSetNextPhase(&decay);
				break;

			case DECAY:
					checkForAndSetNextPhase(&sustain);
				break;
					
			case SUSTAIN:
					checkForAndSetNextPhase(&release);										
				break;
					
			case RELEASE:
					checkForAndSetIdle();							
				break;

		}	
	}

	
	
	/** Advances one audio step along the ADSR and returns the level.
	Call this in updateAudio().
	@return the next value, as an unsigned int.
	 */
	inline
	unsigned int next()
	{
		return Q16n16_to_Q16n0(transition.next());
	}



	/** Start the attack phase of the ADSR.  THis will restart the ADSR no matter what phase it is up to.
	*/
	inline
	void noteOn(){
		setPhase(&attack);
	}



	/** Start the release phase of the ADSR.
	@todo fix release for rate rather than steps (time), so it releases at the same rate whatever the current level.
	*/
	inline
	void noteOff(){   		
		setPhase(&release);
	}





	/** Set the attack level of the ADSR.
	@param value the attack level.
	 */
	inline
	void setAttackLevel(byte value)
	{
		attack.level=value;
	}



	/** Set the decay level of the ADSR.
	@param value the decay level.
	*/
	inline
	void setDecayLevel(byte value)
	{
		decay.level=value;
	}


	/** Set the sustain level of the ADSR.
	@param value the sustain level.  Usually the same as the decay level, 
	for a steady sustained note.
	*/
	inline
	void setSustainLevel(byte value)
	{
		sustain.level=value;
	}
	
	/** Set the release level of the ADSR.  Normally you'd make this 0, 
	but you have the option of some other value.
	@param value the release level (normally 0).
	*/
	inline
	void setReleaseLevel(byte value)
	{
		release.level=value;
	}
	
	
	
	/** Set the attack and decay levels of the ADSR.  This assumes a conventional 
	ADSR where the sustain continues at the same level as the decay, till the release ramps to 0.
	@param attack the new attack level.
	@param decay the new sustain level.
	*/
	inline
	void setADLevels(byte attack, byte decay)
	{
		setAttackLevel(attack);
		setDecayLevel(decay);
		setSustainLevel(decay);
		setReleaseLevel(0);
	}
	
	
	
	/** Set the attack time of the ADSR in milliseconds.
	The actual time taken will be resolved within the resolution of CONTROL_RATE.
	@param msec the unsigned int attack time in milliseconds.
	 */
	inline
	void setAttackTime(unsigned int msec)
	{
		setTime(&attack, msec);
	}


	/** Set the decay time of the ADSR in milliseconds.
	The actual time taken will be resolved within the resolution of CONTROL_RATE.
	@param msec the unsigned int decay time in milliseconds.
	*/
	inline
	void setDecayTime(unsigned int msec)
	{
		setTime(&decay, msec);
	}

	
	/** Set the sustain time of the ADSR in milliseconds.
	The actual time taken will be resolved within the resolution of CONTROL_RATE.
	The sustain phase will finish if the ADSR recieves a noteOff().
	@param msec the unsigned int sustain time in milliseconds.
	*/
	inline
	void setSustainTime(unsigned int msec)
	{
		setTime(&sustain, msec);
	}
	
	

	/** Set the release time of the ADSR in milliseconds.
	The actual time taken will be resolved within the resolution of CONTROL_RATE.
	@param msec the unsigned int release time in milliseconds.
	*/
	inline
	void setReleaseTime(unsigned int msec)
	{
		setTime(&release, msec);
	}
	


	/** Set the attack, decay and release times of the ADSR in milliseconds.
	The actual times will be resolved within the resolution of CONTROL_RATE.
	@param attack_ms the new attack time in milliseconds.
	@param decay_ms the new decay time in milliseconds.
	@param sustain_ms the new sustain time in milliseconds.
	@param release_ms the new release time in milliseconds.
	*/
	inline
	void setTimes(unsigned int attack_ms, unsigned int decay_ms, unsigned int sustain_ms, unsigned int release_ms)
	{
		setAttackTime(attack_ms);
		setDecayTime(decay_ms);
		setSustainTime(sustain_ms);
		setReleaseTime(release_ms);
	}


};


/** @example 07.Envelopes/ADSR_Envelope/ADSR_Envelope.ino
This is an example of how to use the ADSR class.
*/

#endif /* ADSR_H_ */
