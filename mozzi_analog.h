/*
 * mozzi_analog.h
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

#ifndef MOZZI_ANALOG_H_
#define MOZZI_ANALOG_H_



#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

//#include "mozzi_utils.h"


#if (USE_AUDIO_INPUT==true)
#warning "Using analog pin 0 for audio input."
#endif

//void doControlADC();
//void prepareControlADC();
void startFirstControlADC();
void receiveFirstControlADC();
void startSecondControlADC();
void receiveSecondControlADC();

// hack for Teensy 2 (ATmega32U4), which has "adc_mapping" instead of "analog_pin_to_channel_PGM"
#if defined(__AVR_ATmega32U4__) && defined(CORE_TEENSY) 
//pasted from hardware/arduino/variants/leonardo/pins_arduino.h, doesn't work as of mozzi 0.01.2a
//	__AVR_ATmega32U4__ has an unusual mapping of pins to channels
//extern const uint8_t PROGMEM analog_pin_to_channel_PGM[];
//#define analogPinToChannel(P)  ( pgm_read_byte( analog_pin_to_channel_PGM + (P) ) )

// look at Arduino.app/Contents/Resources/Java/hardware/teensy/cores/teensy/pins_teensy.c - analogRead
// adc_mapping is already declared in pins_teensy.c, but it's static there so we can't access it
static const uint8_t PROGMEM adc_mapping[] = {
// 0, 1, 4, 5, 6, 7, 13, 12, 11, 10, 9, 8
   0, 1, 4, 5, 6, 7, 13, 12, 11, 10, 9, 8, 10, 11, 12, 13, 7, 6, 5, 4, 1, 0, 8 
};
#define analogPinToChannel(P)  ( pgm_read_byte( adc_mapping + (P) ) )
#endif



/** @ingroup analog
Call this in setup() to enable reading analog inputs in the background while audio generating continues.
Then call adcReadAllChannels() at the end of each updateControl() and the results for each analog channel will be
available by calling adcGetResult(channel_num) next time updateControl() runs.
@note This method using adcEnableInterrupt(), adcReadAllChannels() and adcGetResult() is an easy and
efficient way to read analog inputs while generating sound with Mozzi. For many
sketches, however, simply putting setupFastAnalogRead() in setup() and calling
Arduino's usual analogRead() will work fast enough.
@note Don't use setupFastAnalogRead() with adcEnableInterrupt().
It may cause the ADC process to hog the processor, causing audio glitches.
@note In some cases this method can cause glitches which may have to do with the ADC
interrupt interfering with the audio or control interrupts. If this occurs, use
the adcStartConversion(), adcGetResult() methods instead.
*/
void adcEnableInterrupt();


// for setupFastAnalogRead()
enum ANALOG_READ_SPEED {FAST_ADC,FASTER_ADC,FASTEST_ADC};

/** 
@ingroup analog
Make analogRead() faster than the standard Arduino version, changing the
duration from about 105 in unmodified Arduino to about 16 microseconds for a
dependable analogRead() with the default speed parameter FAST_ADC.
To avoid audio glitches, put this in setup() if you use analogRead()
with Mozzi.
See: http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1208715493/11, and
http://www.marulaberry.co.za/index.php/tutorials/code/arduino-adc/
@param speed FAST_ADC, FASTER_ADC or FASTEST_ADC. If no parameter is supplied, the default is FAST_ADC, 
which sets the analog conversion clock predivide rate to 16, giving 1Mhz on a
16MHz board, the fastest rate for which behaviour is defined (~16us per sample).
However, divisors of 8 and 4 also show usable results in the graphs in this
paper:
http://dam.mellis.org/Mellis%20-%20Sensor%20Library%20for%20Arduino%20-%20Paper.pdf,
so you can also try FASTER_ADC or FASTEST_ADC for divide rates of 8 or 4, giving times
of about 8us or 4us per sample.    Beware, reliable results will depend on the sort of input you use.  
Only use FASTER_ADC or FASTEST_ADC if you know what you're doing.
*/
void setupFastAnalogRead(char speed=FAST_ADC);



/**  @ingroup analog
Set up for asynchronous analog input, which enables analog reads to take 
place in the background without blocking the processor.
This goes in setup(), but if  /#define USE_AUDIO_INPUT true is in mozzi_config.h, 
you don't need setupMozziADC() because it's called automatically to enable the audio input.
If you want to use the standard Arduino analogRead(), don't use setupMozziADC(), but do put 
setupFastAnalogRead() in setup() if you want to reduce the chance of audio glitches.
@param speed FAST_ADC, FASTER_ADC or FASTEST_ADC.  See setupFastAnalogRead();
*/
void setupMozziADC(char speed=FAST_ADC);



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
void disconnectDigitalIn(byte channel_num);


/** @ingroup analog
Reconnect the digital input buffer for an analog input channel which has
been set for analog input with disconnectDigitalIn().
@param channel_num the analog input channel you wish to reconnect.
*/
void reconnectDigitalIn(byte channel_num);


/**  @ingroup analog
Prepare all analog input channels by turning off their digital input buffers.
This helps to reduce noise, increase analog reading speed, and save power.
*/
void adcDisconnectAllDigitalIns();


/** @ingroup analog
Reconnect the digital input buffers for analog input channels which have
been set for analog input with disconnectDigitalIn().
*/
void adcReconnectAllDigitalIns();


/** @ingroup analog
Set the channel or pin for the next analog input to be read from.
@param pin channel or pin number.  If pin number is provided, adcSetChannel() will convert it to the channel number.
*/
void adcSetChannel(unsigned char pin);


/** @ingroup analog
Starts an analog to digital conversion of the voltage at a specified pin.  Unlike
Arduino's analogRead() function which waits until a conversion is complete before
returning, adcStartConversion() only sets the conversion to begin, so you can use
the cpu for other things and call for the result later with adcGetResult().
This works well in updateControl(), where you can call adcStartConversion() and
get the result with adcGetResult() the next time the updateControl()
interrupt runs.
@param pin is the analog pin number (A0 to A...) or the channel number (0 to ....) to read.
@note This is the most audio-friendly way to read analog inputs,
but can be messier in your program than the the adcEnableInterrupt(), adcReadAllChannels(), adcGetResult() way.
@note Timing: about 1us when used in updateControl() with CONTROL_RATE 64.
*/
void adcStartConversion(unsigned char pin);



/** @ingroup analog
Starts the analog conversion on the pin or channel most recently set with adcSetChannel();
*/
void adcStartConversion();



/** @ingroup analog
DEPRECIATED.
Waits for the result of the most recent adcStartConversion().  If used as the first function
of updateControl(), to receive the result of adcStartConversion() from the end of the last
updateControl(), there will probably be no waiting time, as the ADC conversion will
have happened in between interrupts.  This is a big time-saver, since you don't have to
waste time waiting for analogRead() to return (1us here vs 105 us for standard Arduino).
@return The resut of the most recent adcStartConversion().
@note This is the most audio-friendly way to read analog inputs,
but can be messier in your program than the the adcEnableInterrupt(), adcReadAllChannels(), adcGetResult(byte) way.
@note Timing: about 1us when used in updateControl() with CONTROL_RATE 64.
*/
int adcGetResult();


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
int adcGetResult(unsigned char channel_num);



/*
Currently, if audio input is enabled it uses analog input channel 0 and doesn't
allow other channels to be used, to avoid interference between the strict audio
rate sampling and any control analog conversions which could be in progress when
the audio needs to be sampled. This might be solved in future, but workable
solutions for control inputs include RC circuits on digital pins, switches, or
serial connections (which also may require extra solutions for asynchronous
operation). There is a fork of Mozzi on github (posted on the mozzi-users list)
which includes a 2-wire async serial adaptation. It would be good eventually to
merge that with the main repo.
*/
// these are in adc_all_channels.cpp with the ISR(ADC_vect) they work with.  The ISR needs to
// hide from the compiler if USE_AUDIO_INPUT is true, which also uses ISR(ADC_vect).
// They could have been conditionally compiled together in the one ISR, but this is slightly 
// less messy and allows for some static variables which would otherwise have had to have been global.


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
void adcReadAllChannels();


#endif /* MOZZI_ANALOG_H_ */
