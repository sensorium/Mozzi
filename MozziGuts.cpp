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

/*
Section 12.7.4:
The dual-slope operation [of phase correct pwm] has lower maximum operation
frequency than single slope operation. However, due to the symmetric feature
of the dual-slope PWM modes, these modes are preferred for motor control
applications.
Due to the single-slope operation, the operating frequency of the
fast PWM mode can be twice as high as the phase correct PWM mode that use
dual-slope operation. This high frequency makes the fast PWM mode well suited
for power regulation, rectification, and DAC applications. High frequency allows
physically small sized external components (coils, capacitors)..

DAC, that's us!  Fast PWM.
*/




// ring buffer for audio output
#define BUFFER_NUM_CELLS 256
static unsigned int output_buffer[BUFFER_NUM_CELLS];

// shared by audioHook() (in loop()), and outputAudio() (in audio interrupt), where it is changed
static volatile unsigned char num_out;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if (STANDARD_9_BIT_PWM)

static void startAudioStandard9bitPwm(){
	pinMode(AUDIO_CHANNEL_1_PIN, OUTPUT);	// set pin to output for audio
	Timer1.initialize(1000000UL/AUDIO_RATE);		// set period
	Timer1.pwm(AUDIO_CHANNEL_1_PIN, AUDIO_BIAS);		// pwm pin, 50% of Mozzi's duty cycle, ie. 0 signal
	//Timer1.attachInterrupt(outputAudio); // TB 15-2-2013 Replaced this line with the ISR, saves some processor time
	TIMSK1 = _BV(TOIE1); 	// Overflow Interrupt Enable (when not using Timer1.attachInterrupt())
}


/* Interrupt service routine moves sound data from the output buffer to the
Arduino output register, running at AUDIO_RATE. */
ISR(TIMER1_OVF_vect, ISR_BLOCK) {
	AUDIO_CHANNEL_1_OUTPUT_REGISTER =  output_buffer[num_out++];
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif (HI_SPEED_16_BIT_PWM)

void setTimer1DualPwmLevels(){
	//setPin13High();
	unsigned int out = output_buffer[num_out++];
	// read about dual pwm at http://www.openmusiclabs.com/learning/digital/pwm-dac/dual-pwm-circuits/
	// sketches at http://wiki.openmusiclabs.com/wiki/PWMDAC,  http://wiki.openmusiclabs.com/wiki/MiniArDSP
	// 16 bit
	//AUDIO_CHANNEL_1_HIGHBYTE_REGISTER = out>>8; //  output high byte, converted to unsigned
	//AUDIO_CHANNEL_1_LOWBYTE_REGISTER = out; // output the bottom byte, offset to match at 0 crossover
	
	// 14 bit
	AUDIO_CHANNEL_1_HIGHBYTE_REGISTER = out >> 7; // B11111110000000 becomes B1111111
	AUDIO_CHANNEL_1_LOWBYTE_REGISTER = out & 127; // B001111111
	
	// 12 bit
	//AUDIO_CHANNEL_1_HIGHBYTE_REGISTER = out >> 6; 	// B111111000000 becomes B1111111
	//AUDIO_CHANNEL_1_LOWBYTE_REGISTER = out & 63; 	// B000111111
	
	// 10 bit
	//AUDIO_CHANNEL_1_HIGHBYTE_REGISTER = out >> 5; // B1111100000 becomes B11111
	//AUDIO_CHANNEL_1_LOWBYTE_REGISTER = out & 31; // B0000011111
	//setPin13Low();
}

static void startAudioHiSpeed16bitPwm(){
	// pwm on timer 1
	pinMode(AUDIO_CHANNEL_1_HIGHBYTE_PIN, OUTPUT);	// set pin to output for audio, use 3.9k resistor
	pinMode(AUDIO_CHANNEL_1_LOWBYTE_PIN, OUTPUT);	// set pin to output for audio, use 1M resistor

	//Timer1.initialize(1000000UL/31250);		// set period for 31250 Hz pwm carrier frequency = 16 bits, audible aliasing
	Timer1.initialize(1000000UL/62500);		// set period for 62500 Hz pwm carrier frequency = 14 bits, audible aliasing
	//Timer1.initialize(1000000UL/65536);	// almost 14 bit
	//Timer1.initialize(1000000UL/131072);	// almost 12 bit
	//Timer1.initialize(1000000UL/125000);		// set period for 125000 Hz pwm carrier frequency = 12 bits
	//Timer1.initialize(1000000UL/250000);		// set period for 250000 Hz pwm carrier frequency = 10 bits
	Timer1.pwm(AUDIO_CHANNEL_1_HIGHBYTE_PIN, 0);		// pwm pin, 0% duty cycle, ie. 0 signal
	Timer1.pwm(AUDIO_CHANNEL_1_LOWBYTE_PIN, 0);		// pwm pin, 0% duty cycle, ie. 0 signal

	// audio output interrupt on timer 2, sets the pwm levels of timer 1
	 FrequencyTimer2::setPeriod(2000000UL/16384); // gives a period half of what's provided, for some reason
	 //FrequencyTimer2::setPeriod(2000000UL/32768); // gives a period half of what's provided, for some reason
	 FrequencyTimer2::enable();
	 FrequencyTimer2::setOnOverflow(setTimer1DualPwmLevels);
	 
	 // sync timers http://www.openmusiclabs.com/learning/digital/synchronizing-timers/
	//GTCCR = (1<<TSM)|(1<<PSRASY)|(1<<PSRSYNC); // halt all timers
	//TCNT1 = 0; 
	//TCNT2 = 1; // offset of 1 for OCRA as TOP timer
	//GTCCR = 0; // restart timers
}


	
	
	
	
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Sets up Timer 0 for control interrupts. This is the same for all output
options Using Timer0 for control disables Arduino's time functions but also
saves on the interrupts and blocking action of those functions. May add a config
option for Using Timer2 instead if needed. (MozziTimer2 can be re-introduced for
that). */
static void startControl(unsigned int control_rate_hz)
{
	TimerZero::init(1000000/control_rate_hz,updateControl); // set period, attach updateControl()
	TimerZero::start();
}


/** @ingroup core
Sets up the timers for audio and control rate processes. It goes in your
sketch's setup() routine. Control interrupts use Timer 0 (disabling Arduino
delay(), millis(), micros()). For standard almost-9-bit pwm, startMozzi() starts
audio interrupts on Timer 1. The audio rate is currently fixed at 16384 Hz.
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
	startControl(control_rate_hz);
#if (STANDARD_9_BIT_PWM)
	startAudioStandard9bitPwm();
#elif (HI_SPEED_16_BIT_PWM)
	startAudioHiSpeed16bitPwm();
#endif
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
		output_buffer[num_in++] = (unsigned int) (updateAudio() + AUDIO_BIAS);
	}
}
/*
0000 0000 0111 1111		127
0111 1111 0000 0000		127<<8=32512
1111 1111 0000 0000		32512+32768=65280 uint = -128 int

1111 1111 1000 0000		-128 int
1000 0000 0000 0000		-128<<8=32768u, int = -32768 int
0000 0001 0000 0000		-128*256=256
0111 1111 1000 0000		-128+32768=32640
*/

// Unmodified TimerOne.cpp has TIMER3_OVF_vect.
// Watch out if you update the library file.
// The symptom will be no sound.
// ISR(TIMER1_OVF_vect)
// {
// 	Timer1.isrCallback();
// }
