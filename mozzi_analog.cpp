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
#include "Stack.h"
//#include "mozzi_utils.h"

void setupFastAnalogRead(char speed)
{
	if (speed == FAST_ADC){ // divide by 16
		ADCSRA |= (1 << ADPS2);
		ADCSRA &= ~(1 << ADPS1);
		ADCSRA &= ~(1 << ADPS0);
	} else if(speed == FASTER_ADC){ // divide by 8
		ADCSRA &= ~(1 << ADPS2);
		ADCSRA |= (1 << ADPS1);
		ADCSRA |= (1 << ADPS0);
	} else if(speed == FASTEST_ADC){ // divide by 4
		ADCSRA &= ~(1 << ADPS2);
		ADCSRA |= (1 << ADPS1);
		ADCSRA &= ~(1 << ADPS0);
	}
}

/*
void adcEnableInterrupt(){
	ADCSRA |= (1 << ADIE);
}
*/

void setupMozziADC(char speed) {
	ADCSRA |= (1 << ADIE); // adc Enable Interrupt
	setupFastAnalogRead(speed);
	adcDisconnectAllDigitalIns();
}



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


unsigned char adcPinToChannelNum(unsigned char pin) {
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	if (pin >= 54) pin -= 54; // allow for channel or pin numbers
#elif defined(__AVR_ATmega32U4__)
	if (pin >= 18) pin -= 18; // allow for channel or pin numbers
	pin = analogPinToChannel(pin); // moved from extra #if which was below in Arduino code, and redefined in mozzi_analog.h, with notes
#elif defined(__AVR_ATmega1284__)
	if (pin >= 24) pin -= 24; // allow for channel or pin numbers
#else
	if (pin >= 14) pin -= 14; // allow for channel or pin numbers
#endif
	
	return pin;
}


// assumes channel is correct, not pin number, pin number would be converted first with adcPinToChannelNum
static void adcSetChannel(unsigned char channel) {
#if defined(__AVR_ATmega32U4__)
	ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((channel >> 3) & 0x01) << MUX5);
#elif defined(ADCSRB) && defined(MUX5)
	// the MUX5 bit of ADCSRB selects whether we're reading from channels
	// 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
	ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((channel >> 3) & 0x01) << MUX5);
#endif

	// set the analog reference (high two bits of ADMUX) and select the
	// channel (low 4 bits).  this also sets ADLAR (left-adjust result)
	// to 0 (the default).
#if defined(ADMUX)
	ADMUX = (1 << REFS0) | (channel & 0x07);
#endif
}




// basically analogRead() chopped in half so the ADC conversion
// can be started here and received by another function.
void adcStartConversion(unsigned char channel) {
	adcSetChannel(channel);
#if defined(ADCSRA) && defined(ADCL)
	// start the conversion
	ADCSRA |= (1 << ADSC);
#endif
}



/*
The code below was informed initially by a discussion between 
jRaskell, bobgardner, theusch, Koshchi, and code by jRaskell. 
http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&p=789581
*/


static volatile int analog_readings[NUM_ANALOG_INPUTS];
static Stack <volatile char,NUM_ANALOG_INPUTS> adc_channels_to_read;
volatile static char current_channel = -1; // volatile because accessed in control and adc ISRs
static bool first = true;


void adcStartReadCycle(){
	if (current_channel == -1) {
#if (USE_AUDIO_INPUT == true)
		adc_channels_to_read.push(0); // analog 0 is for audio
#else
		adcReadSelectedChannels();
		first = true;		
#endif
	}
}



int mozziAnalogRead(unsigned char pin) {
	//analogPinToChannel<pin>;
	pin = adcPinToChannelNum(pin); // allow for channel or pin numbers
	adc_channels_to_read.push(pin);
	return analog_readings[pin];
}



/* gets the next channel to read off the stack, and if there is a channel there, it changes to that channel and startsa conversion.
*/
void adcReadSelectedChannels() {
	current_channel = adc_channels_to_read.pop();
	if(current_channel != -1) adcStartConversion(current_channel);
}

/*
void receiveFirstControlADC(){
	// do nothing
}
*/

void startSecondControlADC() {
	ADCSRA |= (1 << ADSC); // start a second conversion on the current channel
}


void receiveSecondControlADC(){
	analog_readings[current_channel] = ADC; // officially (ADCL | (ADCH << 8)) but the compiler works it out
}


/* This interrupt handler cycles through all analog inputs on the adc_channels_to_read Stack,
doing 2 conversions on each channel but only keeping the second conversion each time, 
because the first conversion after changing channels is often inaccurate.
*/
#if(USE_AUDIO_INPUT==false)

ISR(ADC_vect, ISR_BLOCK) {
	if (first){
      	//<1us
      	startSecondControlADC();
      	first=false;
  	}else{	
  		// 3us
      	receiveSecondControlADC();
      	adcReadSelectedChannels();
      	first=true;
	}
}

#endif



