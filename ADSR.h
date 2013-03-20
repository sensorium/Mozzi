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

#include "Arduino.h"
//#include <util/atomic.h>
#include "Line.h"
#include "mozzi_fixmath.h"

/** A simple ADSR envelope generator.
@todo Test whether using the template parameter makes any difference to speed, 
and rationalise which units which do and don't need them.  
Template objects are messy when you try to use pointers to them, 
you have to include the whole template shebang in the pointer handling.
*/
template <unsigned int CONTROL_UPDATE_RATE>
class ADSR
{
private:

	const unsigned int AUDIO_TICKS_PER_CONTROL;

	unsigned int phase_control_step_counter;
	unsigned int phase_num_control_steps;

	enum {ATTACK,DECAY,SUSTAIN,RELEASE,IDLE};
	
	
	struct phase{
		byte phase_type;
		unsigned int control_steps;
		unsigned long audio_steps;
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
		transition.set(Q8n0_to_Q16n16(next_phase->level),next_phase->audio_steps);
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
		p->audio_steps = (ulong) p->control_steps * AUDIO_TICKS_PER_CONTROL;
	}
	
	
public:

	/** Constructor.
	 */
	ADSR():AUDIO_TICKS_PER_CONTROL(AUDIO_RATE/CONTROL_UPDATE_RATE)
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
	@param value the new sustain level.
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
	@param value the unsigned int attack time in milliseconds.
	 */
	inline
	void setAttackTime(unsigned int msec)
	{
		setTime(&attack, msec);
	}


	/** Set the decay time of the ADSR in milliseconds.
	The actual time taken will be resolved within the resolution of CONTROL_RATE.
	@param value the unsigned int decay time in milliseconds.
	*/
	inline
	void setDecayTime(unsigned int msec)
	{
		setTime(&decay, msec);
	}

	
	/** Set the sustain time of the ADSR in milliseconds.
	The actual time taken will be resolved within the resolution of CONTROL_RATE.
	The sustain phase will finish if the ADSR recieves a noteOff().
	@param value the unsigned int sustain time in milliseconds.
	*/
	inline
	void setSustainTime(unsigned int msec)
	{
		setTime(&sustain, msec);
	}
	
	

	/** Set the release time of the ADSR in milliseconds.
	The actual time taken will be resolved within the resolution of CONTROL_RATE.
	@param value the unsigned int release time in milliseconds.
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
	@param decay_ms the new sustain time in milliseconds.
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

#endif /* ADSR_H_ */
