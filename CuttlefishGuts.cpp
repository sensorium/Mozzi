/*
 * CuttlefishGuts.cpp
 *
 * Copyright 2012 sweatsonics@sweatsonics.com.
 *
 * This file is part of Cuttlefish.
 *
 * Cuttlefish is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cuttlefish is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cuttlefish.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "Cuttlefishguts.h"
#include <util/atomic.h>

// Interrupts:
// Timer2 has highest priority, but you can't have a flexible interrupt rate as well as pwm.
// The TimerOne library is more flexible and allows both pwm and interrupts,
// by using ICR1 for interrupt rate and OCR1A (pin 9) (or OCR1B pin 10) for pwm level
// So we use Timer1 for audio.
// Ideally we'd use Timer0 for control because it has lowest priority, but Arduino uses
// it for time and delay functions, so we use Timer 2 for control and hope it won't
// interfere with the audio interrupt.


/** startCuttlefish sets up the timers for audio and control rate processes.
 *  It goes in your sketch's setup() routine.  You need to specify a control rate in
 *  Hz, which can be any power of 2 above and including 64.
 *  The practical upper limit for control is about 512 Hz.
 * 	It starts audio interrupts on Timer 1 and control interrupts on Timer 2.
 *  The audio rate is currently fixed at 16384 Hz.  The control rate is 64 Hz,
 *  but you can set any power of 2 above and including 64.
 *  The practical upper limit for control is about 512 Hz or even as low as 128 Hz,
 *  depending on how much you try to do in your updateControl() routine.
 */

void startCuttlefish(unsigned int control_rate_hz)
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


#define BUFFER_NUM_CELLS 256
static int bufarray[BUFFER_NUM_CELLS];
static volatile unsigned int num_out;


void audioHook()
{

	static unsigned char in_pos = 0;
	static unsigned int num_in = 0;
	unsigned int gap;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{ // because 16bit access to num_out which can be changed in Buffer::out() used in audio interrupt
		gap = num_in - num_out; // wraps to a big number if it's negative
	}
	if (gap < BUFFER_NUM_CELLS)
	{
		in_pos++;
		num_in++;
		bufarray[in_pos] = updateAudio() + AUDIO_BIAS;
	}

}


inline
static void outputAudio()
{ // takes 1-2 us, and doesn't appear to get interrupted by control on scope
	static unsigned char out_pos;
	out_pos++;
	num_out++;
	AUDIO_CHANNEL_1_OUTPUT_REGISTER =  bufarray[out_pos];
}


