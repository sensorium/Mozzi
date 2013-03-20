#ifndef AUDIODELAY_FEEDBACK_H_
#define AUDIODELAY_FEEDBACK_H_

#include "Arduino.h"
#include "mozzi_utils.h"

/** Audio delay line with feedback for comb filter, flange, chorus and short echo effects.
@tparam NUM_BUFFER_SAMPLES is the length of the delay buffer in samples, and should be a
power of two. The maximum delay length which will fit in an atmega328 is half
that of a plain AudioDelay object, in this case 256 cells, or about 15
milliseconds. AudioDelayFeedback uses int sized cells to accomodate the higher
amplitude of direct input to the delay as well as the feedback, without losing
precision. Output is only the delay line signal. If you want to mix the delay
with the input, do it in your sketch. AudioDelayFeedback uses more processing
than a plain AudioDelay, but allows for more dramatic effects with feedback.
@todo sub-sample delay times, just using linear interpolation of adacent cells
*/

template <unsigned int NUM_BUFFER_SAMPLES>
class AudioDelayFeedback
{

private:
	int delay_array[NUM_BUFFER_SAMPLES];
	unsigned int write_pos;
	char _feedback_level;
	unsigned int _delaytime_cells;

public:
	/** Constructor.
	*/
	AudioDelayFeedback(): write_pos(0), _delaytime_cells(0), _feedback_level(0)
	{}
	
	
	/** Constructor.
	@param delaytime_cells delay time expressed in cells.  
	For example, 128 cells delay at AUDIO_RATE would produce a time delay of 128/16384 = 0.0078125 s = 7.8 ms
	Put another way, num_cells = delay_seconds * AUDIO_RATE.
	*/
	AudioDelayFeedback(unsigned int delaytime_cells): write_pos(0), _feedback_level(0), _delaytime_cells(delaytime_cells)
	{}

	
	/** Constructor.
	@param delaytime_cells delay time expressed in cells.  
	For example, 128 cells delay at AUDIO_RATE would produce a time delay of 128/16384 = 0.0078125 s = 7.8 ms
	Put another way, num_cells = delay_seconds * AUDIO_RATE.
	@param feedback_level is the feedback level from -128 to 127 (representing -1 to 1).
	*/
	AudioDelayFeedback(unsigned int delaytime_cells, char feedback_level): write_pos(0), _delaytime_cells(delaytime_cells), _feedback_level(feedback_level)
	{}
	
	
	
	/** Input a value to the delay and retrieve the signal in the delay line at the position delaytime_cells.
	@param in_value the signal input.
	*/
	inline
	int next(char in_value)
	{
		++write_pos &= (NUM_BUFFER_SAMPLES - 1);
		unsigned int read_pos = (write_pos - _delaytime_cells) & (NUM_BUFFER_SAMPLES - 1);

		int delay_sig = delay_array[read_pos];								// read the delay buffer
		char feedback_sig = (char) min(max(((delay_sig * _feedback_level)/128),-128),127); // feedback clipped
		delay_array[write_pos] = (int) in_value + feedback_sig;					// write to buffer

		return delay_sig;
	}
	
	
	
	/** Input a value to the delay and retrieve the signal in the delay line at the position delaytime_cells,
	updating all positions and feedback from the output to the input.  
	This is the simplest way to use AudioDelayFeedback(), like an effects box with no extra fancy holes or controls.
	@param in_value the signal input.
	@param delaytime_cells indicates the delay time in terms of cells in the delay buffer.
	It doesn't change the stored internal value of _delaytime_cells.
	*/
	// 4us 

	inline
	int next(char in_value, unsigned int delaytime_cells)
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
		delay_array[write_pos] = (int) in_value + feedback_sig;					// write to buffer
		//setPin13Low();
		return delay_sig;
	}

	
	/** Input a value to the delay but don't change the read position or retrieve the output signal.
	@param in_value the signal input.
	*/
	inline
	void write(char in_value)
	{
		++write_pos &= (NUM_BUFFER_SAMPLES - 1);
	}
	
	
	/** Retrieve the signal in the delay line at the position delaytime_cells.
	It doesn't change the stored internal value of _delaytime_cells or feedback the output to the input.
	@param delaytime_cells indicates the delay time in terms of cells in the delay buffer.
	*/
	inline
	int read(unsigned int delaytime_cells)
	{
		unsigned int read_pos = (write_pos - delaytime_cells) & (NUM_BUFFER_SAMPLES - 1);
		int delay_sig = delay_array[read_pos];								// read the delay buffer

		return delay_sig;
	}
	
	

	/** Retrieve the signal in the delay line at the current delaytime_cells.
	It doesn't change the stored internal value of _delaytime_cells or feedback the output to the input.
	*/
	inline
	int read()
	{
		unsigned int read_pos = (write_pos - _delaytime_cells) & (NUM_BUFFER_SAMPLES - 1);
		int delay_sig = delay_array[read_pos];								// read the delay buffer

		return delay_sig;
	}
	

	
	/**  Set delay time expressed in samples.
	@param delaytime_cells delay time expressed in cells, with each cell played per tick of AUDIO_RATE.  
	For example, 128 cells delay at AUDIO_RATE would produce a time delay of 128/16384 = 0.0078125 s = 7.8 ms
	Put another way, num_cells = delay_seconds * AUDIO_RATE.
	*/
	inline
	void setDelayTimeCells(unsigned int delaytime_cells)
	{
		_delaytime_cells = delaytime_cells;
	}
	
	
	/**  Set the feedback gain.
	@param feedback_level is the feedback level from -128 to 127 (representing -1 to 1).
	*/
	inline
	void setFeedbackLevel(char feedback_level)
	{
		_feedback_level = feedback_level;
	}

};

#endif        //  #ifndef AUDIODELAY_FEEDBACK_H_

