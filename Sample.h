/*
 * Sample.h
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

#ifndef SAMPLE_H_
#define SAMPLE_H_

#include "Arduino.h"
#include "MozziGuts.h"
#include <util/atomic.h>

// fractional bits for samplelator index precision
#define SAMPLE_F_BITS 16
#define SAMPLE_F_BITS_AS_MULTIPLIER 65536

// phmod_proportion is an 1n15 fixed-point number only using
// the fractional part and the sign bit
#define SAMPLE_PHMOD_BITS 16


/** Sample is like Oscil, it plays a wavetable, but defaults to playing once through,
from start to finish.
@tparam NUM_TABLE_CELLS This is defined in the table ".h" file the Sample will be
using. It's important that it's either a literal number (eg. "8192") or a
defined macro, rather than a const or int, for the Sample to run fast enough.
@tparam UPDATE_RATE This will be AUDIO_RATE if the Sample is updated in
updateAudio(), or CONTROL_RATE if it's updated each time updateControl() is
called. It could also be a fraction of CONTROL_RATE if you are doing some kind
of cyclic updating in updateControl(), for example, to spread out the processor load.
@section raw2mozzi
Converting soundfiles for Mozzi.
There is a python script called raw2mozzi.py in the Mozzi/python folder.
The usage is:
python raw2mozzi.py infilename outfilename tablename samplerate
@todo Make Sample a descendent of Oscil.
*/
template <unsigned int NUM_TABLE_CELLS, unsigned int UPDATE_RATE>
class Sample
{

public:

	/** Constructor.
	@param TABLE_NAME the name of the array the Sample will be using. This
	can be found in the table ".h" file if you are using a table made for
	Mozzi by the raw2mozzi.py python script in Mozzi's python
	folder.*/
	Sample(const char * TABLE_NAME):table(TABLE_NAME)
	{}



	/** Constructor.
	Declare a Sample with template TABLE_NUM_CELLS and UPDATE_RATE parameters, without specifying a particular wave table for it to play.
	The table can be set or changed on the fly with setTable().
	*/
	Sample()
	{}


	/** Change the sound table which will be played by the Sample.
	@param TABLE_NAME is the name of the array in the table ".h" file you're using.
	*/
	void setTable(const char * TABLE_NAME)
	{
		table = TABLE_NAME;
	}


	/** Resets the phase (the playhead) to the beginning of the table.
	*/
	inline
	void start()
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			phase_fractional = 0;
		}
	}


	/** Sets the phase (the playhead) to an offset position.  This does the same thing as Oscil::setPhase(phase).  Just different ways of thinking about samples and oscillators.
	@param offset position in samples.
	*/
	inline
	void start(unsigned int offset)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			phase_fractional = (unsigned long) offset<<SAMPLE_F_BITS;
		}
	}


	/** Updates the phase according to the current frequency and returns the sample at
	the new phase position, or 0 when the phase overshoots the end of the table.
	@return the next sample value from the table, or 0 if it's finished playing.
	*/
	inline
	char next()
	{
		char out = 0;
		if (phase_fractional < (unsigned long) NUM_TABLE_CELLS<<SAMPLE_F_BITS )
		{
			incrementPhase();
			out = readTable();
		}
		return out;
	}


	/** Returns the next sample given a phase modulation value.
	@param a phase modulation value given as a proportion of the wave. The
	phmod_proportion parameter is a Q15n16 fixed-point number where to fractional
	n16 part represents -1 to 1, modulating the phase by one whole table length in
	each direction.
	@return a sample from the table.
	*/
	inline
	char phMod(long phmod_proportion)
	{
		incrementPhase();
		return (char)pgm_read_byte_near(table + (((phase_fractional+(phmod_proportion * NUM_TABLE_CELLS))>>SAMPLE_F_BITS) & (NUM_TABLE_CELLS - 1)));
	}

	/** Set the frequency using Q24n8 fixed-point number format.
	This might be faster than the float version for setting low frequencies
	such as 1.5 Hz, or other values which may not work well with your table
	size. Note: use with caution because it's prone to overflow with higher
	frequencies and larger table sizes. An Q24n8 representation of 1.5 is 384
	(ie. 1.5 * 256).
	@param frequency in Q24n8 fixed-point number format.
	*/
	inline
	void setFreq_n8(unsigned long frequency)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			phase_increment_fractional = ((frequency * NUM_TABLE_CELLS)/UPDATE_RATE) << (SAMPLE_F_BITS-8);
		}
	}

	/** Set the oscillator frequency with an unsigned int. This is faster than using a
	float, so it's useful when processor time is tight, but it can be tricky with
	low and high frequencies, depending on the size of the wavetable being used. If
	you're not getting the results you expect, try explicitly using a float, or try
	setFreq_n8.
	@param frequency to play the wave table.
	*/
	inline
	void setFreq(unsigned int frequency)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			phase_increment_fractional = (((unsigned long)frequency * NUM_TABLE_CELLS)/UPDATE_RATE) << SAMPLE_F_BITS; // Obvious but slow
		}
	}

	/** Set the oscillator frequency with a float. Using a float is the most reliable
	way to set frequencies, -Might- be slower than using an int but you need either
	this or setFreq_n8 for fractional frequencies.
	@param frequency to play the wave table.
	 */
	inline
	void setFreq(float frequency)
	{ // 1 us - using float doesn't seem to incur measurable overhead with the oscilloscope
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			phase_increment_fractional = (unsigned long)((((float)NUM_TABLE_CELLS * frequency)/UPDATE_RATE) * SAMPLE_F_BITS_AS_MULTIPLIER);
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
		return (((unsigned long)frequency * NUM_TABLE_CELLS)/UPDATE_RATE) << SAMPLE_F_BITS;
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
		return (char)pgm_read_byte_near(table + ((phase_fractional >> SAMPLE_F_BITS) & (NUM_TABLE_CELLS - 1)));
	}

	unsigned long phase_fractional;
	volatile unsigned long phase_increment_fractional;
	const char * table;

};

#endif /* SAMPLE_H_ */
