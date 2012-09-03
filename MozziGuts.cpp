/*
 * MozziGuts.cpp
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


#include "MozziGuts.h"
#include <util/atomic.h>
#include "utils.h"

// Interrupts:
// Timer2 has highest priority (on a 328), but you can't have a flexible interrupt rate as well as pwm.
// The TimerOne library is more flexible and allows both pwm and interrupts,
// by using ICR1 for interrupt rate and OCR1A (pin 9) (or OCR1B pin 10) for pwm level
// So we use Timer1 for audio.
// Ideally we'd use Timer0 for control because it has lowest priority, but Arduino uses
// it for time and delay functions, so we use Timer 2 for control and hope it won't
// interfere with the audio interrupt.
// TODO: Timer0 is messed up anyway, it seems, so maybe it's better to
// use it for control and leave Timer2 available for other things?


/** @ingroup core
Sets up the timers for audio and control rate processes. It goes in your
sketch's setup() routine. startMozzi() starts audio interrupts on Timer 1
and control interrupts on Timer 2. The audio rate is currently fixed at 16384
Hz.
@param control_rate_hz Sets how often updateControl() is called. It can be any
power of 2 above and including 64. The practical upper limit for control rate
depends on how busy the processor is, and you might need to do some tests to
find the best setting. It's good to define CONTROL_RATE in your
sketches (eg. "#define CONTROL_RATE 128") because the literal numeric value is
necessary for Oscils to work properly, and it also helps to keep the
calculations in your sketch clear.
 */
void startMozzi(unsigned int control_rate_hz)
{

	// audio
	pinMode(AUDIO_CHANNEL_1_PIN, OUTPUT);	// set pin to output for audio
	Timer1.initialize(1000000UL/AUDIO_RATE);		// set period
	Timer1.pwm(AUDIO_CHANNEL_1_PIN, AUDIO_BIAS);		// pwm pin, 50% of Mozzi's duty cycle, ie. 0 signal
	Timer1.attachInterrupt(outputAudio);			// call outputAudio() on each interrupt

	// control
	TimerZero::init(1000000/control_rate_hz,updateControl); // set period, attach updateControl()
	TimerZero::start();

	//MozziTimer2::set(1000000/control_rate_hz,updateControl); // set period, attach updateControl()
	//MozziTimer2::start();

	// FrequencyTimer2::setPeriod(2000000UL/control_rate_hz); // would use 1000000, but FrequencyTimer2 sets the period as half
	// FrequencyTimer2::setOnOverflow(updateControl);
	// FrequencyTimer2::enable();

}


// ring buffer for audio output
#define BUFFER_NUM_CELLS 256
static int output_buffer[BUFFER_NUM_CELLS];
static volatile unsigned int num_out; // shared by audioHook() (in loop()), and outputAudio() (in audio interrupt), where it is changed


/** @ingroup core
This is required in Arduino's loop().
It calls updateAudio() and puts the result into Mozzi's output buffer.
*/
void audioHook()
{

	static unsigned char in_pos = 0;
	static unsigned int num_in = 0;
	unsigned int gap;
	// Upon entering the block the Global Interrupt Status flag in SREG is disabled, and re-enabled upon exiting the block
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		// Atomic access to 16 bit num_out, which is changed in outputAudio(), called in the audio interrupt.
		// It's atomic because we don't want num_out to change half-way through reading it.
		gap = num_in - num_out; // wraps to a big number if it's negative
	}
	if (gap < BUFFER_NUM_CELLS)
	{
		in_pos++;
		num_in++;
		output_buffer[in_pos] = updateAudio() + AUDIO_BIAS;
	}

}


/* This is the callback routine attached to the Timer1 audio interrupt.
It moves sound data from the output buffer to the Arduino output register,
running at AUDIO_RATE.
*/
inline
void outputAudio()
{ // takes 1-2 us, and doesn't appear to get interrupted by control on scope
	//SET_PIN13_HIGH;
	static unsigned char out_pos;
	out_pos++;
	num_out++;
	AUDIO_CHANNEL_1_OUTPUT_REGISTER =  output_buffer[out_pos];
	//SET_PIN13_LOW;
}

// Unmodified TimerOne.cpp has TIMER3_OVF_vect.
// Watch out if you update the library file.
// The symptom will be no sound.
// ISR(TIMER1_OVF_vect)
// {
// 	Timer1.isrCallback();
// }


