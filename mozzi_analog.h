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
#warning "Using AUDIO_INPUT_PIN defined in mozzi_config.h for audio input."
#endif


void adcReadSelectedChannels();
//void receiveFirstControlADC();
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



// for setupFastAnalogRead()
enum ANALOG_READ_SPEED {FAST_ADC,FASTER_ADC,FASTEST_ADC};

/** 
@ingroup analog
This is automatically called in startMozzi.
It makes mozziAnalogRead() happen faster than the standard Arduino analogRead(), changing the
duration from about 105 in unmodified Arduino to about 16 microseconds for a
dependable read with the default speed parameter FAST_ADC.
If you want to set on of the faster modes (see params) you can call this after startMozzi().
See: http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1208715493/11, and
http://www.marulaberry.co.za/index.php/tutorials/code/arduino-adc/
@param speed FAST_ADC, FASTER_ADC or FASTEST_ADC. If no parameter is supplied, the
default is FAST_ADC, which sets the analog conversion clock predivide rate to
16, giving 1Mhz on a 16MHz board, the fastest rate for which behaviour is
defined (~16us per sample). However, divisors of 8 and 4 also show usable
results in the graphs in this paper:
http://dam.mellis.org/Mellis%20-%20Sensor%20Library%20for%20Arduino%20-%20Paper.pdf,
so you can also try FASTER_ADC or FASTEST_ADC for divide rates of 8 or 4, giving
times of about 8us or 4us per sample. Beware, reliable results will depend on
the sort of input signal you have. Only use FASTER_ADC or FASTEST_ADC if you
know what you're doing.
*/
void setupFastAnalogRead(char speed=FAST_ADC);



/*  @ingroup analog
Set up for asynchronous analog input, which enables analog reads to take 
place in the background without blocking the processor.
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



/* @ingroup analog
Starts an analog to digital conversion of the voltage on a specified channel.  Unlike
Arduino's analogRead() function which waits until a conversion is complete before
returning, adcStartConversion() only sets the conversion to begin, so you can use
the cpu for other things and call for the result later with adcGetResult().
@param channel is the analog channel number (0 to ....), which is not necessarily the same as the pin number
Use adcPinToChannelNum() to convert the pin number to its channel number.
@note Timing: about 1us when used in updateControl() with CONTROL_RATE 64.
*/
void adcStartConversion(unsigned char channel);



/** @ingroup analog
Reads the analog input of a chosen channel, without blocking other operations from running.
It actually returns the most recent analog reading and puts the chosen pin or channel
on the stack of channels to be read in the background before the next control
interrupt.
@param pin_or_channel the analog pin or channel number.
@return the digitised value of the voltage on the chosen channel, in the range 0-1023.
*/
int mozziAnalogRead(unsigned char pin);


/* Used in MozziGuts.cpp, in updateControlWithAutoADC() to kick off any mozziAnalogReads waiting on the stack
*/
void adcStartReadCycle();


unsigned char adcPinToChannelNum(unsigned char pin);

#endif /* MOZZI_ANALOG_H_ */
