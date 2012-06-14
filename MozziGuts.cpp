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

// Interrupts:
// Timer2 has highest priority, but you can't have a flexible interrupt rate as well as pwm.
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
find the best setting. Its' strongly recommended to define CONTROL_RATE in your
sketches (eg. "#define CONTROL_RATE 128") because the literal numeric value is
necessary for Oscils to work properly, and it also helps to keep your
calculations clear.
 */
void startMozzi(unsigned int control_rate_hz)
{

	// audio
	pinMode(AUDIO_CHANNEL_1_PIN, OUTPUT);		// set pin to output for audio
	Timer1.initialize(1000000/AUDIO_RATE);      // set period
	Timer1.pwm(AUDIO_CHANNEL_1_PIN, 512);       // pwm pin, 50% duty cycle
	Timer1.attachInterrupt(outputAudio);		// call outputAudio() on each interrupt

	// control
	TimerTwo::init(1000000/control_rate_hz,updateControl); // set period, attach updateControl()
	TimerTwo::start();

}


#define BUFFER_NUM_TABLE_CELLS 256
static int output_buffer[BUFFER_NUM_TABLE_CELLS];
static volatile unsigned int num_out;


/** @ingroup core
This is required in Arduino's loop().
It puts the sound calculated by updateAudio()
into Mozzi's output buffer.
*/
void audioHook()
{

	static unsigned char in_pos = 0;
	static unsigned int num_in = 0;
	unsigned int gap;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{ // because 16bit access to num_out which can be changed in Buffer::out() used in audio interrupt
		gap = num_in - num_out; // wraps to a big number if it's negative
	}
	if (gap < BUFFER_NUM_TABLE_CELLS)
	{
		in_pos++;
		num_in++;
		output_buffer[in_pos] = updateAudio() + AUDIO_BIAS;
	}

}


/* This is the callback routine attached to the Timer1 audio interrupt. It
moves sound data from the output buffer to the Arduino output register, running
at AUDIO_RATE.
*/
inline
static void outputAudio()
{ // takes 1-2 us, and doesn't appear to get interrupted by control on scope
	static unsigned char out_pos;
	out_pos++;
	num_out++;
	AUDIO_CHANNEL_1_OUTPUT_REGISTER =  output_buffer[out_pos];
}



