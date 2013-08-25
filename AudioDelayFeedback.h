/*
 * AudioDelayFeedback.h
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
 
#ifndef AUDIODELAY_FEEDBACK_H_
#define AUDIODELAY_FEEDBACK_H_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "mozzi_utils.h"
#include "meta.h"

enum interpolation_types {LINEAR,ALLPASS};


/** Audio delay line with feedback for comb filter, flange, chorus and short echo effects.
@tparam NUM_BUFFER_SAMPLES is the length of the delay buffer in samples, and should be a
power of two. The maximum delay length which will fit in an atmega328 is half
that of a plain AudioDelay object, in this case 256 cells, or about 15
milliseconds. AudioDelayFeedback uses int sized cells to accomodate the higher
amplitude of direct input to the delay as well as the feedback, without losing
precision. Output is only the delay line signal. If you want to mix the delay
with the input, do it in your sketch. AudioDelayFeedback uses more processing and memory
than a plain AudioDelay, but allows for more dramatic effects with feedback.
@tparam INTERP_TYPE a choice of LINEAR (default) or ALLPASS interpolation.  LINEAR is better
for sweeping delay times, ALLPASS may be better for reverb-like effects.
*/
template <unsigned int NUM_BUFFER_SAMPLES, char INTERP_TYPE = LINEAR>
class AudioDelayFeedback
{

public:
	/** Constructor.
	*/
	AudioDelayFeedback(): write_pos(0), _feedback_level(0), _delaytime_cells(0)
	{}


	/** Constructor.
	@param delaytime_cells delay time expressed in cells.  
	For example, 128 cells delay at AUDIO_RATE 16384 would produce a time delay of 128/16384 = 0.0078125 s = 7.8 ms
	Put another way, num_cells = delay_seconds * AUDIO_RATE.
	*/
	AudioDelayFeedback(unsigned int delaytime_cells): write_pos(0), _feedback_level(0), _delaytime_cells(delaytime_cells)
	{}


	/** Constructor.
	@param delaytime_cells delay time expressed in cells.  
	For example, 128 cells delay at AUDIO_RATE 16384 would produce a time delay of 128/16384 = 0.0078125 s = 7.8 ms
	Put another way, num_cells = delay_seconds * AUDIO_RATE.
	@param feedback_level is the feedback level from -128 to 127 (representing -1 to 1).
	*/
	AudioDelayFeedback(unsigned int delaytime_cells, char feedback_level): write_pos(0),  _feedback_level(feedback_level), _delaytime_cells(delaytime_cells)
	{}



	/** Input a value to the delay and retrieve the signal in the delay line at the position delaytime_cells.
	@param input the signal input.
	@note slower than next(char input, unsigned int delaytime_cells)
	*/
	inline
	int next(char input)
	{
		// chooses a different next() function depending on whether the
		// the template parameter is LINEAR(default if none provided) or ALLPASS.
		// See meta.h.
		return next(input, Int2Type<INTERP_TYPE>());
	}



	/**  Input a value to the delay, retrieve the signal in the delay line at the position delaytime_cells, and add feedback from the output to the input.
	@param input the signal input.
	@param delaytime_cells indicates the delay time in terms of cells in the delay buffer.
	It doesn't change the stored internal value of _delaytime_cells.
	@note Timing: 4us
	*/
	inline
	int next(char input, unsigned int delaytime_cells)
	{
		//setPin13High();
		++write_pos &= (NUM_BUFFER_SAMPLES - 1);
		unsigned int read_pos = (write_pos - delaytime_cells) & (NUM_BUFFER_SAMPLES - 1);
		// < 1us to here
		int delay_sig = delay_array[read_pos];								// read the delay buffer
		// with this line, the method takes 18us
		//char feedback_sig = (char) min(max(((delay_sig * _feedback_level)/128),-128),127); // feedback clipped
		// this line, the whole method takes 4us... Compiler doesn't optimise pow2 divides.  Why?
		char feedback_sig = (char) min(max(((delay_sig * _feedback_level)>>7),-128),127); // feedback clipped
		delay_array[write_pos] = (int) input + feedback_sig;					// write to buffer
		//setPin13Low();
		return delay_sig;
	}



	/** Input a value to the delay, retrieve the signal in the delay line at the interpolated fractional position delaytime_cells, and add feedback from the output to the input.
	@param input the signal input.
	@param delaytime_cells is a fractional number to set the delay time in terms of cells
	or partial cells in the delay buffer. It doesn't change the stored internal
	value of _delaytime_cells.
	*/
	inline
	int next(char input, Q16n16 delaytime_cells)
	{
		//setPin13High();
		++write_pos &= (NUM_BUFFER_SAMPLES - 1);
		unsigned int index = delaytime_cells >> 16;
		unsigned int fraction = (unsigned int) delaytime_cells; // keeps low word

		unsigned int read_pos1 = (write_pos - index) & (NUM_BUFFER_SAMPLES - 1);
		int delay_sig1 = delay_array[read_pos1];								// read the delay buffer

		unsigned int read_pos2 = (write_pos - (index+1)) & (NUM_BUFFER_SAMPLES - 1);
		int delay_sig2 = delay_array[read_pos2];								// read the delay buffer


		int difference = delay_sig2 - delay_sig1;
		int delay_sig_fraction = ((long) fraction * difference) >> 16;

		int delay_sig = delay_sig1+delay_sig_fraction;

		//int delay_sig = delay_sig1 + ((long)delay_sig2*fraction)>>16;

		char feedback_sig = (char) min(max(((delay_sig * _feedback_level)>>7),-128),127); // feedback clipped
		delay_array[write_pos] = (int) input + feedback_sig;					// write to buffer
		//setPin13Low();
		return delay_sig;
	}


	/** Input a value to the delay but don't change the delay time or retrieve the output signal.
	@param input the signal input.
	*/
	inline
	void write(char input)
	{
		++write_pos &= (NUM_BUFFER_SAMPLES - 1);
		delay_array[write_pos] = input;
	}


	/** Input a value to the delay but don't advance the write position, change the delay time or retrieve the output signal.
	This can be useful for manually adding feedback to the delay line, "behind" the advancing write head.
	@param input the signal input.
	*/
	inline
	void writeFeedback(char input)
	{
		delay_array[write_pos] = input;
	}


	/** Input a value to the delay at an offset from the current write position.  Don't advance the main
	write position or change the stored delay time or retrieve the output signal.
	@param input the signal input.
	@param offset the number of cells behind the ordinary write position where the input will be written.
	*/
	inline
	void write(char input, unsigned int offset)
	{
		(write_pos + offset) &= (NUM_BUFFER_SAMPLES - 1);
		delay_array[write_pos] = input;
	}


	/** Retrieve the signal in the delay line at the interpolated fractional position delaytime_cells.
	It doesn't change the stored internal value of _delaytime_cells or feedback the output to the input.
	@param delaytime_cells indicates the delay time in terms of cells in the delay buffer.
	*/
	inline
	int read(Q16n16 delaytime_cells)
	{
		return read(delaytime_cells, Int2Type<INTERP_TYPE>());
	}


	/** Retrieve the signal in the delay line at the current stored delaytime_cells.
	It doesn't change the stored internal value of _delaytime_cells or feedback the output to the input.
	*/
	inline
	int read()
	{
		return read(Int2Type<INTERP_TYPE>());
	}


	/**  Set delay time expressed in samples.
	@param delaytime_cells delay time expressed in cells, with each cell played per tick of AUDIO_RATE.  
	For example, 128 cells delay at AUDIO_RATE would produce a time delay of 128/16384 = 0.0078125 s = 7.8 ms
	Put another way, num_cells = delay_seconds * AUDIO_RATE.
	*/
	inline
	void setDelayTimeCells(unsigned int delaytime_cells)
	{
		_delaytime_cells = (unsigned int) delaytime_cells;
	}


	/**  Set delay time expressed in samples, fractional Q16n16 for an interpolating delay.
	@param delaytime_cells delay time expressed in cells, with each cell played per tick of AUDIO_RATE.  
	For example, 128 cells delay at AUDIO_RATE would produce a time delay of 128/16384 = 0.0078125 s = 7.8 ms
	Put another way, num_cells = delay_seconds * AUDIO_RATE.
	*/
	inline
	void setDelayTimeCells(Q16n16 delaytime_cells)
	{
		return setDelayTimeCells(delaytime_cells, Int2Type<INTERP_TYPE>());
	}


	/**  Set delay time expressed in samples, fractional float for an interpolating delay.
	@param delaytime_cells delay time expressed in cells, with each cell played per tick of AUDIO_RATE.  
	For example, 128 cells delay at AUDIO_RATE would produce a time delay of 128/16384 = 0.0078125 s = 7.8 ms
	Put another way, num_cells = delay_seconds * AUDIO_RATE.
	*/
	inline
	void setDelayTimeCells(float delaytime_cells)
	{
		return setDelayTimeCells(delaytime_cells, Int2Type<INTERP_TYPE>());
	}


	/**  Set the feedback gain.
	@param feedback_level is the feedback level from -128 to 127 (representing -1 to 1).
	*/
	inline
	void setFeedbackLevel(char feedback_level)
	{
		_feedback_level = feedback_level;
	}



private:
	int delay_array[NUM_BUFFER_SAMPLES];
	unsigned int write_pos;
	char _feedback_level;
	unsigned int _delaytime_cells;
	Q15n16 _coeff; // for allpass interpolation



	/** Input a value to the delay and retrieve the signal in the delay line at the position delaytime_cells.
	@param in_value the signal input.
	*/
	inline
	int next(char in_value, Int2Type<LINEAR>)
	{
		++write_pos &= (NUM_BUFFER_SAMPLES - 1);
		unsigned int read_pos = (write_pos - _delaytime_cells) & (NUM_BUFFER_SAMPLES - 1);

		int delay_sig = delay_array[read_pos];								// read the delay buffer
		char feedback_sig = (char) min(max(((delay_sig * _feedback_level)/128),-128),127); // feedback clipped
		delay_array[write_pos] = (int) in_value + feedback_sig;					// write to buffer

		return delay_sig;
	}



	/**  The delaytime_cells has to be set seperately, because it's slowish
	and in this implementation the allpass interpolation mode doesn't slide
	nicely from one delay time to another.
	@param input an audio signal in
	@return the delayed signal, including feedback
	@note Timing: 10us
	*/
	inline
	int next(char input, Int2Type<ALLPASS>)
	{
		/*
		http://www.scandalis.com/Jarrah/Documents/DelayLine.pdf
		also https://ccrma.stanford.edu/~jos/Interpolation/Interpolation_4up.pdf
		for desired fractional delay of d samples,
		coeff = (1-d)/(1+d)
		or
		coeff = ((d-1)>1) + (((d-1)*(d-1))>>2) - (((d-1)*(d-1)*(d-1))>>3)
		out = coeff * in + last_in - coeff * last_out
		= coeff * (in-last_out) + last_in
		*/
		//setPin13High();
		static char last_in;
		static int last_out;

		++write_pos &= (NUM_BUFFER_SAMPLES - 1);

		unsigned int read_pos1 = (write_pos - _delaytime_cells) & (NUM_BUFFER_SAMPLES - 1);
		int delay_sig = delay_array[read_pos1];								// read the delay buffer

		int interp = (int)(_coeff * ((int)input - last_out)>>16) + last_in; // Q15n16*Q15n0 + Q15n0 = Q15n16 + Q15n0 = Q15n16
		delay_sig += interp;

		char feedback_sig = (char) min(max(((delay_sig * _feedback_level)>>7),-128),127); // feedback clipped
		delay_array[write_pos] = (int) input + feedback_sig;					// write to buffer

		last_in = input;
		last_out = delay_sig;
		//setPin13Low();
		return delay_sig;
	}



	// 20-25us
	inline
	void setDelayTimeCells(Q16n16 delaytime_cells, Int2Type<ALLPASS>)
	{
		/*
		integer optimisation/approximation from 
		Van Duyne, Jaffe, Scandalis, Stilson 1997
		http://www.scandalis.com/Jarrah/Documents/DelayLine.pdf
		//coeff = -((d-1)>1) + (((d-1)*(d-1))>>2) - (((d-1)*(d-1)*(d-1))>>3) , d is fractional part
		*/
		_delaytime_cells = delaytime_cells>>16; // whole integer part
		Q15n16 dminus1 = - Q15n16_FIX1 + (unsigned int) delaytime_cells;
		Q15n16 dminus1squared = (dminus1)*(dminus1)>>16;
		_coeff = -(dminus1>>1) + (dminus1squared>>2) - (((dminus1squared*dminus1)>>16)>>3);
	}


	// 100us
	inline
	void setDelayTimeCells(float delaytime_cells, Int2Type<ALLPASS>)
	{
		//coeff = (1-d)/(1+d)
		_delaytime_cells = (unsigned int) delaytime_cells;

		float fraction = delaytime_cells - _delaytime_cells;

		// modified from stk DelayA.cpp
		float alpha_ = 1.0f + fraction; // fractional part
		if ( alpha_ < 0.5f ) {
			// (stk): The optimal range for alpha is about 0.5 - 1.5 in order to
			// achieve the flattest phase delay response.

			// something's not right about how I use _delaytime_cells and
			// NUM_BUFFER_SAMPLES etc. in my ringbuffer compared to stk
			_delaytime_cells += 1;
			if ( _delaytime_cells >= NUM_BUFFER_SAMPLES ) _delaytime_cells -= NUM_BUFFER_SAMPLES;
			alpha_ += 1.0f;
		}
		// otherwise this would use fraction instead of alpha
		_coeff = float_to_Q15n16((1.f-alpha_)/(1.f+alpha_));
	}

	//  Retrieve the signal in the delay line at the position delaytime_cells.
	// It doesn't change the stored internal value of _delaytime_cells or feedback the output to the input.
	// param delaytime_cells indicates the delay time in terms of cells in the delay buffer.
	// 
	// inline
	// int read(unsigned int delaytime_cells, Int2Type<LINEAR>)
	// {
	// unsigned int read_pos = (write_pos - delaytime_cells) & (NUM_BUFFER_SAMPLES - 1);
	// int delay_sig = delay_array[read_pos];								// read the delay buffer
	//
	// return delay_sig;
	// }

	/** Retrieve the signal in the delay line at the interpolated fractional position delaytime_cells.
	It doesn't change the stored internal value of _delaytime_cells or feedback the output to the input.
	@param delaytime_cells indicates the delay time in terms of cells in the delay buffer.
	*/
	inline
	int read(Q16n16 delaytime_cells, Int2Type<LINEAR>)
	{
		unsigned int index = delaytime_cells >> 16;
		unsigned int fraction = (unsigned int) delaytime_cells; // keeps low word

		unsigned int read_pos1 = (write_pos - index) & (NUM_BUFFER_SAMPLES - 1);
		int delay_sig1 = delay_array[read_pos1];								// read the delay buffer

		unsigned int read_pos2 = (write_pos - (index+1)) & (NUM_BUFFER_SAMPLES - 1);
		int delay_sig2 = delay_array[read_pos2];								// read the delay buffer

		/*
		int difference = delay_sig2 - delay_sig1;
		int delay_sig_fraction = ((long) fraction * difference) >> 16;

		int delay_sig = delay_sig1+delay_sig_fraction;
		*/
		int delay_sig = delay_sig1 + ((long)delay_sig2*fraction)>>16;

		return delay_sig;
	}

	
};

/**
@example 09.Delays/AudioDelayFeedback/AudioDelayFeedback.ino
This is an example of how to use the AudioDelayFeedback class.
*/

#endif        //  #ifndef AUDIODELAY_FEEDBACK_H_
