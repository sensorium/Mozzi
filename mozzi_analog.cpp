
#include "mozzi_analog.h"
#include "Arduino.h"

static unsigned char analog_reference = DEFAULT;

///approach 1: just make analogRead faster //////////////////////////////////////////////////////////////////////////////////

/** @ingroup analog
Make analogRead() faster than the standard Arduino version, changing the
duration from about 105 in unmodified Arduino to 15 microseconds for a
dependable analogRead(). Put this in setup() if you intend to use analogRead()
with Mozzi, to avoid glitches.
Don't use it with the adcEnableInterrupt(), adcReadAllChannels(), adcGetResult() approach, it may contribute to glitches.
See: http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1208715493/11, and
http://www.marulaberry.co.za/index.php/tutorials/code/arduino-adc/
*/
void setupFastAnalogRead()
{
	// fastest predivided rate (divide by 16, giving 1Mhz) for which behaviour is defined (~16us per sample)
	sbi(ADCSRA,ADPS2);
	cbi(ADCSRA,ADPS1);
	cbi(ADCSRA,ADPS0);
}


/** @ingroup analog
Call this in setup() to enable reading analog inputs in the background while audio generating continues.
Then call adcReadAllChannels() at the end of each updateControl() and the results for each analog channel will be
available by calling adcGetResult(channel_num) next time updateControl() runs.
@note This method using adcEnableInterrupt(), adcReadAllChannels() and adcGetResult() is an easy and
efficient way to read analog inputs while generating sound with Mozzi. For many
sketches, however, simply putting setupFastAnalogRead() in setup() and calling
Arduino's usual analogRead() will work fast enough.
@note DON"T USE setupFastAnalogRead() with adcEnableInterrupt.
It may cause the ADC process to hog the processor, causing audio glitches.
@note In some cases this method can cause glitches which may have to do with the ADC
interrupt interfering with the audio or control interrupts. If this occurs, use
the startAnalogRead(), receiveAnalogRead() methods instead.
*/
void adcEnableInterrupt(){
	// The only difference between this and vanilla arduino is ADIE, enable ADC interrupt.
	// Enable a2d conversions | Enable ADC Interrupt | Set a2d prescale factor to 128
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}


///approach 3: startAnalogRead(), receiveAnalogRead(), read one channel at a time in the background///////

/** @ingroup analog
Set the channel or pin for the next analog input to be read from.
@param channel or pin number.  If pin number is provided, adcSetChannel will convert it to the channel number.
*/

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

	ADMUX = (analog_reference << 6) | (pin & 0x07);
#endif
}

/** @ingroup analog
Starts an analog-to-digital conversion of the voltage at a specified pin.  Unlike
Arduino's analogRead() function which waits until a conversion is complete before
returning, startAnalogRead() only sets the conversion to begin, so you can use
the cpu for other things and call for the result later with receiveAnalogRead().
This works well in updateControl(), where you can call startAnalogRead() and
get the result with receiveAnalogRead() the next time the updateControl()
interrupt runs.
This technique should also be suitable for audio-rate sampling of a single channel in updateAudio() calls.
@param pin is the analog pin number (A0 to A...) or the channel number (0 to ....) to read.
@note This is the most audio-friendly way to read analog inputs,
but can be messier in your program than the the adcEnableInterrupt(), adcReadAllChannels(), adcGetResult() way.
@note Timing: about 1us when used in updateControl() with CONTROL_RATE 64.
*/

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

	ADMUX = (analog_reference << 6) | (pin & 0x07);
#endif

	// without a delay, we seem to read from the wrong channel
	//delay(1);

#if defined(ADCSRA) && defined(ADCL)
	// start the conversion
	sbi(ADCSRA, ADSC);
#endif
}

/** @ingroup analog
Starts the analog conversion on the pin or channel most recently set with adcSetChannel();
*/
void adcStartConversion()
{
	sbi(ADCSRA, ADSC);
}


/** @ingroup analog
Waits for the result of the most recent startAnalogRead().  If used as the first function
of updateControl(), to receive the result of startAnalogRead() from the end of the last
updateControl(), there will probably be no waiting time, as the ADC conversion will
have happened in between interrupts.  This is a big time-saver, since you don't have to
waste time waiting for analogRead() to return (1us here vs 105 us for standard Arduino).
@return The resut of the most recent startAnalogRead().
@note This is the most audio-friendly way to read analog inputs,
but can be messier in your program than the the adcEnableInterrupt(), adcReadAllChannels(), adcGetResult(byte) way.
@note Timing: about 1us when used in updateControl() with CONTROL_RATE 64.
*/
//int receiveAnalogRead()
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


///useful for all approaches////////////////////////////////////////////////////////////////////////////////////////////

/**  @ingroup analog
Prepare an analog input channel by turning off its digital input buffer.
This helps to reduce noise, increase analog reading speed, and save power.

Here's more detail from http://www.openmusiclabs.com/learning/digital/atmega-adc/:

The DIDR (Data Input Disable Register) disconnects the digital inputs from
whichever ADC channels you are using. This is important for 2 reasons. First
off, an analog input will be floating all over the place, and causing the
digital input to constantly toggle high and low. This creates excessive noise
near the ADC, and burns extra power. Secondly, the digital input and associated
DIDR switch have a capacitance associated with them which will slow down your
input signal if you are sampling a highly resistive load.

And from the ATmega328p datasheet, p266:

When an analog signal is applied to the ADC pin and the digital input from
this pin is not needed, this bit should be written logic one to reduce power
consumption in the digital input buffer. Note that ADC named_pins ADC7 
and ADC6 do not have digital input buffers, and therefore do not require 
Digital Input Disable bits.
@param channel_num the analog input channel you wish to use.
*/
void disconnectDigitalIn(byte channel_num){
	DIDR0 |= 1<<channel_num;
}


/** @ingroup analog
Reconnect the digital input buffer for an analog input channel which has
been set for analog input with disconnectDigitalIn().
@param channel_num the analog input channel you wish to reconnect.
*/
void reconnectDigitalIn(byte channel_num){
	DIDR0 &= ~(1<<channel_num);
}


/**  @ingroup analog
Prepare all analog input channels by turning off their digital input buffers.
This helps to reduce noise, increase analog reading speed, and save power.
*/
void adcDisconnectAllDigitalIns(){
	for (unsigned char i = 0; i<NUM_ANALOG_INPUTS; i++){
		DIDR0 |= 1<<i;
	}
}


/** @ingroup analog
Reconnect the digital input buffers for analog input channels which have
been set for analog input with disconnectDigitalIn().
*/
void adcReconnectAllDigitalIns(){
	for (unsigned char i = 0; i<NUM_ANALOG_INPUTS; i++){
		DIDR0 &= ~(1<<i);
	}
}
