#ifndef DELAY_H_
#define DELAY_H_


/** A delay line, with a variable delay up to NUM_CELLS long, NUM_CELLS should
be a power of two. The largest delay you'll fit in an atmega328 will be 512
cells, which at 16384 Hz sample rate is 31 milliseconds. More of a flanger or a
doubler than an echo. This version doesn't have feedback, for that use
DelayFeedback instead. */

template <unsigned int NUM_CELLS>
class Delay
{

private:
	char delay_array[NUM_CELLS];
	unsigned int write_pos;

public:
	Delay(): write_pos(0)
	{}

	/** Input a value to the delay and retrieve the signal in the delay line at the position delay_cells.
	*/
	inline
	char next(char in_value, unsigned int delay_cells)
	{
		++write_pos &= (NUM_CELLS - 1);
		unsigned int read_pos = (write_pos - delay_cells) & (NUM_CELLS - 1);

		// why does delay jump if I read it before writing?
		delay_array[write_pos] = in_value;			// write to buffer
		char delay_sig = delay_array[read_pos] ;	// read the delay buffer

		return (char)(((int) in_value + delay_sig)>>1);
	}

};

#endif        //  #ifndef DELAY_H_

