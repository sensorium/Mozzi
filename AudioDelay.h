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
doubler than an echo. The amount of memory available for delays on other chips will vary.
AudioDelay() doesn't have feedback.  If you want feedback, use AudioDelayFeedback().
@tparam the type of numbers to use for the signal in the delay.  The default is char, but int could be useful
when adding manual feedback.  When using int, the input should be limited to 15 bits width, ie. -16384 to 16383.
*/

template <unsigned int NUM_BUFFER_SAMPLES, class T = char>
class AudioDelay
{

private:

	T delay_array[NUM_BUFFER_SAMPLES];
	unsigned int _write_pos;
	unsigned int _delaytime_cells;
	
public:

	/** Constructor.
	*/
	AudioDelay(): _write_pos(0)
	{}
	
	
	/** Constructor.
	@param delaytime_cells delay time expressed in cells.  
	For example, 128 cells delay at AUDIO_RATE would produce a time delay of 128/16384 = 0.0078125 s = 7.8 ms
	Put another way, num_cells = delay_seconds * AUDIO_RATE.
	*/
	AudioDelay(unsigned int delaytime_cells): _write_pos(0), _delaytime_cells(delaytime_cells)
	{}

	

	/** Input a value to the delay and retrieve the signal in the delay line at the position delaytime_cells.
	@param in_value the signal input.
	@param delaytime_cells sets the delay time in terms of cells in the delay buffer.
	*/
	inline
	T next(T in_value, unsigned int delaytime_cells)
	{
		++_write_pos &= (NUM_BUFFER_SAMPLES - 1);
		unsigned int read_pos = (_write_pos - delaytime_cells) & (NUM_BUFFER_SAMPLES - 1);

		// why does delay jump if I read it before writing?
		delay_array[_write_pos] = in_value;			// write to buffer
		char delay_sig = delay_array[read_pos] ;	// read the delay buffer

		return (T)delay_sig;
	}
	
	
	
	/** Input a value to the delay and retrieve the signal in the delay line at the position delaytime_cells.
	@param in_value the signal input.
	*/
	inline
	T next(T in_value)
	{
		++_write_pos &= (NUM_BUFFER_SAMPLES - 1);
		unsigned int read_pos = (_write_pos - _delaytime_cells) & (NUM_BUFFER_SAMPLES - 1);

		// why does delay jump if I read it before writing?
		delay_array[_write_pos] = in_value;			// write to buffer
		T delay_sig = delay_array[read_pos] ;	// read the delay buffer

		return delay_sig;
	}

	
	/** Set the delay time, measured in cells.
	@param delaytime_cells how many cells to delay the input signal by.
	*/
	inline
	void set(unsigned int delaytime_cells){
		_delaytime_cells = delaytime_cells;
	}

	
	/** Retrieve the signal in the delay line at the position delaytime_cells.
	It doesn't change the stored internal value of _delaytime_cells.
	@param delaytime_cells indicates the delay time in terms of cells in the delay buffer.
	*/
	inline
	T read(unsigned int delaytime_cells)
	{
		unsigned int read_pos = (_write_pos - delaytime_cells) & (NUM_BUFFER_SAMPLES - 1);
		return delay_array[read_pos];
	}
	
	
	// /** Input a value to the delay but don't advance the write position, change the delay time or retrieve the output signal.
	// This can be useful for manually adding feedback to the delay line, "behind" the advancing write head.
	// @param input the signal input.
	// */
	// inline
	// void writeFeedback(T input)
	// {
		// delay_array[_write_pos] = input;
	// }

};

/**
@example 09.Delays/AudioDelay/AudioDelay.ino
This is an example of how to use the AudioDelay class.
*/

#endif        //  #ifndef AUDIODELAY_H_

