#ifndef DELAY_FEEDBACK_H_
#define DELAY_FEEDBACK_H_

#include "Arduino.h"

/** A delay line, with a variable delay up to NUM_CELLS long and feedback,
NUM_CELLS should be a power of two. The delay length is half that of a plain
Delay object, in this case 256 cells, or about 15 milliseconds. DelayFeedback
uses ints to accomodate the direct input to the delay as well as the feedback,
without losing precision. Output is only the delay line signal. If you want to
mix the delay with the input, do it in your sketch.*/

template <unsigned int NUM_CELLS>
class DelayFeedback
{

private:
	int delay_array[NUM_CELLS];
	unsigned int write_pos;
	char feedback_level;

public:
	DelayFeedback(): write_pos(0), feedback_level(0)
	{}

	/** Input a value to the delay and retrieve the signal in the delay line at the position delay_cells.
	*/
	inline
	int next(char in_value, unsigned int delay_cells)
	{
		++write_pos &= (NUM_CELLS - 1);
		unsigned int read_pos = (write_pos - delay_cells) & (NUM_CELLS - 1);

		int delay_sig = delay_array[read_pos];								// read the delay buffer
		char feedback_sig = (char) min(max(((delay_sig * feedback_level)/128),-128),127); // feedback clipped
		delay_array[write_pos] = (int) in_value + feedback_sig;					// write to buffer

		return delay_sig;
	}

	inline
	void setFeedbackLevel(char fbl)
	{
		feedback_level = fbl;
	}

};

#endif        //  #ifndef DELAY_FEEDBACK_H_

