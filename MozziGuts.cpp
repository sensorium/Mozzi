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
static volatile unsigned long num_out; // shared by audioHook() (in loop()), and outputAudio() (in audio interrupt), where it is changed
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

void audioHook() // 2us excluding updateAudio()
{
	static unsigned long num_in = 0;

	unsigned long gap = num_in - num_out; // wraps to a big number if it's negative, and will take a long time to wrap
	if(gap < BUFFER_NUM_CELLS) // prevent writing over cells which haven't been output yet
	{
		num_in++;
		output_buffer[(unsigned char)num_in & (unsigned char)(BUFFER_NUM_CELLS-1)] = (unsigned int) (updateAudio() + AUDIO_BIAS);
	}

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if USING_AUDIO_INPUT


static void adcSetupAudioInput(){
	adcEnableInterrupt();
	setupFastAnalogRead();
	adcSetChannel(0);
}

// ring buffer for audio input
// based initially on Paul Stoffergen's buffer example, http://www.pjrc.com/teensy/adc.html


// should use adc isr so that when the buffer is empty it will put something there in time for updateAudio to work on it
// befor next output isr

// the buffer will always be empty if updateAudio runs faster than output isr
// which means no input buffering
// and also no output buffering becuase output relies on the input being there

// would it be better to work with chunks like pd instead of ring buffers?

static volatile uint8_t head, tail = 0;
static volatile int16_t buffer[BUFFER_NUM_CELLS];

int getAudioInputFromBuffer()
{
	//static unsigned char tail;

	unsigned char h;
	do {
		h = head;
	} while (h == tail); // wait for data in buffer

	return buffer[tail++];
}

#endif

//static void audioInputToBuffer() // 1.5us
ISR(ADC_vect, ISR_BLOCK)
{
#if USING_AUDIO_INPUT
	//buffer[head++] = ADCL | (ADCH << 8);
	buffer[head++] = ADC; // put new data into buffer, don't worry about overwriting old, as guarding would also cause a glitch
#else
	static boolean secondRead = false;
	//Only record the second read on each channel
	if(secondRead){
		//sensors[current_adc] = ADCL | (ADCH << 8);
		//bobgardner: ..The compiler is clever enough to read the 10 bit value like this: val=ADC;
		sensors[current_adc] = ADC;
		current_adc++;
		if(current_adc > NUM_ANALOG_INPUTS){
			//Sequence complete.  Stop A2D conversions
			readComplete = true;
		}
		else{
			//Switch to next channel
			/*
			ADMUX = (1 << REFS0) | current_adc;
			ADCSRA |= (1 << ADSC);
			*/
			startAnalogRead(current_adc);
		}
		secondRead = false;
	}
	else{
		secondRead = true;
		ADCSRA |= (1 << ADSC);
	}

#endif
}

/*
static void audioInputToBuffer() // 4us
{
	uint8_t h;
	int16_t val;
	//val =receiveAnalogRead();   // grab new reading from ADC
	val = ADC; // calling this at 16384Hz with no other analog ins and setupFastAnalogRead requires no checking         
	h = head + 1;
	//if (h >= BUFFER_NUM_CELLS) h = 0; // not required when BUFFER_NUM_CELLS is 256
	// below will cause glitches as much as overwriting old values
	if (h != tail) {                // if the buffer isn't full
		buffer[h] = val;            // put new data into buffer
		head = h;
	}

	adcStartConversion(); // this way reduces timing by about 0.5 us - not worth keeping if other considerations arise
	//startAnalogRead(0);
}
*/

/*
int getAudioInputFromBuffer()
{
	uint8_t h, t;
	int16_t val;

	do {
		h = head;
		t = tail;                   // wait for data in buffer
	} while (h == t);
	if (++t >= BUFFER_NUM_CELLS) t = 0; // not required when BUFFER_NUM_CELLS is 256
	val = buffer[t];                // remove 1 sample from buffer
	tail = t;
	return val;
}
*/


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if (AUDIO_MODE == STANDARD)

static void startAudioStandard9bitPwm(){
	pinMode(AUDIO_CHANNEL_1_PIN, OUTPUT);	// set pin to output for audio
	Timer1.initialize(1000000UL/AUDIO_RATE, PHASE_FREQ_CORRECT);		// set period, phase and frequency correct
	//Serial.print("STANDARD Timer 1 period = "); Serial.println(Timer1.getPeriod()); // 976

	Timer1.pwm(AUDIO_CHANNEL_1_PIN, AUDIO_BIAS);		// pwm pin, 50% of Mozzi's duty cycle, ie. 0 signal
	//Timer1.attachInterrupt(outputAudio); // TB 15-2-2013 Replaced this line with the ISR, saves some processor time
	TIMSK1 = _BV(TOIE1); 	// Overflow Interrupt Enable (when not using Timer1.attachInterrupt())

#if USING_AUDIO_INPUT
	adcSetupAudioInput();
#endif
}

/* Interrupt service routine moves sound data from the output buffer to the
Arduino output register, running at AUDIO_RATE. */
ISR(TIMER1_OVF_vect, ISR_BLOCK) {
#if USING_AUDIO_INPUT
	sbi(ADCSRA, ADSC);				// start next conversion
	// audioInputToBuffer(); // 1.5us
#endif
	num_out++;
	AUDIO_CHANNEL_1_OUTPUT_REGISTER = output_buffer[(unsigned char)num_out & (unsigned char)(BUFFER_NUM_CELLS-1)]; // 1us, 2.5us with longs
//AUDIO_CHANNEL_1_OUTPUT_REGISTER = output_buffer[(unsigned char)num_out & (unsigned char)(BUFFER_NUM_CELLS-1)]; // 1us, 2.5us with longs

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

#if USING_AUDIO_INPUT
	adcSetupAudioInput()
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
	unsigned int out = output_buffer[num_out++];
	// read about dual pwm at http://www.openmusiclabs.com/learning/digital/pwm-dac/dual-pwm-circuits/
	// sketches at http://wiki.openmusiclabs.com/wiki/PWMDAC,  http://wiki.openmusiclabs.com/wiki/MiniArDSP

	// 14 bit - this sounds better than 12 bit, it's cleaner, less bitty, don't notice aliasing
	AUDIO_CHANNEL_1_HIGHBYTE_REGISTER = out >> 7; // B11111110000000 becomes B1111111
	AUDIO_CHANNEL_1_LOWBYTE_REGISTER = out & 127; // B001111111
	// #if USING_AUDIO_INPUT
	// audioInputToBuffer();
	// #endif
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
