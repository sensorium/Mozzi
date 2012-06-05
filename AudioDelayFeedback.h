#ifndef AUDIODELAY_FEEDBACK_H_
#define AUDIODELAY_FEEDBACK_H_

#include "Arduino.h"

/** An audio delay line with feedback for comb filter, flange, chorus and short echo effects.
NUM_BUFFER_SAMPLES is the length of the delay buffer in samples, and should be a power
of two. The delay length is half that of a plain AudioDelay object, in this case
256 cells, or about 15 milliseconds. AudioDelayFeedback uses int sized cells to
accomodate the higher amplitude of direct input to the delay as well as the
feedback, without losing precision. Output is only the delay line signal. If you
want to mix the delay with the input, do it in your sketch.  AudioDelayFeedback
uses more processing than a plain AudioDelay, but allows for more dramatic
effects with feedback.
*/

template <unsigned int NUM_BUFFER_SAMPLES>
class AudioDelayFeedback
{

private:
	int delay_array[NUM_BUFFER_SAMPLES];
	unsigned int write_pos;
	char feedback_level;

public:
	/** Constructor.  Doesn't take any parameters itself, but needs the delay length in samples to
	be set in the template parameter, for example:
	AudioDelayFeedback <128> myAudioDelay();
	*/
	AudioDelayFeedback(): write_pos(0), feedback_level(0)
	{}

	/** Input a value to the delay and retrieve the signal in the delay line at the position delay_cells.
	*/
	inline
	int next(char in_value, unsigned int delay_cells)
	{
		++write_pos &= (NUM_BUFFER_SAMPLES - 1);
		unsigned int read_pos = (write_pos - delay_cells) & (NUM_BUFFER_SAMPLES - 1);

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

#endif        //  #ifndef AUDIODELAY_FEEDBACK_H_

