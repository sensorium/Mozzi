/*
This code is from
http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&p=789581
discussion between jRaskell, bobgardner, theusch, Koshchi, code by jRaskell. 

Another approach discussed on the same page is to use free running mode on one channel only,
with (eg. 4) two resistor voltage dividers to define each input pseudo-channel.
The drawback there is lower resolution for each input because the 10-bit input 
range has to be divided between them.

*/
#include "mozzi_analog.h"


/** @ingroup analog
Make analogRead() faster than the standard Arduino version, changing the
duration from about 105 in unmodified Arduino to 15 microseconds for a
dependable analogRead(). Put this in setup() if you intend to use analogRead()
with Mozzi, to avoid glitches.
Don't use it with the initADC(), startRead(), getSensor() approach, it may contribute to glitches.
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


///approach 1: initADC(), startRead(), getSensor() //////////////////////////////////////////////////////////////////////////////////

static volatile int sensors[NUM_ANALOG_INPUTS];
static volatile byte current_adc = 0;
static volatile boolean readComplete = false; 

/** @ingroup analog
Call this in setup() to enable reading analog inputs in the background while audio generating continues.
Then call startRead() at the end of each updateControl() and the results for each analog channel will be
available by calling getSensor(channel_num) next time updateControl() runs.
@note This method using initADC(), startRead() and getSensor() is an easy and
efficient way to read analog inputs while generating sound with Mozzi. For many
sketches, however, simply putting setupFastAnalogRead() in setup() and calling
Arduino's usual analogRead() will work fast enough.
@note In some cases this method can cause glitches which may have to do with the ADC
interrupt interfering with the audio or control interrupts. If this occurs, use
the startAnalogRead(), receiveAnalogRead() methods instead.
*/
void initADC(){
// The only difference between this and vanilla arduino is ADIE, enable ADC interrupt.
// Enable a2d conversions | Enable ADC Interrupt | Set a2d prescale factor to 128
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); 
}



/** @ingroup analog
Call startRead() at the end of each updateControl() and the reading will happen in the background, 
using a minimum of processor time and without blocking other code.
The results for each analog channel are available by calling getSensor(channel_num) next time updateControl() runs.

More detail: startRead() starts an initial conversion which triggers an interrupt when it's complete.
The interrupt code stores the result in an array, changes to the next channel and
starts another conversion.  When all the channels have been sampled, the ISR doesn't
start a new conversion, so it doesn't re-trigger itself or use processor time until
startRead() is called again.  At any time the latest conversion result for each channel is
available by calling getSensor(channel_num).
@note In some cases this method can cause glitches which may have to do with the ADC interrupt
interfering with the audio or control interrupts.  
*/
void startRead(){
  current_adc = 0;
 /*
  //Set MUX channel
  ADMUX = (1 << REFS0) | current_adc;
  //Start A2D Conversions
  ADCSRA |= (1 << ADSC);
  */
  startAnalogRead(current_adc);
}


// TB25-02-13 added ISR_NOBLOCK to see if it helps with glitches
// maybe need ATOMIC around int parts too?
// or a way to spread out the reads so the interrupt doesn't get called 
// NUM_ANALOG_INPUTS times, quick in a row 

ISR(ADC_vect, ISR_NOBLOCK){
  static boolean secondRead = false;
 
  //Only record the second read on each channel
  if(secondRead){
    sensors[current_adc] = ADCL | (ADCH << 8); 
    //bobgardner: ..The compiler is clever enough to read the 10 bit value like this: val=ADC;
    //sensors[current_adc] = ADC;
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
 
} 



/** @ingroup analog
This returns the most recent analog reading for the specified channel.
@param channel_num The channels are plain numbers 0 to whatever your board goes up to, not the pin
labels A0 to A... which Arduino maps to different numbers depending on the board
being used.
@note The InitADC(), startRead(), getSensor() approach is currently set to work with
all channels on each kind of board. You can change the number of channels to use in
mozzi_analog.cpp by editing NUM_ANALOG_INPUTS if desired.
@note In some cases this method can cause glitches which may have to do with the ADC interrupt
interfering with the audio or control interrupts.  
*/
int getSensor(unsigned char channel_num){
	return sensors[channel_num];
}

///method 1: read all channels in background////////////////////////////////////////////////////////////////////////////////////////////






/**  @ingroup analog
Prepare an analog input channel by turning off its digital input buffer.
This helps to reduce noise, increase analog reading speed, and save power.

Here's more detail from http://www.openmusiclabs.com/learning/digital/atmega-adc/:

The DIDR (Data Input Disable Register) disconnects the digital inputs from whichever ADC channels you are using.  This is important for 2 reasons. First off, an analog input will be floating all over the place, and causing the digital input to constantly toggle high and low. This creates excessive noise near the ADC, and burns extra power. Secondly, the digital input and associated DIDR switch have a capacitance associated with them which will slow down your input signal if you are sampling a highly resistive load.

And from the ATmega328p datasheet, p266:

When an analog signal is applied to the ADC5..0 pin and the digital input from
this pin is not needed, this bit should be written logic one to reduce power
consumption in the digital input buffer. Note that ADC named_pins ADC7 and ADC6 do not have digital input buffers, and therefore do not require Digital Input Disable bits.
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

///approach 3: startAnalogRead(), receiveAnalogRead() //////////////////////////////////////////////////////////////////////////////////

static unsigned char analog_reference = DEFAULT;

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
but can be messier in your program than the the initADC(), startRead(), getSensor() way.
@note Timing: about 1us when used in updateControl() with CONTROL_RATE 64.
*/

// basically analogRead() chopped in half so the ADC conversion
// can be started in one function and received in another.
void startAnalogRead(unsigned char pin)
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
Waits for the result of the most recent startAnalogRead().  If used as the first function
of updateControl(), to receive the result of startAnalogRead() from the end of the last
updateControl(), there will probably be no waiting time, as the ADC conversion will
have happened in between interrupts.  This is a big time-saver, since you don't have to
waste time waiting for analogRead() to return (1us here vs 105 us for standard Arduino).
@return The resut of the most recent startAnalogRead().
@note This is the most audio-friendly way to read analog inputs,
but can be messier in your program than the the initADC(), startRead(), getSensor() way.
@note Timing: about 1us when used in updateControl() with CONTROL_RATE 64.
*/
int receiveAnalogRead()
{
	unsigned char low, high;
#if defined(ADCSRA) && defined(ADCL)
	// ADSC is cleared when the conversion finishes
	while (bit_is_set(ADCSRA, ADSC))
		;

	// we have to read ADCL first; doing so locks both ADCL
	// and ADCH until ADCH is read.  reading ADCL second would
	// cause the results of each conversion to be discarded,
	// as ADCL and ADCH would be locked when it completed.
	low  = ADCL;
	high = ADCH;
#else
	// we dont have an ADC, return 0
	low  = 0;
	high = 0;
#endif

	// combine the two bytes
	return (high << 8) | low;
}


