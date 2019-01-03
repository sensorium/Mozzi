/*
 * Sample16.h
 *
 * Oscil.h owes much to AF_precision_synthesis.pde, 2009, Adrian Freed.
 *
 * Copyright 2012 Tim Barrass, 2009 Adrian Freed.
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */

#ifndef SAMPLE16_H_
#define SAMPLE16_H_

#include <MozziGuts.h>
#include <mozzi_fixmath.h>
#include <mozzi_pgmspace.h>

// fractional bits for sample index precision
#define SAMPLE_F_BITS 16
#define SAMPLE_F_BITS_AS_MULTIPLIER 65536

// phmod_proportion is an 1n15 fixed-point number only using
// the fractional part and the sign bit
#define SAMPLE_PHMOD_BITS 16

enum interpolation {INTERP_NONE, INTERP_LINEAR};

/** Sample is like Oscil, it plays a wavetable.  However, Sample can be
set to play once through only, with variable start and end points,
or can loop, also with variable start and end points.
It defaults to playing once through the whole sound table, from start to finish.
@tparam NUM_TABLE_CELLS This is defined in the table ".h" file the Sample will be
using.  The sound table can be arbitrary length for Sample.
It's important that NUM_TABLE_CELLS is either a literal number (eg. "8192") or a
defined macro, rather than a const or int, for the Sample to run fast enough.
@tparam UPDATE_RATE This will be AUDIO_RATE if the Sample is updated in
updateAudio(), or CONTROL_RATE if it's updated each time updateControl() is
called. It could also be a fraction of CONTROL_RATE if you are doing some kind
of cyclic updating in updateControl(), for example, to spread out the processor load.
@section int8_t2mozzi
Converting soundfiles for Mozzi.
There is a python script called int8_t2mozzi.py in the Mozzi/python folder.
The script converts raw sound data saved from a program like Audacity.
Instructions are in the int8_t2mozzi.py file.
*/
template <unsigned int NUM_TABLE_CELLS, unsigned int UPDATE_RATE, uint8_t INTERP=INTERP_NONE>
class Sample16
{

public:

	/** Constructor.
	@param TABLE_NAME the name of the array the Sample will be using. This
	can be found in the table ".h" file if you are using a table made for
	Mozzi by the int8_t2mozzi.py python script in Mozzi's python
	folder.  Sound tables can be of arbitrary lengths for Sample().
	*/
	Sample16(const int16_t * TABLE_NAME):table(TABLE_NAME),endpos_fractional((unsigned long) NUM_TABLE_CELLS << SAMPLE_F_BITS) // so isPlaying() will work
	{
		setLoopingOff();
		//rangeWholeSample();
	}



	/** Constructor.
	Declare a Sample with template TABLE_NUM_CELLS and UPDATE_RATE parameters, without specifying a particular wave table for it to play.
	The table can be set or changed on the fly with setTable().
	*/
	Sample16():endpos_fractional(4294967295UL) // biggest UL number so isPlaying() will work
	{
		setLoopingOff();
		//rangeWholeSample();
	}


	/** Change the sound table which will be played by the Sample.
	@param TABLE_NAME is the name of the array in the table ".h" file you're using.
	*/
	inline
	void setTable(const int16_t * TABLE_NAME)
	{
		table = TABLE_NAME;
	}


	/** Sets the starting position in samples.
	@param startpos offset position in samples.
	*/
	inline
	void setStart(unsigned int startpos)
	{
		startpos_fractional = (unsigned long) startpos << SAMPLE_F_BITS;
	}


	/** Resets the phase (the playhead) to the start position, which will be 0 unless set to another value with setStart();
	*/
	inline
	void start()
	{
		phase_fractional = startpos_fractional;
	}


	/** Sets a new start position plays the sample from that position.
	@param startpos position in samples from the beginning of the sound. 
	*/
	inline
	void start(unsigned int startpos)
	{
		setStart(startpos);
		start();
	}


	/** Sets the end position in samples from the beginning of the sound.
	@param end position in samples.
	*/
	inline
	void setEnd(unsigned int end)
	{
		endpos_fractional = (unsigned long) end << SAMPLE_F_BITS;
	}


	/** Sets the start and end points to include the range of the whole sound table.
	*/
	inline
	void rangeWholeSample()
	{
		startpos_fractional = 0;
		endpos_fractional = (unsigned long) NUM_TABLE_CELLS << SAMPLE_F_BITS;
	}


	/** Turns looping on.
	*/
	inline
	void setLoopingOn()
	{
		looping=true;
	}


	/** Turns looping off.
	*/
	inline
	void setLoopingOff()
	{
		looping=false;
	}



	/**
	Returns the sample at the current phase position, or 0 if looping is off
	and the phase overshoots the end of the sample. Updates the phase
	according to the current frequency.
	@return the next sample value from the table, or 0 if it's finished playing.
	@todo in next(), incrementPhase() happens in a different position than for Oscil - check if it can be standardised
	*/
	inline
	int16_t next() { // 4us
		
		if (phase_fractional>endpos_fractional){
			if (looping) {
				phase_fractional = startpos_fractional + (phase_fractional - endpos_fractional);
			}else{
				return 0;
			}
		}
		int16_t out;
		if(INTERP==INTERP_LINEAR){
			// WARNNG this is hard coded for when SAMPLE_F_BITS is 16
			unsigned int index = phase_fractional >> SAMPLE_F_BITS;
			out = FLASH_OR_RAM_READ<const int16_t>(table + index);
			int16_t difference = FLASH_OR_RAM_READ<const int16_t>((table + 1) + index) - out;
			int16_t diff_fraction = (int16_t)(((((unsigned int) phase_fractional)>>8)*difference)>>8); // (unsigned int) phase_fractional keeps low word, then>> for only 8 bit precision
			out += diff_fraction;
		}else{
			out = FLASH_OR_RAM_READ<const int16_t>(table + (phase_fractional >> SAMPLE_F_BITS));
		}
		incrementPhase();
		return out;
	}


	/** Checks if the sample is playing by seeing if the phase is within the limits of its end position.
	@return true if the sample is playing
	*/
	inline
	boolean isPlaying(){
		return phase_fractional<endpos_fractional;
	}


	// Not readjusted for arbitrary table length yet
	//
	//  Returns the next sample given a phase modulation value.
	// @param phmod_proportion phase modulation value given as a proportion of the wave. The
	// phmod_proportion parameter is a Q15n16 fixed-point number where to fractional
	// n16 part represents -1 to 1, modulating the phase by one whole table length in
	// each direction.
	// @return a sample from the table.
	// 
	// inline
	// int8_t phMod(long phmod_proportion)
	// {
	// 	incrementPhase();
	// 	return FLASH_OR_RAM_READ<const int8_t>(table + (((phase_fractional+(phmod_proportion * NUM_TABLE_CELLS))>>SAMPLE_SAMPLE_F_BITS) & (NUM_TABLE_CELLS - 1)));
	// }



	/** Set the oscillator frequency with an unsigned int. 
	This is faster than using a float, so it's useful when processor time is tight,
	but it can be tricky with low and high frequencies, depending on the size of the
	wavetable being used. If you're not getting the results you expect, try
	explicitly using a float, or try setFreq_Q24n8.
	@param frequency to play the wave table.
	*/
	inline
	void setFreq (int frequency) {
		phase_increment_fractional = ((((unsigned long)NUM_TABLE_CELLS<<ADJUST_FOR_NUM_TABLE_CELLS)*frequency)/UPDATE_RATE) << (SAMPLE_F_BITS - ADJUST_FOR_NUM_TABLE_CELLS);
	}


	/** Set the sample frequency with a float. Using a float is the most reliable
	way to set frequencies, -Might- be slower than using an int but you need either
	this or setFreq_Q24n8 for fractional frequencies.
	@param frequency to play the wave table.
	*/
	inline
	void setFreq(float frequency)
	{ // 1 us - using float doesn't seem to incur measurable overhead with the oscilloscope
		phase_increment_fractional = (unsigned long)((((float)NUM_TABLE_CELLS * frequency)/UPDATE_RATE) * SAMPLE_F_BITS_AS_MULTIPLIER);
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
	void setFreq_Q24n8(Q24n8 frequency)
	{
		//phase_increment_fractional = (frequency* (NUM_TABLE_CELLS>>3)/(UPDATE_RATE>>6)) << (F_BITS-(8-3+6));
		phase_increment_fractional = (((((unsigned long)NUM_TABLE_CELLS<<ADJUST_FOR_NUM_TABLE_CELLS)>>3)*frequency)/(UPDATE_RATE>>6))
			                             << (SAMPLE_F_BITS - ADJUST_FOR_NUM_TABLE_CELLS - (8-3+6));
	}


	/**  Returns the sample at the given table index.
	@param index between 0 and the table size.
	@return the sample at the given table index.
	*/
	inline
	int16_t atIndex(unsigned int index)
	{
		return FLASH_OR_RAM_READ<const int16_t>(table + index);
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
		phase_increment_fractional = phaseinc_fractional;
	}


private:


	/** Used for shift arithmetic in setFreq() and its variations.
	*/
static const uint8_t ADJUST_FOR_NUM_TABLE_CELLS = (NUM_TABLE_CELLS<2048) ? 8 : 0;


	/** Increments the phase of the oscillator without returning a sample.
	 */
	inline
	void incrementPhase()
	{
		phase_fractional += phase_increment_fractional;
	}


	volatile unsigned long phase_fractional;
	volatile unsigned long phase_increment_fractional;
	const int16_t * table;
	bool looping;
	unsigned long startpos_fractional, endpos_fractional;
};


/**
@example 08.Samples/Sample/Sample.ino
This example demonstrates the Sample class.
*/

#endif /* SAMPLE16_H_ */
