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
#include "mozzi_config.h" // at the top of all MozziGuts and analog files
#include <util/atomic.h>
//#include "mozzi_utils.h"
#include "CircularBuffer.h"

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
CircularBuffer <unsigned int>output_buffer;

//-----------------------------------------------------------------------------------------------------------------

// to store backups of timer registers so Mozzi can be stopped and pre_mozzi timer values can be restored
static byte pre_mozzi_TCCR0A, pre_mozzi_TCCR0B, pre_mozzi_OCR0A, pre_mozzi_TIMSK0;
static byte pre_mozzi_TCCR1A, pre_mozzi_TCCR1B, pre_mozzi_OCR1A, pre_mozzi_TIMSK1;

#if (AUDIO_MODE == HIFI)
#if defined(TCCR2A)
static byte pre_mozzi_TCCR2A, pre_mozzi_TCCR2B, pre_mozzi_OCR2A, pre_mozzi_TIMSK2;
#elif defined(TCCR2)
static byte pre_mozzi_TCCR2, pre_mozzi_OCR2, pre_mozzi_TIMSK;
#elif defined(TCCR4A)
static byte pre_mozzi_TCCR4A, pre_mozzi_TCCR4B, pre_mozzi_TCCR4C, pre_mozzi_TCCR4D, pre_mozzi_TCCR4E, pre_mozzi_OCR4C, pre_mozzi_TIMSK4;
#endif
#endif



static void backupPreMozziTimer1()
{
	// backup pre-mozzi register values for pausing later
	pre_mozzi_TCCR1A = TCCR1A;
	pre_mozzi_TCCR1B = TCCR1B;
	pre_mozzi_OCR1A = OCR1A;
	pre_mozzi_TIMSK1 = TIMSK1;
}



//-----------------------------------------------------------------------------------------------------------------

// to store backups of mozzi's changes to timer registers so Mozzi can be paused and unPaused
static byte mozzi_TCCR0A, mozzi_TCCR0B, mozzi_OCR0A, mozzi_TIMSK0;
static byte mozzi_TCCR1A, mozzi_TCCR1B, mozzi_OCR1A, mozzi_TIMSK1;

#if (AUDIO_MODE == HIFI)
#if defined(TCCR2A)
static byte mozzi_TCCR2A, mozzi_TCCR2B, mozzi_OCR2A, mozzi_TIMSK2;
#elif defined(TCCR2)
static byte mozzi_TCCR2, mozzi_OCR2, mozzi_TIMSK;
#elif defined(TCCR4A)
static byte mozzi_TCCR4A, mozzi_TCCR4B, mozzi_TCCR4C, mozzi_TCCR4D, mozzi_TCCR4E, mozzi_OCR4C, mozzi_TIMSK4;
#endif
#endif



static void backupMozziTimer1()
{
	// backup mozzi register values for unpausing later
	mozzi_TCCR1A = TCCR1A;
	mozzi_TCCR1B = TCCR1B;
	mozzi_OCR1A = OCR1A;
	mozzi_TIMSK1 = TIMSK1;
}

//-----------------------------------------------------------------------------------------------------------------

#if (USE_AUDIO_INPUT==true)

//static volatile long input_gap;
//static volatile unsigned long input_buffer_head;
//static volatile int input_buffer[BUFFER_NUM_CELLS];

// ring buffer for audio input
CircularBuffer <unsigned int>input_buffer;

static boolean audio_input_is_available;
static int audio_input; // holds the latest audio from input_buffer
unsigned char adc_count = 0;


int getAudioInput()
{
	return audio_input;
}


static void startFirstAudioADC()
{
	adcStartConversion(adcPinToChannelNum(AUDIO_INPUT_PIN));
}

/*
static void receiveFirstAudioADC() 
{
	// nothing
}
*/

static void startSecondAudioADC()
{
	ADCSRA |= (1 << ADSC); // start a second conversion on the current channel
}



static void receiveSecondAudioADC()
{
	if (!input_buffer.isFull()) 
		input_buffer.write(ADC);
}



ISR(ADC_vect, ISR_BLOCK)
{
	switch (adc_count){
	case 0:
		// 6us
		receiveSecondAudioADC();
		adcReadSelectedChannels();
		break;

	case 1:
		// <2us, <1us w/o receive
		//receiveFirstControlADC();
		startSecondControlADC();
		break;

	case 2:
		// 3us
		receiveSecondControlADC();
		startFirstAudioADC();
		break;


		//      	case 3:
		// invisible
		//      	receiveFirstAudioADC();
		//      	break;

	}
	adc_count++;
}
#endif


void audioHook() // 2us excluding updateAudio()
{
//setPin13High();
#if (USE_AUDIO_INPUT==true)
		if (!input_buffer.isEmpty()) 
			audio_input = input_buffer.read();
#endif

	if (!output_buffer.isFull()) {
		output_buffer.write((unsigned int) (updateAudio() + AUDIO_BIAS));

	}
//setPin13Low();
}



//-----------------------------------------------------------------------------------------------------------------
#if (AUDIO_MODE == STANDARD) || (AUDIO_MODE == STANDARD_PLUS)

static void startAudioStandard()
{
	backupPreMozziTimer1();

	pinMode(AUDIO_CHANNEL_1_PIN, OUTPUT);	// set pin to output for audio
#if (AUDIO_MODE == STANDARD)
	Timer1.initializeCPUCycles(16000000UL/AUDIO_RATE, PHASE_FREQ_CORRECT);		// set period, phase and frequency correct
#else // (AUDIO_MODE == STANDARD_PLUS)
	Timer1.initializeCPUCycles(16000000UL/PWM_RATE, FAST);	// fast mode enables higher PWM rate
#endif
	Timer1.pwm(AUDIO_CHANNEL_1_PIN, AUDIO_BIAS);		// pwm pin, 50% of Mozzi's duty cycle, ie. 0 signal
	TIMSK1 = _BV(TOIE1); 	// Overflow Interrupt Enable (when not using Timer1.attachInterrupt())

	backupMozziTimer1();
}



/* Interrupt service routine moves sound data from the output buffer to the
Arduino output register, running at AUDIO_RATE. */

ISR(TIMER1_OVF_vect, ISR_BLOCK)
{

#if (AUDIO_MODE == STANDARD_PLUS) && (AUDIO_RATE == 16384) // only update every second ISR, if lower audio rate
	static boolean alternate;
	alternate = !alternate;
	if(alternate)
	{
#endif

#if (USE_AUDIO_INPUT==true)
		adc_count = 0;
		startSecondAudioADC();
#endif

//if (!output_buffer.isEmpty()) {
	AUDIO_CHANNEL_1_OUTPUT_REGISTER = output_buffer.read();
//}

#if (AUDIO_MODE == STANDARD_PLUS) && (AUDIO_RATE==16384) // all this conditional compilation is so clutsy!
	}
#endif

}

// end STANDARD

//-----------------------------------------------------------------------------------------------------------------
#elif (AUDIO_MODE == HIFI)

static void startAudioHiFi()
{
	backupPreMozziTimer1();

	// pwm on timer 1
	pinMode(AUDIO_CHANNEL_1_HIGHBYTE_PIN, OUTPUT);	// set pin to output for audio, use 3.9k resistor
	pinMode(AUDIO_CHANNEL_1_LOWBYTE_PIN, OUTPUT);	// set pin to output for audio, use 499k resistor

	Timer1.initializeCPUCycles(16000000UL/125000, FAST);		// set period for 125000 Hz fast pwm carrier frequency = 14 bits

	Timer1.pwm(AUDIO_CHANNEL_1_HIGHBYTE_PIN, 0);		// pwm pin, 0% duty cycle, ie. 0 signal
	Timer1.pwm(AUDIO_CHANNEL_1_LOWBYTE_PIN, 0);		// pwm pin, 0% duty cycle, ie. 0 signal

	backupMozziTimer1();

	// audio output interrupt on timer 2, sets the pwm levels of timer 1
	setupTimer2();
}

/* set up Timer 2 using modified FrequencyTimer2 library */
void dummy(){}


static void backupPreMozziTimer2()
{
	//backup Timer2 register values
#if defined(TCCR2A)
	pre_mozzi_TCCR2A = TCCR2A;
	pre_mozzi_TCCR2B = TCCR2B;
	pre_mozzi_OCR2A = OCR2A;
	pre_mozzi_TIMSK2 = TIMSK2;
#elif defined(TCCR2)
	pre_mozzi_TCCR2 = TCCR2;
	pre_mozzi_OCR2 = OCR2;
	pre_mozzi_TIMSK = TIMSK;
#elif defined(TCCR4A)
	pre_mozzi_TCCR4B = TCCR4A;
	pre_mozzi_TCCR4B = TCCR4B;
	pre_mozzi_TCCR4B = TCCR4C;
	pre_mozzi_TCCR4B = TCCR4D;
	pre_mozzi_TCCR4B = TCCR4E;
	pre_mozzi_OCR4C = OCR4C;
	pre_mozzi_TIMSK4 = TIMSK4;
#endif
}



static void backupMozziTimer2()
{
#if defined(TCCR2A)
	mozzi_TCCR2A = TCCR2A;
	mozzi_TCCR2B = TCCR2B;
	mozzi_OCR2A = OCR2A;
	mozzi_TIMSK2 = TIMSK2;
#elif defined(TCCR2)
	mozzi_TCCR2 = TCCR2;
	mozzi_OCR2 = OCR2;
	mozzi_TIMSK = TIMSK;
#elif defined(TCCR4A)
	mozzi_TCCR4B = TCCR4A;
	mozzi_TCCR4B = TCCR4B;
	mozzi_TCCR4B = TCCR4C;
	mozzi_TCCR4B = TCCR4D;
	mozzi_TCCR4B = TCCR4E;
	mozzi_OCR4C = OCR4C;
	mozzi_TIMSK4 = TIMSK4;
#endif
}


// audio output interrupt on timer 2 (or 4 on ATMEGA32U4 cpu), sets the pwm levels of timer 1
static void setupTimer2()
{
	backupPreMozziTimer2(); // to reset while pausing
	
	FrequencyTimer2::setPeriodCPUCycles(16000000UL/AUDIO_RATE);
	FrequencyTimer2::setOnOverflow(dummy);
	FrequencyTimer2::enable();

	// backup mozzi register values for unpausing later
	backupMozziTimer2();
}



#if defined(TIMER2_COMPA_vect)
ISR(TIMER2_COMPA_vect)
#elif defined(TIMER2_COMP_vect)
ISR(TIMER2_COMP_vect)
#elif defined(TIMER4_COMPA_vect)
ISR(TIMER4_COMPA_vect)
#else
#error "This board does not have a hardware timer which is compatible with FrequencyTimer2"
void dummy_function(void)
#endif
{
#if (USE_AUDIO_INPUT==true)
	adc_count = 0;
	startSecondAudioADC();
#endif

		// read about dual pwm at http://www.openmusiclabs.com/learning/digital/pwm-dac/dual-pwm-circuits/
		// sketches at http://wiki.openmusiclabs.com/wiki/PWMDAC,  http://wiki.openmusiclabs.com/wiki/MiniArDSP
		//if (!output_buffer.isEmpty()){
		unsigned int out = output_buffer.read();
		// 14 bit, 7 bits on each pin
		AUDIO_CHANNEL_1_HIGHBYTE_REGISTER = out >> 7; // B11111110000000 becomes B1111111
		AUDIO_CHANNEL_1_LOWBYTE_REGISTER = out & 127; // B01111111
		//}

}

//  end of HIFI

#endif


//-----------------------------------------------------------------------------------------------------------------

static void updateControlWithAutoADC()
{
	updateControl();
	adcStartReadCycle();
}


/* Sets up Timer 0 for control interrupts. This is the same for all output
options Using Timer0 for control disables Arduino's time functions but also
saves on the interrupts and blocking action of those functions. May add a config
option for Using Timer2 instead if needed. (MozziTimer2 can be re-introduced for
that). */
static void startControl(unsigned int control_rate_hz)
{
	// backup pre-mozzi register values
	pre_mozzi_TCCR0A = TCCR0A;
	pre_mozzi_TCCR0B = TCCR0B;
	pre_mozzi_OCR0A = OCR0A;
	pre_mozzi_TIMSK0 = TIMSK0;

	TimerZero::init(1000000/control_rate_hz,updateControlWithAutoADC); // set period, attach updateControlWithAutoADC()
	TimerZero::start();

	// backup mozzi register values for unpausing later
	mozzi_TCCR0A = TCCR0A;
	mozzi_TCCR0B = TCCR0B;
	mozzi_OCR0A = OCR0A;
	mozzi_TIMSK0 = TIMSK0;
}


void startMozzi(int control_rate_hz)
{
	setupMozziADC(); // you can use setupFastAnalogRead() with FASTER or FASTEST in setup() if desired
	// delay(200); // so AutoRange doesn't read 0 to start with
	startControl(control_rate_hz);
#if (AUDIO_MODE == STANDARD) || (AUDIO_MODE == STANDARD_PLUS)
	startAudioStandard();
#elif (AUDIO_MODE == HIFI)
	startAudioHiFi();
#endif
}


void pauseMozzi(){
	// restore backed up register values
	TCCR0A = pre_mozzi_TCCR0A;
	TCCR0B = pre_mozzi_TCCR0B;
	OCR0A = pre_mozzi_OCR0A;
	TIMSK0 = pre_mozzi_TIMSK0;

	TCCR1A = pre_mozzi_TCCR1A;
	TCCR1B = pre_mozzi_TCCR1B;
	OCR1A = pre_mozzi_OCR1A;
	TIMSK1 = pre_mozzi_TIMSK1;

#if (AUDIO_MODE == HIFI)
#if defined(TCCR2A)
	TCCR2A = pre_mozzi_TCCR2A;
	TCCR2B = pre_mozzi_TCCR2B;
	OCR2A = pre_mozzi_OCR2A;
	TIMSK2 = pre_mozzi_TIMSK2;
#elif defined(TCCR2)
	TCCR2 = pre_mozzi_TCCR2;
	OCR2 = pre_mozzi_OCR2;
	TIMSK = pre_mozzi_TIMSK;
#elif defined(TCCR4A)
	TCCR4B = pre_mozzi_TCCR4A;
	TCCR4B = pre_mozzi_TCCR4B;
	TCCR4B = pre_mozzi_TCCR4C;
	TCCR4B = pre_mozzi_TCCR4D;
	TCCR4B = pre_mozzi_TCCR4E;
	OCR4C = pre_mozzi_OCR4C;
	TIMSK4 = pre_mozzi_TIMSK4;
#endif
#endif			
}


void unPauseMozzi()
{
	// restore backed up register values
	TCCR0A = mozzi_TCCR0A;
	TCCR0B = mozzi_TCCR0B;
	OCR0A = mozzi_OCR0A;
	TIMSK0 = mozzi_TIMSK0;

	TCCR1A = mozzi_TCCR1A;
	TCCR1B = mozzi_TCCR1B;
	OCR1A = mozzi_OCR1A;
	TIMSK1 = mozzi_TIMSK1;

#if (AUDIO_MODE == HIFI)
#if defined(TCCR2A)
	TCCR2A = mozzi_TCCR2A;
	TCCR2B = mozzi_TCCR2B;
	OCR2A = mozzi_OCR2A;
	TIMSK2 = mozzi_TIMSK2;
#elif defined(TCCR2)
	TCCR2 = mozzi_TCCR2;
	OCR2 = mozzi_OCR2;
	TIMSK = mozzi_TIMSK;
#elif defined(TCCR4A)
	TCCR4B = mozzi_TCCR4A;
	TCCR4B = mozzi_TCCR4B;
	TCCR4B = mozzi_TCCR4C;
	TCCR4B = mozzi_TCCR4D;
	TCCR4B = mozzi_TCCR4E;
	OCR4C = mozzi_OCR4C;
	TIMSK4 = mozzi_TIMSK4;
#endif
#endif			
}


unsigned long audioTicks()
{
	return output_buffer.count();
}


unsigned long mozziMicros()
{
	return audioTicks() * MICROS_PER_AUDIO_TICK;
}



// Unmodified TimerOne.cpp has TIMER3_OVF_vect.
// Watch out if you update the library file.
// The symptom will be no sound.
// ISR(TIMER1_OVF_vect)
// {
// 	Timer1.isrCallback();
// }
