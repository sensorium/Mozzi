/*
 * StateVariable.h
 *
 * This implementation copyright 2012 Tim Barrass.
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


/**
State Variable Filter (approximation of Chamberlin version)
Informed by pseudocode at http://www.musicdsp.org/showone.php?id=23 and http://www.musicdsp.org/showone.php?id=142.
References : 
Hal Chamberlin, Musical Applications of Microprocessors, 2nd Ed, Hayden Book Company 1985. pp 490-492.
Jon Dattorro, Effect Design Part 1, J. Audio Eng. Soc., Vol 45, No. 9, 1997 September
*/

#ifndef STATEVARIABLE_H_
#define STATEVARIABLE_H_

#include "Arduino.h"
#include "util/atomic.h"
#include "mozzi_fixmath.h"
#include "math.h"
#include "mozzi_utils.h"
#include "meta.h"


enum filter_types {LOWPASS,BANDPASS,HIGHPASS,NOTCH};

/** A State Variable filter which offers 12db resonant low, high, bandpass and notch modes.
@tparam FILTER_TYPE choose between LOWPASS, BANDPASS, HIGHPASS and NOTCH.
@note To save processing time, this version of the filter does not saturate internally, 
so any resonant peaks are unceremoniously truncated.  It may be worth adding code to
constrain the internal variables to enable resonant saturating effects.
@todo Try adding code to constrain the internal variables to enable resonant
saturating effects.
*/
template <char FILTER_TYPE>
class StateVariable
{

public:


	/** Constructor.
	*/
	StateVariable()
	{
	}



	/** Set how resonant the filter will be.
	@param resonance a byte value between 1 and 255.  
	The lower this value is, the more resonant the filter.
	At very low values, the filter can output loud peaks which can exceed
	Mozzi's output range, so you may need to attenuate the output in your sketch.
	@note Timing < 500 ns
	*/
	void setResonance(Q0n8 resonance){
		// qvalue goes from 255 to 0, representing .999 to 0 in fixed point
		// lower q, more resonance
		q = resonance;
		//ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		//{
		scale = (Q0n8)sqrt((unsigned int) resonance<<8);
		//}
	}



	/** Set the centre or corner frequency of the filter.
	@param centre_freq 20 - 4096 Hz (AUDIO_RATE/4).
	This will be the cut-off frequency for LOWPASS and HIGHPASS, and the
	centre frequency to pass or reduce for BANDPASS and NOTCH.
	@note Timing 25-30us
	*/
	/*
	void setCentreFreq(unsigned int centre_freq){
		// simple frequency tuning with error towards nyquist
		// F is the filter's center frequency
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			f = (Q1n15)(((Q16n16_2PI*centre_freq)>>AUDIO_RATE_AS_LSHIFT)>>1);
		}
	}
*/
	void setCentreFreq(unsigned int centre_freq){
		// simple frequency tuning with error towards nyquist
		// F is the filter's center frequency
		//ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		//{
			//f = (Q1n15)(((Q16n16_2PI*centre_freq)>>AUDIO_RATE_AS_LSHIFT)>>1);
			f = (Q1n15)((Q16n16_2PI*centre_freq)>>(AUDIO_RATE_AS_LSHIFT+1));
		//}
	}


	/** Calculate the next sample, given an input signal.
	@param input the signal input.
	@return the signal output.
	@note Timing: 16 - 20 us
	*/
	inline
	int next(int input)
	{
		// chooses a different next() function depending on whether the
		// filter is declared as LOWPASS, BANDPASS, HIGHPASS or NOTCH.
		// See meta.h.
		return next(input, Int2Type<FILTER_TYPE>());
	}




private:
	int low, band;
	Q0n8 q,scale;
	volatile Q1n15 f;


	/** Calculate the next sample, given an input signal.
	@param in the signal input.
	@return the signal output.
	@note Timing: 16 - 20 us
	*/
	inline
	int next(int input, Int2Type<LOWPASS>)
	{
		//SET_PIN13_HIGH;
		low += (((long)band * f)>>15);
		int high = (((long)input - low - (((long)band * q)>>8))*scale)>>8;
		band += (((long)high * f)>>15);
		//int notch = high + low;
		//SET_PIN13_LOW;
		return low;
	}


	/** Calculate the next sample, given an input signal.
	@param input the signal input.
	@return the signal output.
	@note Timing: 
	*/
	inline
	int next(int input, Int2Type<BANDPASS>)
	{
		//SET_PIN13_HIGH;
		low += (((long)band * f)>>15);
		int high = (((long)input - low - (((long)band * q)>>8))*scale)>>8;
		band += (((long)high * f)>>15);
		//int notch = high + low;
		//SET_PIN13_LOW;
		return band;
	}



	/** Calculate the next sample, given an input signal.
	@param input the signal input.
	@return the signal output.
	@note Timing: 
	*/
	inline
	int next(int input, Int2Type<HIGHPASS>)
	{
		//SET_PIN13_HIGH;
		low += (((long)band * f)>>15);
		int high = (((long)input - low - (((long)band * q)>>8))*scale)>>8;
		band += (((long)high * f)>>15);
		//int notch = high + low;
		//SET_PIN13_LOW;
		return high;
	}



	/** Calculate the next sample, given an input signal.
	@param input the signal input.
	@return the signal output.
	@note Timing: 16 - 20 us
	*/
	inline
	int next(int input, Int2Type<NOTCH>)
	{
		//SET_PIN13_HIGH;
		low += (((long)band * f)>>15);
		int high = (((long)input - low - (((long)band * q)>>8))*scale)>>8;
		band += (((long)high * f)>>15);
		int notch = high + low;
		//SET_PIN13_LOW;
		return notch;
	}

};

#endif /* STATEVARIABLE_H_ */
