/*
 * mozzi_analog.cpp
 *
 * Copyright 2012 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */


#include "mozzi_config.h"
#include "mozzi_analog.h"
#include "Stack.h"
//#include "mozzi_utils.h"

// required from http://github.com/pedvide/ADC for Teensy 3.0/3.1
// This is a hacky way to access the ADC library, otherwise ADC.h has to be included at the top of every Arduino sketch.
/*#if defined(__MK20DX128__) || defined(__MK20DX256__)
#include "../ADC/ADC_Module.cpp"
#include "../ADC/ADC.cpp"
#endif
*/

void setupFastAnalogRead(int8_t speed)
{
#if defined(__MK20DX128__) || defined(__MK20DX256__) // teensy 3, 3.1
#else
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
#endif
}


/*
void adcEnableInterrupt(){
	ADCSRA |= (1 << ADIE);
}
*/


void setupMozziADC(int8_t speed) {
	#if defined(__MK20DX128__) || defined(__MK20DX256__) // teensy 3, 3.1
	adc = new ADC();
	adc->enableInterrupts(ADC_0);
#else
	ADCSRA |= (1 << ADIE); // adc Enable Interrupt
	setupFastAnalogRead(speed);
	adcDisconnectAllDigitalIns();
#endif
}


void disconnectDigitalIn(uint8_t channel_num){
	#if defined(__MK20DX128__) || defined(__MK20DX256__) // teensy 3, 3.1
#else
	DIDR0 |= 1<<channel_num;
	#endif
}


void reconnectDigitalIn(uint8_t channel_num){
	#if defined(__MK20DX128__) || defined(__MK20DX256__) // teensy 3, 3.1
#else
	DIDR0 &= ~(1<<channel_num);
	#endif
}


void adcDisconnectAllDigitalIns(){
	#if defined(__MK20DX128__) || defined(__MK20DX256__) // teensy 3, 3.1
#else
	for (uint8_t i = 0; i<NUM_ANALOG_INPUTS; i++){
		DIDR0 |= 1<<i;
	}
	#endif
}                     


void adcReconnectAllDigitalIns(){
	#if defined(__MK20DX128__) || defined(__MK20DX256__) // teensy 3, 3.1
#else
	for (uint8_t i = 0; i<NUM_ANALOG_INPUTS; i++){
		DIDR0 &= ~(1<<i);
	}
	#endif
}


uint8_t adcPinToChannelNum(uint8_t pin) {
#if defined(__MK20DX128__) || defined(__MK20DX256__) // teensy 3, 3.1

#else
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
	#endif
	return pin;
}


// assumes channel is correct, not pin number, pin number would be converted first with adcPinToChannelNum
static void adcSetChannel(uint8_t channel) {
#if defined(__MK20DX128__) || defined(__MK20DX256__) // teensy 3, 3.1
// ADC library converts pin/channel each time in startSingleRead
#else
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
#endif
}




	
// basically analogRead() chopped in half so the ADC conversion
// can be started here and received by another function.
void adcStartConversion(uint8_t channel) {
#if defined(__MK20DX128__) || defined(__MK20DX256__) // teensy 3, 3.1
	teensy_pin = channel; // remember for second startSingleRead
	adc->startSingleRead(teensy_pin); // channel/pin gets converted every time in startSingleRead
#else
	adcSetChannel(channel);
#if defined(ADCSRA) && defined(ADCL)
	// start the conversion
	ADCSRA |= (1 << ADSC);
#endif
#endif
}



/*
The code below was informed initially by a discussion between 
jRaskell, bobgardner, theusch, Koshchi, and code by jRaskell. 
http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&p=789581
*/


static volatile int analog_readings[NUM_ANALOG_INPUTS];
static Stack <volatile int8_t,NUM_ANALOG_INPUTS> adc_channels_to_read;
volatile static int8_t current_channel = -1; // volatile because accessed in control and adc ISRs
static bool first = true;


/* Called each time in updateControlWithAutoADC(), after updateControl()
*/
void adcStartReadCycle(){
	if (current_channel == -1) // last read of adc_channels_to_read stack was empty, ie. all channels from last time have been read
	{
#if (USE_AUDIO_INPUT == true)
		adc_channels_to_read.push(AUDIO_INPUT_PIN); // for audio
#else
		adcReadSelectedChannels();
		first = true;		
#endif
	}
}



/* gets the next channel to read off the stack, and if there is a channel there, it changes to that channel and startsa conversion.
*/
void adcReadSelectedChannels() {
	current_channel = adc_channels_to_read.pop();
	if(current_channel != -1) adcStartConversion(current_channel);
}



int mozziAnalogRead(uint8_t pin) {
#if defined(__MK20DX128__) || defined(__MK20DX256__) // teensy 3, 3.1
// ADC lib converts pin/channel in startSingleRead
#else
	pin = adcPinToChannelNum(pin); // allow for channel or pin numbers
#endif
	adc_channels_to_read.push(pin);
	return analog_readings[pin];
}


/*
void receiveFirstControlADC(){
	// do nothing
}
*/


void startSecondControlADC() {
#if defined(__MK20DX128__) || defined(__MK20DX256__) // teensy 3, 3.1
	adc->startSingleRead(teensy_pin);
#else
	ADCSRA |= (1 << ADSC); // start a second conversion on the current channel
#endif
}


void receiveSecondControlADC(){
#if defined(__MK20DX128__) || defined(__MK20DX256__) // teensy 3, 3.1
	analog_readings[current_channel] = adc->readSingle();
#else
	analog_readings[current_channel] = ADC; // officially (ADCL | (ADCH << 8)) but the compiler works it out
#endif
}


/* This interrupt handler cycles through all analog inputs on the adc_channels_to_read Stack,
doing 2 conversions on each channel but only keeping the second conversion each time, 
because the first conversion after changing channels is often inaccurate (on atmel-based arduinos).

The version for USE_AUDIO_INPUT==true is in MozziGuts.cpp... compilation reasons...
*/
#if(USE_AUDIO_INPUT==false)
#if defined(__MK20DX128__) || defined(__MK20DX256__) // teensy 3, 3.1
void adc0_isr(void) 
#else
ISR(ADC_vect, ISR_BLOCK) 
#endif
{
	if (first)
	{
    //<1us
   	startSecondControlADC();
   	first=false;
  }
  else
  {	
  	// 3us
    receiveSecondControlADC();
    adcReadSelectedChannels();
   	first=true;
	}
}
#endif

