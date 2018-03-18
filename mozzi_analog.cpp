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

#include "hardware_defines.h"
#if IS_TEENSY3()
// required from http://github.com/pedvide/ADC for Teensy 3.*
#include <ADC.h>
#elif IS_STM32()
#include <STM32ADC.h>
#endif

// defined in Mozziguts.cpp
#if IS_TEENSY3()
	extern ADC *adc; // adc object
	extern uint8_t teensy_pin;
#elif IS_STM32()
	extern STM32ADC adc;
	extern uint8_t stm32_current_adc_pin;
	void stm32_adc_eoc_handler();
#endif

extern uint8_t analog_reference;

void setupFastAnalogRead(int8_t speed)
{
#if IS_AVR()
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
#elif IS_STM32()
	// NOTE: These picks are pretty arbitrary. Further available options are 7_5, 28_5, 55_5, 71_5 and 239_5 (i.e. 7.5 ADC cylces, etc.)
	if (speed == FASTEST_ADC) adc.setSampleRate(ADC_SMPR_1_5);
	else if (speed == FASTER_ADC) adc.setSampleRate(ADC_SMPR_13_5);
        else (adc.setSampleRate(ADC_SMPR_41_5));
#endif
}


/*
void adcEnableInterrupt(){
	ADCSRA |= (1 << ADIE);
}
*/


void setupMozziADC(int8_t speed) {
#if IS_TEENSY3()
	adc = new ADC();
	adc->enableInterrupts(ADC_0);
#elif IS_STM32()
	adc.calibrate();
	setupFastAnalogRead(speed);
	adc.attachInterrupt(stm32_adc_eoc_handler, ADC_EOC);
#elif IS_AVR()
	ADCSRA |= (1 << ADIE); // adc Enable Interrupt
	setupFastAnalogRead(speed);
	adcDisconnectAllDigitalIns();
#else
#warning Fast ADC not implemented on this platform
#endif
}


void disconnectDigitalIn(uint8_t channel_num){
#if IS_AVR()
	DIDR0 |= 1<<channel_num;
#endif
}


void reconnectDigitalIn(uint8_t channel_num){
#if IS_AVR()
	DIDR0 &= ~(1<<channel_num);
#endif
}


void adcDisconnectAllDigitalIns(){
#if IS_AVR()
	for (uint8_t i = 0; i<NUM_ANALOG_INPUTS; i++){
		DIDR0 |= 1<<i;
	}
#endif
}


void adcReconnectAllDigitalIns(){
#if IS_AVR()
	for (uint8_t i = 0; i<NUM_ANALOG_INPUTS; i++){
		DIDR0 &= ~(1<<i);
	}
#endif
}


uint8_t adcPinToChannelNum(uint8_t pin) {

#if IS_AVR()
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
#if IS_AVR()
#if defined(__AVR_ATmega32U4__)
	ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((channel >> 3) & 0x01) << MUX5);
#elif defined(ADCSRB) && defined(MUX5)
	// the MUX5 bit of ADCSRB selects whether we're reading from channels
	// 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
	ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((channel >> 3) & 0x01) << MUX5);
#endif

// from wiring_analog.c:
// set the analog reference (high two bits of ADMUX) and select the
// channel (low 4 bits).  this also sets ADLAR (left-adjust result)
// to 0 (the default).
#if defined(ADMUX)
#if defined(TEENSYDUINO) // analog_reference is not part TEENSY 2.0 codebase
    ADMUX = (1 << REFS0) | (channel & 0x07); // TB2017 this overwrote analog_reference
#else
	ADMUX = (analog_reference << 6) | (channel & 0x07);
#endif
#endif
#else
// For other platforms ADC library converts pin/channel each time in startSingleRead
#endif
}





// basically analogRead() chopped in half so the ADC conversion
// can be started here and received by another function.
void adcStartConversion(uint8_t channel) {
#if IS_TEENSY3()
	teensy_pin = channel; // remember for second startSingleRead
	adc->startSingleRead(teensy_pin); // channel/pin gets converted every time in startSingleRead
#elif IS_STM32()
	stm32_current_adc_pin = channel;
	adc.setPins(&stm32_current_adc_pin, 1);
	adc.startConversion();
#elif IS_AVR()
	adcSetChannel(channel);
#if defined(ADCSRA) && defined(ADCL)
	// start the conversion
	ADCSRA |= (1 << ADSC);
#endif
#else
#warning Fast analog read not implemented on this platform
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
#if IS_ESP8266()
#warning Asynchronouos analog reads not implemented for this platform
	return analogRead(pin);
#else
// ADC lib converts pin/channel in startSingleRead
#if IS_AVR()
	pin = adcPinToChannelNum(pin); // allow for channel or pin numbers
#endif
	adc_channels_to_read.push(pin);
	return analog_readings[pin];
#endif
}


/*
void receiveFirstControlADC(){
	// do nothing
}
*/


void startSecondControlADC() {
#if IS_TEENSY3()
	adc->startSingleRead(teensy_pin);
#elif IS_STM32()
	adc.setPins(&stm32_current_adc_pin, 1);
	adc.startConversion();
#elif IS_AVR()
	ADCSRA |= (1 << ADSC); // start a second conversion on the current channel
#endif
}


void receiveSecondControlADC(){
#if IS_TEENSY3()
	analog_readings[current_channel] = adc->readSingle();
#elif IS_STM32()
	analog_readings[current_channel] = adc.getData();
#elif IS_AVR()
	analog_readings[current_channel] = ADC; // officially (ADCL | (ADCH << 8)) but the compiler works it out
#endif
}


/* This interrupt handler cycles through all analog inputs on the adc_channels_to_read Stack,
doing 2 conversions on each channel but only keeping the second conversion each time,
because the first conversion after changing channels is often inaccurate (on atmel-based arduinos).

The version for USE_AUDIO_INPUT==true is in MozziGuts.cpp... compilation reasons...
*/
#if(USE_AUDIO_INPUT==false)
#if IS_TEENSY3()
void adc0_isr(void)
#elif IS_STM32()
void stm32_adc_eoc_handler()
#elif IS_AVR()
ISR(ADC_vect, ISR_BLOCK)
#endif
#if IS_TEENSY3() || IS_STM32() || IS_AVR()
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
#endif
