/*
 * Oscil.h
 *
 * Oscil.h owes much to AF_precision_synthesis.pde, 2009, Adrian Freed.
 *
 * Copyright 2012 Tim Barrass, 2009 Adrian Freed.
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

#ifndef OSCIL_H_
#define OSCIL_H_

#include "Arduino.h"
#include "MozziGuts.h"
#include "fixedMath.h"
#include <util/atomic.h>

// fractional bits for oscillator index precision
#define F_BITS 16
#define F_BITS_AS_MULTIPLIER 65536

// phmod_proportion is an 1n15 fixed-point number only using
// the fractional part and the sign bit
#define PHMOD_BITS 16

/** Oscil plays a wavetable, cycling through the table to generate an audio or
control signal. The frequency of the signal can be set or changed with
setFreq(), and the output of an Oscil can be produced with next() for a simple
cycling oscillator, or atIndex() for a particular sample in the table.
@tparam NUM_TABLE_CELLS This is defined in the table ".h" file the Oscil will be
using. It's important that it's either a literal number (eg. "8192") or a
defined macro, rather than a const or int, for the Oscil to run fast enough.
@tparam UPDATE_RATE This will be AUDIO_RATE if the Oscil is updated in
updateAudio(), or CONTROL_RATE if it's updated each time updateControl() is
called. It could also be a fraction of CONTROL_RATE if you are doing some kind
of cyclic updating in updateControl(), for example, to spread out the processor load.
@todo Use conditional compilation to optimise setFreq() variations for different table
sizes.
@section raw2mozzi
Converting soundfiles for Mozzi
There is a python script called raw2mozzi.py in the Mozzi/python folder.
The usage is:
python raw2mozzi.py infilename outfilename tablename samplerate
*/
template <unsigned int NUM_TABLE_CELLS, unsigned int UPDATE_RATE>
class Oscil
{

public:
	/** Constructor.
	@param TABLE_NAME the name of the array the Oscil will be using. This
	can be found in the table ".h" file if you are using a table made for
	Mozzi by the raw2mozzi.py python script in Mozzi's python
	folder.*/
	Oscil(prog_char * TABLE_NAME):table(TABLE_NAME)
	{}


	/** Constructor.
	Declare an Oscil with template TABLE_NUM_CELLS and UPDATE_RATE
	parameters, without specifying a particular wave table for it to play.
	The table can be set or changed on the fly with setTable(). Any tables
	used by the Oscil must be the same size.
	*/
	Oscil()
	{}


	/** Updates the phase according to the current frequency
	and returns the sample at the new phase position.
	@return the next sample.
	*/
	inline
	char next()
	{
		incrementPhase();
		return readTable();
	}


	/** Change the sound table which will be played by the Oscil.
	@param TABLE_NAME is the name of the array in the table ".h" file you're using.
	*/
	void setTable(prog_char * TABLE_NAME)
	{
		table = TABLE_NAME;
	}


	/** Set the phase of the Oscil.  This does the same thing as Sample::start(offset).  Just different ways of thinking about oscillators and samples.
	@param phase a position in the wavetable.
	*/
	void setPhase(unsigned int phase)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			phase_fractional = (unsigned long)phase << F_BITS;
		}
	}


	/** Returns the next sample given a phase modulation value.
	@param a phase modulation value given as a proportion of the wave. The
	phmod_proportion parameter is a Q15n16 fixed-point number where the fractional
	n16 part represents -1 to 1, modulating the phase by one whole table length in
	each direction.
	@return a sample from the table.
	*/
	// PM: cos((angle += incr) + change)
	// FM: cos(angle += (incr + change))
	// The ratio of deviation to modulation frequency is called the "index of modulation". ( I = d / Fm )
	inline
	char phMod(Q15n16 phmod_proportion)
	{
		incrementPhase();
		return (char)pgm_read_byte_near(table + (((phase_fractional+(phmod_proportion * NUM_TABLE_CELLS))>>F_BITS) & (NUM_TABLE_CELLS - 1)));
	}


	/** Set the frequency using Q24n8 fixed-point number format.
	This might be faster than the float version for setting low frequencies such as
	1.5 Hz, or other values which may not work well with your table size. A Q24n8
	representation of 1.5 is 384 (ie. 1.5 * 256). Can't be used with UPDATE_RATE
	less than 64 Hz.
	@param frequency in Q24n8 fixed-point number format.
	*/
	inline
	void setFreq_Q24n8(Q24n8 frequency)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			phase_increment_fractional = (frequency* (NUM_TABLE_CELLS>>3)/(UPDATE_RATE>>6)) << (F_BITS-(8-3+6));
		}
	}


	/** Set the frequency using Q16n16 fixed-point number format. This is useful in
	combination with Q16n16_mtof(), a fast alternative to mtof(), using Q16n16
	fixed-point format instead of floats.  Note: this should work OK with tables 2048 cells or smaller and
	frequencies up to 4096 Hz.  Can't be used with UPDATE_RATE
	less than 64 Hz.
	@param frequency in Q16n16 fixed-point number format.
	*/
	inline
	void setFreq_Q16n16(Q16n16 frequency)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			phase_increment_fractional = ((frequency * (NUM_TABLE_CELLS>>7))/(UPDATE_RATE>>6)) << (F_BITS-16+1);
		}
	}


	/** Set the oscillator frequency with an unsigned int. This is faster than using a
	float, so it's useful when processor time is tight, but it can be tricky with
	low and high frequencies, depending on the size of the wavetable being used. If
	you're not getting the results you expect, try explicitly using a float, or try
	setFreq_Q24n8() or or setFreq_Q16n16().
	@param frequency to play the wave table.
	*/
	inline
	void setFreq(unsigned int frequency)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			phase_increment_fractional = (((unsigned long)frequency * NUM_TABLE_CELLS)/UPDATE_RATE) << F_BITS;
		}
	}


	/** Set the oscillator frequency with a float. Using a float is the most reliable
	way to set frequencies, -Might- be slower than using an int but you need either
	this, setFreq_Q24n8() or setFreq_Q16n16() for fractional frequencies.
	@param frequency to play the wave table.
	 */
	inline
	void setFreq(float frequency)
	{ // 1 us - using float doesn't seem to incur measurable overhead with the oscilloscope
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			phase_increment_fractional = (unsigned long)((((float)NUM_TABLE_CELLS * frequency)/UPDATE_RATE) * F_BITS_AS_MULTIPLIER);
		}
	}


	/**  Returns the sample at the given table index.
	@param atIndex table index between 0 and the table size.The
	index rolls back around to 0 if it's larger than the table size.
	@return the sample at the given table index.
	*/
	inline
	char atIndex(unsigned int index)
	{
		return (char)pgm_read_byte_near(table + (index & (NUM_TABLE_CELLS - 1)));
	}


	/** phaseIncFromFreq() and setPhaseInc() are for saving processor time when sliding
	between frequencies. Instead of recalculating the phase increment for each
	frequency in between, you can just calculate the phase increment for each end
	frequency with phaseIncFromFreq(), then use a Line to interpolate on the fly and
	use setPhaseInc() to set the phase increment at each step. (Note: I should
	really profile this with the oscilloscope to see if it's worth the extra
	confusion!)
	@param frequency for which you want to calculate a phase increment value.
	@return the phase increment value which will produce a given frequency.
	*/
	inline
	unsigned long phaseIncFromFreq(unsigned int frequency)
	{
		return (((unsigned long)frequency * NUM_TABLE_CELLS)/UPDATE_RATE) << F_BITS;
	}


	/** Set a specific phase increment.  See phaseIncFromFreq().
	@param phaseinc_fractional a phase increment value as calculated by phaseIncFromFreq().
	 */
	inline
	void setPhaseInc(unsigned long phaseinc_fractional)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			phase_increment_fractional = phaseinc_fractional;
		}
	}


private:

	/** Increments the phase of the oscillator without returning a sample.
	 */
	inline
	void incrementPhase()
	{
		phase_fractional += phase_increment_fractional;
	}


	/** Returns the current sample.
	 */
	inline
	char readTable()
	{
		return (char)pgm_read_byte_near(table + ((phase_fractional >> F_BITS) & (NUM_TABLE_CELLS - 1)));
	}

	unsigned long phase_fractional;
	/** @todo  does Oscil::phase_increment_fractional really need to be declared volatile?*/
	volatile unsigned long phase_increment_fractional;
	prog_char * table;

};


#endif /* OSCIL_H_ */
