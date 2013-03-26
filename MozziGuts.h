/*
 * MozziGuts.h
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

#ifndef MOZZIGUTS_H_
#define MOZZIGUTS_H_

#if F_CPU != 16000000
#error Mozzi expects a cpu clock speed of 16MHz!
#endif

#include "Arduino.h"
#include "TimerOne.h"
#include "TimerZero.h"
#include "FrequencyTimer2.h"
#include "mozzi_config.h" // User can change the config file to set audio mode


/** @mainpage Welcome

The latest version of Mozzi and this documentation are at the <a href="http://sensorium.github.com/Mozzi/">Mozzi home page.</a>

Here's a brief description of each section of this documentation:

@section Modules
Fixed point number types, macros and functions for fast fractional maths.

Mozzi core definitions and functions, which are used in every Mozzi sketch.

Utility functions which are generally useful, including midi note to frequency conversion, efficient asynchronous analog input and random functions.

@section Classes
The Classes used to generate control and audio signals.

@section  Files
Files lists ready-to-use wave tables which are in the Mozzi/tables folder.

You can also convert your own sounds from a program like
Audacity to tables for Mozzi with a script called char2mozzi.py, in Mozzi/python.

Read the char2mozzi.py file for instructions.
*/

/** @defgroup core Mozzi core definitions and functions
	@defgroup tables Mozzi look-up-tables for audio waveforms, waveshaping, and control functions.
*/

/** @ingroup core
The STANDARD word is used in Mozzi/config.h to select Mozzi's original audio
output configuration, which is nearly 9 bit sound (-244 to 243) at 16384 Hz and
16384 Hz pwm rate. It uses Timer 1 to output samples at AUDIO_RATE 16384 Hz,
with an interrupt being called once every PWM cycle to set the timer's own pwm
level.

Advantages: Only uses one timer for audio, and one output pin
Disadvantages: low dynamic range, some people can hear pwm carrier frequency, may need simple hardware filter.

Below is a list of the Digital Pins used by Mozzi for STANDARD mode PWM audio out on different boards.
Those which have been tested and reported to work have an x.
Feedback about others is welcome.

x....9........Arduino Uno \n
x....9........Arduino Duemilanove \n 
x....9........Arduino Nano  \n
x....9........Arduino Leonardo  \n
x....9........Ardweeny  \n
x....9........Boarduino  \n 
x...11.......Freetronics EtherMega  \n
x...11.......Arduino Mega  \n
....14........Teensy  \n
x..B5........Teensy2  \n
x..B5(25)..Teensy2++  \n
....13	.......Sanguino  \n
*/
#define STANDARD 0



/** @ingroup core
HIFI can be used in Mozzi/config.h to set the audio mode.  
HIFI enables Mozzi to output 14 bit sound at 16384 Hz sample rate and 125kHz PWM rate.
The high PWM rate of HIFI mode places the carrier frequency beyond audible range, 
overcoming one of the disadvantages of STANDARD mode.

Also, 14 bits of dynamic range in HIFI mode provides more definition than the nearly 9 bits in STANDARD mode.
HIFI mode takes about the same amount of processing time as STANDARD mode, and sounds clearer and brighter.
However, it requires an extra timer to be used on the Arduino, which could increase the chances of 
conflicts with other libraries or processes if they rely on Timer 2.

Timer 1 is used to provide the PWM output at 125kHz.
Timer 2 generates an interrupt at AUDIO_RATE 16384 Hz, which sets the Timer1 PWM levels.
HIFI mode uses 2 output pins, and sums their outputs with resistors, so is slightly less convenient for 
rapid prototyping where you could listen to STANDARD mode by connecting the single output pin 
directly to a speaker or audio input (though a resistor of about 100 ohms is recommended).

The resistors needed for HIFI output are 3.9k and 1M, with 1% or better tolerance.
Use a multimeter to test a group of 1% resistors to find the most accurate.

On 328 based Arduino boards, output is on Timer1, with the low byte on Pin 10 and high byte on Pin 9.
Add the signals through a 3.9k resistor on high byte pin (9) and 1M resistor on low byte pin (10).
Also, a 4.7nF capacitor is recommended between the summing junction of the resistors and ground.

This dual PWM technique is discussed on http://www.openmusiclabs.com/learning/digital/pwm-dac/dual-pwm-circuits/
Also, there are higher quality output circuits are on the site.

Advantages: higher qulaity sound than STANDARD mode.  Doesn't need a notch filter on 
the audio signal because the carrier frequency is out of hearing range.

Disadvantages: requires 2 pins, 2 resistors and a capacitor, so it's not so quick to set up compared 
to a rough, direct single-pin output in STANDARD mode.
 
Pins and where to put the resistors on various boards for HIFI mode.  
Boards tested in HIFI mode have an x, though most of these have been tested in STANDARD mode
and there's no reason for them not to work in HIFI (unless the pin number is wrong or something).
Any reports are welcome. \n

resistor.....3.9k......1M \n
x................9..........10...............Arduino Uno \n
x................9..........10...............Arduino Duemilanove \n 
x................9..........10...............Arduino Nano  \n
..................9..........10...............Arduino Leonardo  \n
x................9..........10...............Ardweeny  \n
x................9..........10...............Boarduino  \n 
.................11.........12...............Freetronics EtherMega  \n
.................11.........12...............Arduino Mega  \n
.................14.........15...............Teensy  \n
...............B5(14)...B6(15)...........Teensy2  \n
...............B5(25)...B6(26)...........Teensy2++  \n
.................13.........12...............Sanguino  \n

*/
#define HIFI 1


/** @ingroup core
AUDIO_RATE is fixed at 16384 Hz for now. For Mozzi's original audio mode, now
called STANDARD, this was a compromise between the sample rate (interrupt rate)
and sample bitdepth (pwm width), which are interdependent due to the way pulse
wave modulation is used to generate the sound output.

Another factor which is important for Mozzi's operation is that with AUDIO_RATE
being a power of two, some internal calculations can be highly optimised for
speed.

In STANDARD mode and with AUDIO_RATE at 16384, the sample resolution is 488,
which provides some headroom above the 8 bit table resolution currently used by
the oscillators. You can look at the TimerOne library for more info about how
interrupt rate and pwm resolution relate.

For much higher quality output which combines signals from pins 9 and 10, 
edit Mozzi/config.h to contain #define AUDIO_MODE HIFI.

@todo Possible option for output to R/2R DAC circuit, like
http://blog.makezine.com/2008/05/29/makeit-protodac-shield-fo/ This would limit
dynamic range to 8 bit, but would remove the 16384Hz pwm carrier frequency noise
which can be a problem in some applications, requiring filtering to remove (see
the Mozzi wiki for filter schematics).
@todo Test 32768Hz audio rate option properly
*/
#define AUDIO_RATE 16384
//#define AUDIO_RATE 32768

/* Used internally for audio-rate optimisation.*/
#define AUDIO_RATE_AS_LSHIFT 14
//#define AUDIO_RATE_AS_LSHIFT 15

#if AUDIO_MODE == STANDARD
#include "AudioConfigStandard9bitPwm.h"
#elif AUDIO_MODE == HIFI
#include "AudioConfigHiSpeed14bitPwm.h"
#endif

// common abbreviations
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

/* //See toneAC http://code.google.com/p/arduino-tone-ac/
// #define MOZZI_AC_OUTPUT 1
// see notes in MozziGuts.cpp
#ifdef MOZZI_AC_OUTPUT
#define AUDIO_CHANNEL_1_AC_PIN TIMER1_B_PIN // defined in TimerOne/config/known_16bit_timers.h
#endif

// see notes in MozziGuts.cpp
#ifdef MOZZI_AC_OUTPUT
#define AUDIO_CHANNEL_1_AC_OUTPUT_REGISTER OCR1B
#endif */


void startMozzi(unsigned int control_rate_hz);

/** @ingroup core
This is where you put your audio code. updateAudio() has to keep up with the
AUDIO_RATE of 16384 Hz, so to keep things running smoothly, avoid doing any
calculations here which could be done in setup() or updateControl().
@return an audio sample.  In STANDARD mode this is between -244 and 243 inclusive.
In HIFI mode, it's a 14 bit number between -16384 and 16383 inclusive.
*/
int updateAudio();


/** @ingroup core
This is where you put your control code. You need updateControl() somewhere in
your sketch, even if it's empty. updateControl() is called at the control rate
you set in startMozzi(). To save processor load, avoid any calculations here
which could be done in setup().
*/
void updateControl();


// see notes in MozziGuts.cpp
void audioHook();

#endif /* MOZZIGUTS_H_ */
