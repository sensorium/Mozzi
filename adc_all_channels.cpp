#include "mozzi_analog.h"

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


/** @ingroup analog
Call adcReadAllChannels() in updateControl() and the results for each channel
will be available by calling adcGetResult(channel_num) next time updateControl()
runs. Reading will happen in the background, using a minimum of processor time
and without blocking other code.

More detail: adcReadAllChannels() starts an initial conversion which triggers an
interrupt when it's complete. The interrupt code stores the result in an array,
changes to the next channel and starts another conversion. When all the channels
have been sampled, the ISR doesn't start a new conversion, so it doesn't
re-trigger itself or use processor time until adcReadAllChannels() is called
again. At any time the latest conversion result for each channel is available by
calling adcGetResult(channel_num).
@note In some cases this method can cause glitches which may have to do with the ADC interrupt
interfering with the audio or control interrupts.  
*/
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

/** @ingroup analog
This returns the most recent analog reading for the specified channel.
@param channel_num The channels are plain numbers 0 to whatever your board goes up to, not the pin
labels A0 to A... which Arduino maps to different numbers depending on the board
being used.
@note The adcEnableInterrupt(), adcReadAllChannels(), adcGetResult() approach is currently set to work with
all channels on each kind of board. You can change the number of channels to use in
mozzi_analog.cpp by editing NUM_ANALOG_INPUTS if desired.
@note In some cases this method can cause glitches which may have to do with the ADC interrupt
interfering with the audio or control interrupts.  
*/
int adcGetResult(unsigned char channel_num){
	return sensors[channel_num];
}

#if ADC_ALL_CHANNELS
#include "Arduino.h"
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

