/*
 * AudioDelay.h
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

#ifndef AUDIODELAY_H_
#define AUDIODELAY_H_


/** Audio delay line for comb filter, flange, chorus and short echo effects.
@tparam  NUM_BUFFER_SAMPLES is the length of the delay buffer in samples.  This should
be a power of two. The largest delay you'll fit in an atmega328 will be 512
cells, which at 16384 Hz sample rate is 31 milliseconds. More of a flanger or a
doubler than an echo. This version doesn't have feedback, for that use
AudioDelayFeedback instead.
*/

template <unsigned int NUM_BUFFER_SAMPLES>
class AudioDelay
{

private:

	char delay_array[NUM_BUFFER_SAMPLES];
	unsigned int write_pos;

public:

	/** Constructor.
	*/
	AudioDelay(): write_pos(0)
	{}


	/** Input a value to the delay and retrieve the signal in the delay line at the position delay_cells.
	@param in_value the signal input.
	@param delay_cells sets the delay time in terms of cells in the delay buffer.
	*/
	inline
	char next(char in_value, unsigned int delay_cells)
	{
		++write_pos &= (NUM_BUFFER_SAMPLES - 1);
		unsigned int read_pos = (write_pos - delay_cells) & (NUM_BUFFER_SAMPLES - 1);

		// why does delay jump if I read it before writing?
		delay_array[write_pos] = in_value;			// write to buffer
		char delay_sig = delay_array[read_pos] ;	// read the delay buffer

		return (char)(((int) in_value + delay_sig)>>1);
	}

};

#endif        //  #ifndef AUDIODELAY_H_

