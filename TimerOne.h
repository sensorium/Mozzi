/*
 *  Interrupt and PWM utilities for 16 bit Timer1 on ATmega168/328
 *  Original code by Jesse Tane for http://labs.ideo.com August 2008
 *  Modified March 2009 by Jérôme Despatis and Jesse Tane for ATmega328 support
 *  Modified June 2009 by Michael Polli and Jesse Tane to fix a bug in setPeriod() which caused the timer to stop
 *  Modified April 2012 by Paul Stoffregen - portable to other AVR chips, use inline functions
 *  Modified March 2013 by Tim Barrass to use fast PWM for Mozzi, instead of phase and frequency correct.
 *  This is free software. You can redistribute it and/or modify it under
 *  the terms of Creative Commons Attribution 3.0 United States License.
 *  To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/us/
 *  or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 */

#ifndef TimerOne_h_
#define TimerOne_h_

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "config/known_16bit_timers.h"

#define TIMER1_RESOLUTION 65536UL  // Timer1 is 16 bit


// Placing nearly all the code in this .h file allows the functions to be
// inlined by the compiler.  In the very common case with constant values
// the compiler will perform all calculations and simply write constants
// to the hardware registers (for example, setPeriod).

// TB2013
// Modes are in ATmega48P/88P/168P/328P technical notes, Table 13-4
// PWM, Phase and Frequency Correct, TOP is ICR1
//#define TCCR1B_PWM_MODE_BITS  _BV(WGM13)

// Fast PWM, PWM, TOP is ICR1 ( this should go twice as fast
//#define TCCR1B_PWM_MODE_BITS _BV(WGM13) | _BV(WGM12)
//#define TCCR1A_PWM_MODE_BITS _BV(WGM11)
//const unsigned char TCCR1B_PWM_MODE_BITS = _BV(WGM13) | _BV(WGM12);
//const unsigned char TCCR1A_PWM_MODE_BITS = _BV(WGM11);
// TB2013 todo (to improve the Library): make the mode set-able instead of hard-coded
#define FAST 0
#define PHASE_FREQ_CORRECT 1

class TimerOne
{
public:
	//****************************
	//  Configuration
	//****************************
	
	
	// TB2014 different init methods for microseconds or cpu_cycles
	void initializeMicroseconds(unsigned long microseconds=1000000, unsigned char mode = PHASE_FREQ_CORRECT) __attribute__((always_inline))
	{
		setMode(mode);
		setPeriodCPUCycles(microseconds*(F_CPU/1000000));
	}
	
	
	void initializeCPUCycles(unsigned long cpu_cyles=16000000, unsigned char mode = PHASE_FREQ_CORRECT) __attribute__((always_inline))
	{
		setMode(mode);
		setPeriodCPUCycles(cpu_cyles);
	}
	
	
	void setPeriodCPUCycles(unsigned long sixteen_millionths_of_a_second) __attribute__((always_inline))
	{
		unsigned long cycles;
	
		// TB2013
		if (_mode == FAST){
			// TB2014
			cycles = sixteen_millionths_of_a_second;
		}else if (_mode == PHASE_FREQ_CORRECT){
			// TB2014
			cycles = sixteen_millionths_of_a_second >> 1; // has to go twice as fast to climb up and down in the same period as FAST mode: you get 0.5 the counter range
		}
		
		// Serial.println(cycles);
		// end TB2013
		
		if (cycles < TIMER1_RESOLUTION)
		{
			clockSelectBits = _BV(CS10);
			pwmPeriod = cycles;
		}
		else
			if (cycles < TIMER1_RESOLUTION * 8)
			{
				clockSelectBits = _BV(CS11);
				pwmPeriod = cycles / 8;
			}
			else
				if (cycles < TIMER1_RESOLUTION * 64)
				{
					clockSelectBits = _BV(CS11) | _BV(CS10);
					pwmPeriod = cycles / 64;
				}
				else
					if (cycles < TIMER1_RESOLUTION * 256)
					{
						clockSelectBits = _BV(CS12);
						pwmPeriod = cycles / 256;
					}
					else
						if (cycles < TIMER1_RESOLUTION * 1024)
						{
							clockSelectBits = _BV(CS12) | _BV(CS10);
							pwmPeriod = cycles / 1024;
						}
						else
						{
							clockSelectBits = _BV(CS12) | _BV(CS10);
							pwmPeriod = TIMER1_RESOLUTION - 1;
						}
		ICR1 = pwmPeriod;
		TCCR1B = TCCR1B_PWM_MODE_BITS | clockSelectBits;
	}

	//****************************
	//  Run Control
	//****************************
	void start() __attribute__((always_inline))
	{
		TCCR1B = 0;
		TCNT1 = 0;		// TODO: does this cause an undesired interrupt?
		TCCR1B = TCCR1B_PWM_MODE_BITS | clockSelectBits;
	}
	void stop() __attribute__((always_inline))
	{
		TCCR1B = TCCR1B_PWM_MODE_BITS;
	}
	void restart() __attribute__((always_inline))
	{
		TCCR1B = 0;
		TCNT1 = 0;
		TCCR1B = TCCR1B_PWM_MODE_BITS | clockSelectBits;
	}
	void resume() __attribute__((always_inline))
	{
		TCCR1B = TCCR1B_PWM_MODE_BITS | clockSelectBits;
	}

	//****************************
	//  PWM outputs
	//****************************
	void setPwmDuty(char pin, unsigned int duty) __attribute__((always_inline))
	{
		unsigned long dutyCycle = pwmPeriod;
		dutyCycle *= duty;
		dutyCycle >>= 10;
		if (pin == TIMER1_A_PIN)
			OCR1A = dutyCycle;
#ifdef TIMER1_B_PIN

		else if (pin == TIMER1_B_PIN)
			OCR1B = dutyCycle;
#endif
	#ifdef TIMER1_C_PIN

		else if (pin == TIMER1_C_PIN)
			OCR1C = dutyCycle;
#endif
	}
	
	
	
	void pwm(char pin, unsigned int duty) __attribute__((always_inline))
	{
		if (pin == TIMER1_A_PIN)
		{
			pinMode(TIMER1_A_PIN, OUTPUT);
			TCCR1A |= _BV(COM1A1) | TCCR1A_PWM_MODE_BITS;
		}
#ifdef TIMER1_B_PIN
		else if (pin == TIMER1_B_PIN)
		{
			pinMode(TIMER1_B_PIN, OUTPUT);
			TCCR1A |= _BV(COM1B1) | TCCR1A_PWM_MODE_BITS;
		}
#endif
	#ifdef TIMER1_C_PIN
		else if (pin == TIMER1_C_PIN)
		{
			pinMode(TIMER1_C_PIN, OUTPUT);
			TCCR1A |= _BV(COM1C1) | TCCR1A_PWM_MODE_BITS;
		}
#endif
		setPwmDuty(pin, duty);
		TCCR1B = TCCR1B_PWM_MODE_BITS | clockSelectBits;
	}
	
	
	
	void pwm(char pin, unsigned int duty, unsigned long microseconds) __attribute__((always_inline))
	{
		if (microseconds > 0)
			setPeriodCPUCycles(microseconds*16);
		pwm(pin, duty);
	}
	void disablePwm(char pin) __attribute__((always_inline))
	{
		if (pin == TIMER1_A_PIN) {
			TCCR1A &= ~_BV(COM1A1);
			TCCR1A &= ~TCCR1A_PWM_MODE_BITS; }
#ifdef TIMER1_B_PIN

		else if (pin == TIMER1_B_PIN) {
			TCCR1A &= ~_BV(COM1B1);
			TCCR1A &= ~TCCR1A_PWM_MODE_BITS; }
#endif
	#ifdef TIMER1_C_PIN

		else if (pin == TIMER1_C_PIN) {
			TCCR1A &= ~_BV(COM1C1);
			TCCR1A &= ~TCCR1A_PWM_MODE_BITS; }
#endif

	}

	
	
	//****************************
	//  Interrupt Function
	//****************************
	void attachInterrupt(void (*isr)()) __attribute__((always_inline))
	{
		isrCallback = isr;
		TIMSK1 = _BV(TOIE1);
	}
	
	
	
	void attachInterrupt(void (*isr)(), unsigned long microseconds) __attribute__((always_inline))
	{
		if(microseconds > 0)
			setPeriodCPUCycles(microseconds*16);
		attachInterrupt(isr);
	}
	
	
	
	void detachInterrupt() __attribute__((always_inline))
	{
		TIMSK1 = 0;
	}
	
	
	void (*isrCallback)();

	
	
	unsigned int getPeriod(){
		return pwmPeriod;
	}
	
	
	
protected:
	// properties
	static unsigned int pwmPeriod;
	static unsigned char clockSelectBits;
	// TB2013
	unsigned char _mode;
	unsigned char TCCR1B_PWM_MODE_BITS;
	unsigned char TCCR1A_PWM_MODE_BITS;
	
private:
	//TB2013, moved to separate function 2014 to enable init with usecs or cpu cycles
	void setMode(unsigned char mode = PHASE_FREQ_CORRECT) __attribute__((always_inline))
	{
		_mode = mode;
		if (_mode == FAST){
			TCCR1B_PWM_MODE_BITS = _BV(WGM13) | _BV(WGM12);
			TCCR1A_PWM_MODE_BITS = _BV(WGM11);
		}else if (_mode == PHASE_FREQ_CORRECT){
			TCCR1B_PWM_MODE_BITS  = _BV(WGM13);
			TCCR1A_PWM_MODE_BITS = 0;
		}
		TCCR1B = TCCR1B_PWM_MODE_BITS;        // set mode as fast or phase and frequency correct pwm, stop the timer
		TCCR1A = TCCR1A_PWM_MODE_BITS;        // set control register A
	}
	
};

extern TimerOne Timer1;

#endif

