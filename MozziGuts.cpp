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



#if (STANDARD_9_BIT_PWM)

/** @ingroup core
Sets up the timers for audio and control rate processes. It goes in your
sketch's setup() routine. startMozzi() starts audio interrupts on Timer 1
and control interrupts on Timer 0. The audio rate is currently fixed at 16384
Hz.
@param control_rate_hz Sets how often updateControl() is called. It can be any
power of 2 above and including 64. The practical upper limit for control rate
depends on how busy the processor is, and you might need to do some tests to
find the best setting. It's good to define CONTROL_RATE in your
sketches (eg. "#define CONTROL_RATE 128") because the literal numeric value is
necessary for Oscils to work properly, and it also helps to keep the
calculations in your sketch clear.
@todo Revisit output possibilities:
- an audio interrupt which sets the level on a pwm pin which runs much faster than the sample rate (uses 2 timers)
- 16 bit output using 2 x 8 bit pwm pins and 2 0.5% toleranced resistors, gives lower noise floor, see http://www.openmusiclabs.com/learning/digital/pwm-dac/dual-pwm-circuits/
- use full port, without pwm, requires a resistor ladder (maybe use readymade resistor networks)
*/
void startMozzi(unsigned int control_rate_hz)
{

	// audio
	pinMode(AUDIO_CHANNEL_1_PIN, OUTPUT);	// set pin to output for audio
	Timer1.initialize(1000000UL/AUDIO_RATE);		// set period
	Timer1.pwm(AUDIO_CHANNEL_1_PIN, AUDIO_BIAS);		// pwm pin, 50% of Mozzi's duty cycle, ie. 0 signal

	// #if (MOZZI_AC_OUTPUT)
	// // alternative audio technique from "toneAC.cpp" by Tim Eckel - teckel@leethost.com
	// // Copyright 2013 License: GNU GPL v3 http://www.gnu.org/licenses/gpl-3.0.html
	// // to get louder output form a speaker by using two out of phase pins in push/pull fashion
	// //TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(COM1B0); // Inverted/none-inverted mode (AC).
	// Timer1.pwm(AUDIO_CHANNEL_1_AC_PIN, AUDIO_BIAS);  // AC pin
	// TCCR1A |= _BV(COM1B0); // Invert AC pin output
	// #endif

	// TB 15-2-2013 Replaced this line with the ISR below (doesn't work yet)
	//Timer1.attachInterrupt(outputAudio);			// call outputAudio() on each interrupt

	// starts the interrupt (when not using Timer1.attachInterrupt())
	TIMSK1 = _BV(TOIE1);
	
	// control
	// Using Timer0 for control disables Arduino's time functions
	// but also saves on the interrupts and blocking action of those functions.
	// May add a config option for Using Timer2 instead if needed.
	// (MozziTimer2 can be re-introduced for that).
	TimerZero::init(1000000/control_rate_hz,updateControl); // set period, attach updateControl()
	TimerZero::start();

}


// ring buffer for audio output
#define BUFFER_NUM_CELLS 256
static int output_buffer[BUFFER_NUM_CELLS];

// shared by audioHook() (in loop()), and outputAudio() (in audio interrupt), where it is changed
static volatile unsigned char num_out; 

// interrupt service routine
ISR(TIMER1_OVF_vect, ISR_BLOCK)
{
	//AUDIO_CHANNEL_1_OUTPUT_REGISTER =  output_buffer[num_out++];
	outputAudio();
}

/* This is the callback routine attached to the Timer1 audio interrupt.
It moves sound data from the output buffer to the Arduino output register,
running at AUDIO_RATE.
*/
inline
static void outputAudio()
{ // takes 1 us or shorter, digital scope seems to have trouble catching it
	//SET_PIN13_HIGH;
	AUDIO_CHANNEL_1_OUTPUT_REGISTER =  output_buffer[num_out++];
	//SET_PIN13_LOW;
}

/** @ingroup core
This is required in Arduino's loop(). If there is room in Mozzi's output buffer,
audioHook() calls updateAudio() once and puts the result into the output
buffer. If other functions are called in loop() along with audioHook(), see if
they can be moved into updateControl(). Otherwise it may be most efficient to
calculate a block of samples at a time by putting audioHook() in a loop of its
own, rather than calculating only 1 sample for each time your other functions
are called.
@todo try pre-decrement positions and swap gap calc around
*/
void audioHook()
{
	static unsigned char num_in = 0;
	unsigned int gap = num_in - num_out; // wraps to a big number if it's negative

	if(gap < BUFFER_NUM_CELLS) // prevent writing over cells which haven't been output yet
	{
		output_buffer[num_in++] = updateAudio() + AUDIO_BIAS;
	}
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif (LOW_SPEED_16_BIT_PWM)

/** @ingroup core
Sets up the timers for audio and control rate processes. It goes in your
sketch's setup() routine. startMozzi() starts audio interrupts on Timer 1
and control interrupts on Timer 0. The audio rate is currently fixed at 16384
Hz.
@param control_rate_hz Sets how often updateControl() is called. It can be any
power of 2 above and including 64. The practical upper limit for control rate
depends on how busy the processor is, and you might need to do some tests to
find the best setting. It's good to define CONTROL_RATE in your
sketches (eg. "#define CONTROL_RATE 128") because the literal numeric value is
necessary for Oscils to work properly, and it also helps to keep the
calculations in your sketch clear.
@todo Revisit output possibilities:
- an audio interrupt which sets the level on a pwm pin which runs much faster than the sample rate (uses 2 timers)
- 16 bit output using 2 x 8 bit pwm pins and 2 0.5% toleranced resistors, gives lower noise floor, see http://www.openmusiclabs.com/learning/digital/pwm-dac/dual-pwm-circuits/
- use full port, without pwm, requires a resistor ladder (maybe use readymade resistor networks)
*/
void startMozzi(unsigned int control_rate_hz)
{

	// audio
	pinMode(AUDIO_CHANNEL_1_LOWBYTE_PIN, OUTPUT);	// set pin to output for audio
	pinMode(AUDIO_CHANNEL_1_HIGHBYTE_PIN, OUTPUT);	// set pin to output for audio
	Timer1.initialize(1000000UL/AUDIO_RATE);		// set period
	Timer1.pwm(AUDIO_CHANNEL_1_LOWBYTE_PIN, lowByte(AUDIO_BIAS));		// pwm pin, 50% of Mozzi's duty cycle, ie. 0 signal
	Timer1.pwm(AUDIO_CHANNEL_1_HIGHBYTE_PIN, highByte(AUDIO_BIAS));		// pwm pin, 50% of Mozzi's duty cycle, ie. 0 signal

	// starts the interrupt (when not using Timer1.attachInterrupt())
	TIMSK1 = _BV(TOIE1);
	
	// control
	// Using Timer0 for control disables Arduino's time functions
	// but also saves on the interrupts and blocking action of those functions.
	// May add a config option for Using Timer2 instead if needed.
	// (MozziTimer2 can be re-introduced for that).
	TimerZero::init(1000000/control_rate_hz,updateControl); // set period, attach updateControl()
	TimerZero::start();

}


// ring buffer for audio output
#define BUFFER_NUM_CELLS 256
static int output_buffer[BUFFER_NUM_CELLS];

// shared by audioHook() (in loop()), and outputAudio() (in audio interrupt), where it is changed
static volatile unsigned char num_out; 

// interrupt service routine
ISR(TIMER1_OVF_vect, ISR_BLOCK)
{
	// unsigned int out = output_buffer[num_out++];
	// AUDIO_CHANNEL_1_HIGHBYTE_REGISTER = out>>8;
	// AUDIO_CHANNEL_1_LOWBYTE_REGISTER = (unsigned int) lowByte(out);
	//AUDIO_CHANNEL_1_LOWBYTE_REGISTER = (unsigned int) lowByte(out);
	outputAudio();
}

/* This is the callback routine attached to the Timer1 audio interrupt.
It moves sound data from the output buffer to the Arduino output register,
running at AUDIO_RATE.
*/
inline
static void outputAudio()
{ // takes 1 us or shorter, digital scope seems to have trouble catching it
	//SET_PIN13_HIGH;
	int out = output_buffer[num_out++];
	
	// this from http://wiki.openmusiclabs.com/wiki/MiniArDSP
	AUDIO_CHANNEL_1_HIGHBYTE_REGISTER = (out + 0x8000) >> 8; // convert to unsigned and output high byte
	AUDIO_CHANNEL_1_LOWBYTE_REGISTER = out; // output the bottom byte
	//SET_PIN13_LOW;
}

// void TIM16_WriteOCR1A( unsigned int i ) {
// unsigned char sreg;
// /* Save global interrupt flag */ sreg = SREG;
// /* Disable interrupts */
// _CLI();
// /* Set TCNT1 to i */
// OCR1A = i;
// /* Restore global interrupt flag */ SREG = sreg;
// }

/** @ingroup core
This is required in Arduino's loop(). If there is room in Mozzi's output buffer,
audioHook() calls updateAudio() once and puts the result into the output
buffer. If other functions are called in loop() along with audioHook(), see if
they can be moved into updateControl(). Otherwise it may be most efficient to
calculate a block of samples at a time by putting audioHook() in a loop of its
own, rather than calculating only 1 sample for each time your other functions
are called.
@todo try pre-decrement positions and swap gap calc around
*/
void audioHook()
{
	static unsigned char num_in = 0;
	unsigned int gap = num_in - num_out; // wraps to a big number if it's negative

	if(gap < BUFFER_NUM_CELLS) // prevent writing over cells which haven't been output yet
	{
		//output_buffer[num_in++] = (unsigned int) (updateAudio() + AUDIO_BIAS);
		output_buffer[num_in++] = updateAudio();
	}
}

#endif




// Unmodified TimerOne.cpp has TIMER3_OVF_vect.
// Watch out if you update the library file.
// The symptom will be no sound.
// ISR(TIMER1_OVF_vect)
// {
// 	Timer1.isrCallback();
// }
