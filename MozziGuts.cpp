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
//#include "mozzi_utils.h"

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

PWM frequency tests
62500Hz, single 8 or dual 16 bits, bad aliasing
125000Hz dual 14 bits, sweet
250000Hz dual 12 bits, gritty, if you're gonna have 2 pins, have 14 bits
500000Hz dual 10 bits, grittier
16384Hz single nearly 9 bits (original mode) not bad for a single pin, but carrier freq noise can be an issue
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ring buffer for audio output
#define BUFFER_NUM_CELLS 256
static unsigned int output_buffer[BUFFER_NUM_CELLS];

// shared by audioHook() (in loop()), and outputAudio() (in audio interrupt), where it is changed
static volatile unsigned char num_out;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @ingroup core
This is required in Arduino's loop(). If there is room in Mozzi's output buffer,
audioHook() calls updateAudio() once and puts the result into the output
buffer. If other functions are called in loop() along with audioHook(), see if
they can be moved into updateControl(). Otherwise it may be most efficient to
calculate a block of samples at a time by putting audioHook() in a loop of its
own, rather than calculating only 1 sample for each time your other functions
are called.
@todo Try pre-decrement positions and swap gap calc around
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if (AUDIO_MODE == STANDARD)

static void startAudioStandard9bitPwm(){
	pinMode(AUDIO_CHANNEL_1_PIN, OUTPUT);	// set pin to output for audio
	Timer1.initialize(1000000UL/AUDIO_RATE, PHASE_FREQ_CORRECT);		// set period, phase and frequency correct
	//Serial.print("STANDARD Timer 1 period = "); Serial.println(Timer1.getPeriod()); // 976
	
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
#elif (AUDIO_MODE == HIFI)

/* set up Timer 2 using modified FrequencyTimer2 library */
void dummy(){}
static void setupTimer2(){
	// audio output interrupt on timer 2, sets the pwm levels of timer 1
	 FrequencyTimer2::setPeriod(2000000UL/16384); // gives a period half of what's provided, for some reason
	 FrequencyTimer2::enable();
	 FrequencyTimer2::setOnOverflow(dummy);
}


static void startAudioHiSpeed14bitPwm(){
	// pwm on timer 1
	pinMode(AUDIO_CHANNEL_1_HIGHBYTE_PIN, OUTPUT);	// set pin to output for audio, use 3.9k resistor
	pinMode(AUDIO_CHANNEL_1_LOWBYTE_PIN, OUTPUT);	// set pin to output for audio, use 1M resistor

	Timer1.initialize(1000000UL/125000, FAST);		// set period for 125000 Hz fast pwm carrier frequency = 14 bits
	// Serial.print("HIFI Timer 1 period = "); Serial.println(Timer1.getPeriod());

	Timer1.pwm(AUDIO_CHANNEL_1_HIGHBYTE_PIN, 0);		// pwm pin, 0% duty cycle, ie. 0 signal
	Timer1.pwm(AUDIO_CHANNEL_1_LOWBYTE_PIN, 0);		// pwm pin, 0% duty cycle, ie. 0 signal

	// audio output interrupt on timer 2, sets the pwm levels of timer 1
	 setupTimer2();
}


#if defined(TIMER2_COMPA_vect)
ISR(TIMER2_COMPA_vect)
#elif defined(TIMER2_COMP_vect)
ISR(TIMER2_COMP_vect)
#else
#error "This board does not have a hardware timer which is compatible with FrequencyTimer2"
void dummy_function(void)
#endif
{
	unsigned int out = output_buffer[num_out++];
	// read about dual pwm at http://www.openmusiclabs.com/learning/digital/pwm-dac/dual-pwm-circuits/
	// sketches at http://wiki.openmusiclabs.com/wiki/PWMDAC,  http://wiki.openmusiclabs.com/wiki/MiniArDSP

	// 14 bit - this sounds better than 12 bit, it's cleaner, less bitty, don't notice aliasing
	AUDIO_CHANNEL_1_HIGHBYTE_REGISTER = out >> 7; // B11111110000000 becomes B1111111
	AUDIO_CHANNEL_1_LOWBYTE_REGISTER = out & 127; // B001111111
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
sketch's setup() routine. 

In STANDARD and HIFI modes, Mozzi uses Timer 0 for control interrupts 0, disabling Arduino
delay(), millis(), micros() and delayMicroseconds. 
For delaying events, you can use Mozzi's EventDelay() unit instead (not to be confused with AudioDelay()). 

In STANDARD mode, startMozzi() starts Timer 1 for PWM output and audio output interrupts,
and in HIFI mode, Mozzi uses Timer 1 for PWM and Timer2 for audio interrupts. 

The audio rate is currently fixed at 16384 Hz.

@param control_rate_hz Sets how often updateControl() is called. It can be any
power of 2 above and including 64. The practical upper limit for control rate
depends on how busy the processor is, and you might need to do some tests to
find the best setting. 

It's good to define CONTROL_RATE in your
sketches (eg. "#define CONTROL_RATE 128") because the literal numeric value is
necessary for Oscils to work properly, and it also helps to keep the
calculations in your sketch clear.

@todo See if there is any advantage to using 8 bit full port, without pwm, with a resistor ladder (maybe use readymade resistor networks).
*/
void startMozzi(unsigned int control_rate_hz)
{
	startControl(control_rate_hz);
#if (AUDIO_MODE == STANDARD)
	startAudioStandard9bitPwm();
#elif (AUDIO_MODE == HIFI)
	startAudioHiSpeed14bitPwm();
#endif
}


// Unmodified TimerOne.cpp has TIMER3_OVF_vect.
// Watch out if you update the library file.
// The symptom will be no sound.
// ISR(TIMER1_OVF_vect)
// {
// 	Timer1.isrCallback();
// }
