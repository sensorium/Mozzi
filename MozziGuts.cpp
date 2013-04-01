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
ATmega328 technical manual, Section 12.7.4:
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


//-----------------------------------------------------------------------------------------------------------------
// ring buffer for audio output
#define BUFFER_NUM_CELLS 256
static unsigned int output_buffer[BUFFER_NUM_CELLS];
static volatile unsigned long output_buffer_tail; // shared by audioHook() (in loop()), and outputAudio() (in audio interrupt), where it is changed

//-----------------------------------------------------------------------------------------------------------------

#if USE_AUDIO_INPUT
#include "Arduino.h"
#include "mozzi_analog.h"

static void adcSetupAudioInput(){
	adcEnableInterrupt();
	setupFastAnalogRead();
	adcSetChannel(0);
}

static volatile long input_gap;
static volatile unsigned long input_buffer_head;
static volatile int input_buffer[BUFFER_NUM_CELLS];
static boolean do_update_audio;
static int audio_input; // holds the latest audio from input_buffer


int getAudioInput()
{
	return audio_input;
}


/* Audio ISR. This is called with when the adc finishes a conversion.
*/
ISR(ADC_vect, ISR_BLOCK)
{
 // gets here about 16us after being set audio output isr
	input_buffer_head++;
	input_buffer[input_buffer_head & (BUFFER_NUM_CELLS-1)] = ADC; // put new data into input_buffer, don't worry about overwriting old, as guarding would also cause a glitch
	if (input_gap > (BUFFER_NUM_CELLS/2)) {
		do_update_audio = true;
	}else{
		do_update_audio = false;
	}
}

#endif



void audioHook() // 2us excluding updateAudio()
{
	static unsigned long output_buffer_head = 0;
	long output_gap = output_buffer_head - output_buffer_tail; // wraps to a big number if it's negative, and will take a long time to wrap

#if USE_AUDIO_INPUT // 3us

	static unsigned long input_buffer_tail =0;
	input_gap = input_buffer_head - input_buffer_tail; // wraps to a big number if it's negative, and will take a long time to wrap
	if ((output_gap < BUFFER_NUM_CELLS) && do_update_audio) {
		input_buffer_tail++;
		audio_input = input_buffer[input_buffer_tail & (BUFFER_NUM_CELLS-1)];

#else

	if(output_gap < BUFFER_NUM_CELLS) // prevent writing over cells which haven't been output yet
	{
		
#endif
		output_buffer_head++;
		output_buffer[(unsigned char)output_buffer_head & (unsigned char)(BUFFER_NUM_CELLS-1)] = (unsigned int) (updateAudio() + AUDIO_BIAS);
	}
	

}


//-----------------------------------------------------------------------------------------------------------------
#if (AUDIO_MODE == STANDARD)

static void startAudioStandard9bitPwm(){
	pinMode(AUDIO_CHANNEL_1_PIN, OUTPUT);	// set pin to output for audio
	Timer1.initialize(1000000UL/AUDIO_RATE, PHASE_FREQ_CORRECT);		// set period, phase and frequency correct
	//Serial.print("STANDARD Timer 1 period = "); Serial.println(Timer1.getPeriod()); // 976

	Timer1.pwm(AUDIO_CHANNEL_1_PIN, AUDIO_BIAS);		// pwm pin, 50% of Mozzi's duty cycle, ie. 0 signal
	//Timer1.attachInterrupt(outputAudio); // TB 15-2-2013 Replaced this line with the ISR, saves some processor time
	TIMSK1 = _BV(TOIE1); 	// Overflow Interrupt Enable (when not using Timer1.attachInterrupt())

#if USE_AUDIO_INPUT
	adcSetupAudioInput();
#endif
}


/* Interrupt service routine moves sound data from the output buffer to the
Arduino output register, running at AUDIO_RATE. */
ISR(TIMER1_OVF_vect, ISR_BLOCK) {
#if USE_AUDIO_INPUT
	sbi(ADCSRA, ADSC);				// start next adc conversion
#endif
	output_buffer_tail++;
	AUDIO_CHANNEL_1_OUTPUT_REGISTER = output_buffer[(unsigned char)output_buffer_tail & (unsigned char)(BUFFER_NUM_CELLS-1)]; // 1us, 2.5us with longs
}


//-----------------------------------------------------------------------------------------------------------------
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

#if USE_AUDIO_INPUT
	adcSetupAudioInput();
#endif
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
#if USE_AUDIO_INPUT
	sbi(ADCSRA, ADSC);				// start next adc conversion
#endif
	unsigned int out = output_buffer[num_out++];
	// read about dual pwm at http://www.openmusiclabs.com/learning/digital/pwm-dac/dual-pwm-circuits/
	// sketches at http://wiki.openmusiclabs.com/wiki/PWMDAC,  http://wiki.openmusiclabs.com/wiki/MiniArDSP

	// 14 bit - this sounds better than 12 bit, it's cleaner, less bitty, don't notice aliasing
	AUDIO_CHANNEL_1_HIGHBYTE_REGISTER = out >> 7; // B11111110000000 becomes B1111111
	AUDIO_CHANNEL_1_LOWBYTE_REGISTER = out & 127; // B001111111
	// #if USE_AUDIO_INPUT
	// audioInputToBuffer();
	// #endif
}


#endif


//-----------------------------------------------------------------------------------------------------------------

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
