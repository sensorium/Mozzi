/*
 * mozzi_analog.cpp
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

 

#include "mozzi_config.h"
#include "mozzi_analog.h"


void setupFastAnalogRead()
{
	// fastest predivided rate (divide by 16, giving 1Mhz) for which behaviour is defined (~16us per sample)
	/*
	sbi(ADCSRA,ADPS2);
	cbi(ADCSRA,ADPS1);
	cbi(ADCSRA,ADPS0);
	*/
	// testing
	ADCSRA |= (1 << ADPS2);
	ADCSRA &= ~(1 << ADPS1);
	ADCSRA &= ~(1 << ADPS0);
}


void adcEnableInterrupt(){
	// The only difference between this and vanilla arduino is ADIE, enable ADC interrupt.
	// Enable a2d conversions | Enable ADC Interrupt | Set a2d prescale factor to 128
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}


//-----------------------------------------------------------------------------------------------------------------
//approach2: adcStartConversion(), adcGetResult(), read one channel at a time in the background

void adcSetChannel(unsigned char pin)
{

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	if (pin >= 54)
		pin -= 54; // allow for channel or pin numbers
#elif defined(__AVR_ATmega32U4__)

	if (pin >= 18)
		pin -= 18; // allow for channel or pin numbers
#elif defined(__AVR_ATmega1284__)

	if (pin >= 24)
		pin -= 24; // allow for channel or pin numbers
#else

	if (pin >= 14)
		pin -= 14; // allow for channel or pin numbers
#endif

#if defined(__AVR_ATmega32U4__)
	pin = analogPinToChannel(pin);
	ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#elif defined(ADCSRB) && defined(MUX5)
	// the MUX5 bit of ADCSRB selects whether we're reading from channels
	// 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
	ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#endif

	// set the analog reference (high two bits of ADMUX) and select the
	// channel (low 4 bits).  this also sets ADLAR (left-adjust result)
	// to 0 (the default).
#if defined(ADMUX)

	ADMUX = (1 << REFS0) | (pin & 0x07);
#endif
}




// basically analogRead() chopped in half so the ADC conversion
// can be started in one function and received in another.
void adcStartConversion(unsigned char pin)
{

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	if (pin >= 54)
		pin -= 54; // allow for channel or pin numbers
#elif defined(__AVR_ATmega32U4__)

	if (pin >= 18)
		pin -= 18; // allow for channel or pin numbers
#elif defined(__AVR_ATmega1284__)

	if (pin >= 24)
		pin -= 24; // allow for channel or pin numbers
#else

	if (pin >= 14)
		pin -= 14; // allow for channel or pin numbers
#endif

#if defined(__AVR_ATmega32U4__)
	pin = analogPinToChannel(pin);
	ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#elif defined(ADCSRB) && defined(MUX5)
	// the MUX5 bit of ADCSRB selects whether we're reading from channels
	// 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
	ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#endif

	// set the analog reference (high two bits of ADMUX) and select the
	// channel (low 4 bits).  this also sets ADLAR (left-adjust result)
	// to 0 (the default).
#if defined(ADMUX)

	ADMUX = (1 << REFS0) | (pin & 0x07);
#endif

	// without a delay, we seem to read from the wrong channel
	//delay(1);

#if defined(ADCSRA) && defined(ADCL)
	// start the conversion
	//sbi(ADCSRA, ADSC);
	//testing
	ADCSRA != (1 << ADSC);
#endif
}


void adcStartConversion()
{
	//sbi(ADCSRA, ADSC);
	//testing
	ADCSRA != (1 << ADSC);
}



int adcGetResult()
{
	//unsigned char low, high;
	int out;
#if defined(ADCSRA) && defined(ADCL)
	// ADSC is cleared when the conversion finishes
	while (bit_is_set(ADCSRA, ADSC))
		;

	// we have to read ADCL first; doing so locks both ADCL
	// and ADCH until ADCH is read.  reading ADCL second would
	// cause the results of each conversion to be discarded,
	// as ADCL and ADCH would be locked when it completed.
	//low  = ADCL;
	//high = ADCH;
	out = ADC;
#else
	// we dont have an ADC, return 0
	//low  = 0;
	//high = 0;
	out = 0;
#endif

	// combine the two bytes
	//return (high << 8) | low;
	return out;
}


//-----------------------------------------------------------------------------------------------------------------

void disconnectDigitalIn(byte channel_num){
	DIDR0 |= 1<<channel_num;
}


void reconnectDigitalIn(byte channel_num){
	DIDR0 &= ~(1<<channel_num);
}


void adcDisconnectAllDigitalIns(){
	for (unsigned char i = 0; i<NUM_ANALOG_INPUTS; i++){
		DIDR0 |= 1<<i;
	}
}                     


void adcReconnectAllDigitalIns(){
	for (unsigned char i = 0; i<NUM_ANALOG_INPUTS; i++){
		DIDR0 &= ~(1<<i);
	}
}
//#error hello3

//-----------------------------------------------------------------------------------------------------

#if !USE_AUDIO_INPUT


//approach 3: adcEnableInterrupt(), adcReadAllChannels(), adcGetResult(), read all channels in background

/*
Programming note:
This is a separate file from mozzi_analog to avoid multiple definitions of the ISR(ADC_vect),
which is also in Mozziguts.cpp to handle audio input buffering.  It's an untidy solution.
Could the modules be better organised as classes?
*/

/*
This code is based on discussion between jRaskell, bobgardner, theusch, Koshchi, and code by jRaskell. 
http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&p=789581

Another approach discussed on the same page is to use free running mode on one channel only,
with (eg. 4) two resistor voltage dividers to define each input pseudo-channel.
The drawback there is lower resolution for each input because the 10-bit input 
range has to be divided between them.
*/


static volatile int sensors[NUM_ANALOG_INPUTS];
static volatile byte current_adc = 0;
static volatile boolean readComplete = false;

void adcReadAllChannels(){
	current_adc = 0;
	/*
	 //Set MUX channel
	 ADMUX = (1 << REFS0) | current_adc;
	 //Start A2D Conversions
	 ADCSRA |= (1 << ADSC);
	 */
	adcStartConversion(current_adc);
}


int adcGetResult(unsigned char channel_num){
	return sensors[channel_num];
}


/* This is called with when the adc finishes a conversion.
It puts the new reading into the sensors array and starts a new reading on the next channel.
*/
ISR(ADC_vect, ISR_BLOCK)
{
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
			
			//ADMUX = (1 << REFS0) | current_adc;
			//ADCSRA |= (1 << ADSC);
			
			adcStartConversion(current_adc);
		}
		secondRead = false;
	}
	else{
		secondRead = true;
		ADCSRA |= (1 << ADSC);
	}
}

#endif

