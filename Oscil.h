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

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include "MozziGuts.h"
#include "mozzi_fixmath.h"
#include <util/atomic.h>


#ifdef OSCIL_DITHER_PHASE
#include "mozzi_rand.h"
#endif

// fractional bits for oscillator index precision
#define OSCIL_F_BITS 16
#define OSCIL_F_BITS_AS_MULTIPLIER 65536

// phmod_proportion is an 1n15 fixed-point number only using
// the fractional part and the sign bit
#define OSCIL_PHMOD_BITS 16



/** 
Oscil plays a wavetable, cycling through the table to generate an audio or
control signal. The frequency of the signal can be set or changed with
setFreq(), and the output of an Oscil can be produced with next() for a simple
cycling oscillator, or atIndex() for a particular sample in the table.
@tparam NUM_TABLE_CELLS This is defined in the table ".h" file the Oscil will be
using. It's important that it's a power of 2, and either a literal number (eg. "8192") or a
defined macro, rather than a const or int, for the Oscil to run fast enough.
@tparam UPDATE_RATE This will be AUDIO_RATE if the Oscil is updated in
updateAudio(), or CONTROL_RATE if it's updated each time updateControl() is
called. It could also be a fraction of CONTROL_RATE if you are doing some kind
of cyclic updating in updateControl(), for example, to spread out the processor load.
@todo Use conditional compilation to optimise setFreq() variations for different table
sizes.
@note If you #define OSCIL_DITHER_PHASE before you #include <Oscil.h>,
the phase increments will be dithered, which reduces spurious frequency spurs
in the audio output, at the cost of some extra processing and memory.
@section char2mozzi
Converting soundfiles for Mozzi
There is a python script called char2mozzi.py in the Mozzi/python folder.
The usage is:
char2mozzi.py infilename outfilename tablename samplerate
*/
//template <unsigned int NUM_TABLE_CELLS, unsigned int UPDATE_RATE, bool DITHER_PHASE=false>
template <unsigned int NUM_TABLE_CELLS, unsigned int UPDATE_RATE>
class Oscil
{


public:
	/** Constructor.
	@param TABLE_NAME the name of the array the Oscil will be using. This
	can be found in the table ".h" file if you are using a table made for
	Mozzi by the char2mozzi.py python script in Mozzi's python
	folder.*/
	Oscil(const char * TABLE_NAME):table(TABLE_NAME)
	{}


	/** Constructor.
	Declare an Oscil with template TABLE_NUM_CELLS and UPDATE_RATE
	parameters, without specifying a particular wave table for it to play.
	The table can be set or changed on the fly with setTable(). Any tables
	used by the Oscil must be the same size.
	*/
	Oscil()
	{}


	/** Updates the phase according to the current frequency and returns the sample at the new phase position.
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
	void setTable(const char * TABLE_NAME)
	{
		table = TABLE_NAME;
	}


	/** Set the phase of the Oscil.  This does the same thing as Sample::start(offset).  Just different ways of thinking about oscillators and samples.
	@param phase a position in the wavetable.
	@todo Test commenting out ATOMIC_BLOCK in setPhase(), setFreq(), etc.
	*/
	// This could be called in the control interrupt, so phase_fractional should really be volatile,
	// but that could limit optimisation.  Since phase_fractional gets changed often in updateAudio()
	// (in loop()), it's probably worth keeping it nonvolatile until it causes problems
	void setPhase(unsigned int phase)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			phase_fractional = (unsigned long)phase << OSCIL_F_BITS;
		}
	}

	/** Set the phase of the Oscil.  Might be useful with getPhaseFractional().
	@param phase a position in the wavetable.
	@todo Test commenting out ATOMIC_BLOCK in setPhase(), setFreq(), etc.
	*/
	// This could be called in the control interrupt, so phase_fractional should really be volatile,
	// but that could limit optimisation.  Since phase_fractional gets changed often in updateAudio()
	// (in loop()), it's probably worth keeping it nonvolatile until it causes problems
	void setPhaseFractional(unsigned long phase)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			phase_fractional = phase;
		}
	}


	/** Get the phase of the Oscil in fractional format.
	@return position in the wavetable, shifted left by OSCIL_F_BITS (which is 16 when this was written).
	*/
	unsigned long getPhaseFractional()
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			return phase_fractional;
		}
	}



	/** Returns the next sample given a phase modulation value.
	@param phmod_proportion a phase modulation value given as a proportion of the wave. The
	phmod_proportion parameter is a Q15n16 fixed-point number where the fractional
	n16 part represents -1 to 1, modulating the phase by one whole table length in
	each direction.
	@return a sample from the table.
	@todo does the phmod_proportion param need to be Q15n16, why not Q0n15 and cast to long for multiply?
	*/
	// PM: cos((angle += incr) + change)
	// FM: cos(angle += (incr + change))
	// The ratio of deviation to modulation frequency is called the "index of modulation". ( I = d / Fm )
	inline
	char phMod(Q15n16 phmod_proportion)
	{
		incrementPhase();
		return (char)pgm_read_byte_near(table + (((phase_fractional+(phmod_proportion * NUM_TABLE_CELLS))>>OSCIL_F_BITS) & (NUM_TABLE_CELLS - 1)));
	}


	/** Set the oscillator frequency with an unsigned int. This is faster than using a
	float, so it's useful when processor time is tight, but it can be tricky with
	low and high frequencies, depending on the size of the wavetable being used. If
	you're not getting the results you expect, try explicitly using a float, or try
	setFreq_Q24n8() or or setFreq_Q16n16().
	@param frequency to play the wave table.
	*/
	inline
	void setFreq (int frequency) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			phase_increment_fractional = ((((unsigned long)NUM_TABLE_CELLS<<ADJUST_FOR_NUM_TABLE_CELLS)*frequency)/UPDATE_RATE) << (OSCIL_F_BITS - ADJUST_FOR_NUM_TABLE_CELLS);
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
			phase_increment_fractional = (unsigned long)((((float)NUM_TABLE_CELLS * frequency)/UPDATE_RATE) * OSCIL_F_BITS_AS_MULTIPLIER);
		}
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
			//phase_increment_fractional = (frequency* (NUM_TABLE_CELLS>>3)/(UPDATE_RATE>>6)) << (F_BITS-(8-3+6));
			phase_increment_fractional = (((((unsigned long)NUM_TABLE_CELLS<<ADJUST_FOR_NUM_TABLE_CELLS)>>3)*frequency)/(UPDATE_RATE>>6))
			                             << (OSCIL_F_BITS - ADJUST_FOR_NUM_TABLE_CELLS - (8-3+6));
		}
	}


	/** Set the frequency using Q16n16 fixed-point number format. This is useful in
	combination with Q16n16_mtof(), a fast alternative to mtof(), using Q16n16
	fixed-point format instead of floats.
	@note This should work OK with tables 2048 cells or smaller and
	frequencies up to 4096 Hz.  Can't be used with UPDATE_RATE less than 64 Hz.
	@param frequency in Q16n16 fixed-point number format.
	*/
	inline
	void setFreq_Q16n16(Q16n16 frequency)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			//phase_increment_fractional = ((frequency * (NUM_TABLE_CELLS>>7))/(UPDATE_RATE>>6)) << (F_BITS-16+1);
			phase_increment_fractional = (((((unsigned long)NUM_TABLE_CELLS<<ADJUST_FOR_NUM_TABLE_CELLS)>>7)*frequency)/(UPDATE_RATE>>6))
			                             << (OSCIL_F_BITS - ADJUST_FOR_NUM_TABLE_CELLS - 16 + 1);

		}
	}
/*
	inline
	void setFreqMidi(char note_num) {
		setFreq_Q16n16(mtof(note_num));
	}
*/
	/**  Returns the sample at the given table index.
	@param index between 0 and the table size.The
	index rolls back around to 0 if it's larger than the table size.
	@return the sample at the given table index.
	*/
	inline
	char atIndex(unsigned int index)
	{
		return (char)pgm_read_byte_near(table + (index & (NUM_TABLE_CELLS - 1)));
	}


	/** phaseIncFromFreq() and setPhaseInc() are for saving processor time when sliding between frequencies. 
	Instead of recalculating the phase increment for each
	frequency in between, you can just calculate the phase increment for each end
	frequency with phaseIncFromFreq(), then use a Line to interpolate on the fly and
	use setPhaseInc() to set the phase increment at each step. (Note: I should
	really profile this with the oscilloscope to see if it's worth the extra
	confusion!)
	@param frequency for which you want to calculate a phase increment value.
	@return the phase increment value which will produce a given frequency.
	*/
	inline
	const
	unsigned long phaseIncFromFreq(int frequency)
	{
		return (((unsigned long)frequency * NUM_TABLE_CELLS)/UPDATE_RATE) << OSCIL_F_BITS;
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


	/** Used for shift arithmetic in setFreq() and its variations.
	*/
static const unsigned char ADJUST_FOR_NUM_TABLE_CELLS = (NUM_TABLE_CELLS<2048) ? 8 : 0;


	/** Increments the phase of the oscillator without returning a sample.
	 */
	inline
	void incrementPhase()
	{
		//phase_fractional += (phase_increment_fractional | 1); // odd phase incr, attempt to reduce frequency spurs in output
		phase_fractional += phase_increment_fractional;
	}


	/** Returns the current sample.
	 */
	inline
	char readTable()
	{
#ifdef OSCIL_DITHER_PHASE
		return (char)pgm_read_byte_near(table + (((phase_fractional + ((int)(xorshift96()>>16))) >> OSCIL_F_BITS) & (NUM_TABLE_CELLS - 1)));
#else
		return (char)pgm_read_byte_near(table + ((phase_fractional >> OSCIL_F_BITS) & (NUM_TABLE_CELLS - 1)));
		//return (char)pgm_read_byte_near(table + (((phase_fractional >> OSCIL_F_BITS) | 1 ) & (NUM_TABLE_CELLS - 1))); odd phase, attempt to reduce frequency spurs in output
#endif
	}


	unsigned long phase_fractional;
	volatile unsigned long phase_increment_fractional; // volatile with atomic access because it can
	// be set in the updateControl() interrupt and
	// used in updateAudio(), which is outside the
	// interrupt.
	const char * table;

};


/**
@example 01.Basics/Vibrato/Vibrato.ino
This is an example using Oscil::phMod to produce vibrato using phase modulation.
*/

#endif /* OSCIL_H_ */
