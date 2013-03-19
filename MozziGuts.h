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
#include <FrequencyTimer2.h>
// audio modes
#define STANDARD 0
#define HIFI 1
#include "config.h" // change config file to set audio mode

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

/**@ingroup core
PWM audio output pin.  For now there is only one channel in the mainstream version.
Below is a list of the Digital Pins used by Mozzi for PWM audio out on different boards.
Those which have been tested and reported to work have an x.
Feedback about others is welcome.

x	 9	Arduino Uno \n
x	 9	Arduino Duemilanove \n 
x	 9	Arduino Nano  \n
x	 9	Arduino Leonardo  \n
x	 9  Ardweeny  \n
x	11  Freetronics EtherMega  \n
..14	Teensy  \n
x	B5  Teensy2  \n
x	B5(25) Teensy2++  \n
x	11	Arduino Mega  \n
..13	Sanguino  \n
x	 9  Boarduino  \n 
*/

/** @ingroup core
AUDIO_RATE is fixed at 16384 Hz for now. For Mozzi's original audio mode, now
called STANDARD, this was a compromise between the sample rate (interrupt rate)
and sample bitdepth (pwm width), which are interdependent due to the way pulse
wave modulation is used to generate the sound output. 
Another factor which is important for Mozzi's operation is that with AUDIO_RATE
being a power of two, some internal calculations can be highly optimised for
speed.
In STANDARD mode and with AUDIO_RATE at 16384, the sample resolution is 488,
which provides some headroom above the 8bit table resolution currently used by
the oscillators. You can look at the TimerOne library for more info about how
interrupt rate and pwm resolution relate.
@todo Possible option for output to R/2R DAC circuit, like
http://blog.makezine.com/2008/05/29/makeit-protodac-shield-fo/ This would limit
dynamic range to 8 bit, but would remove the 16384Hz pwm carrier frequency noise
which can be a problem in some applications, requiring filtering to remove (see
the Mozzi wiki for filter schematics).
(Later) For much higher quality output which combines signals from pins 9 and 10, 
edit Mozzi/config.h to contain #define AUDIO_MODE HIFI.
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


//template <unsigned int CONTROL_RATE, unsigned char AUDIO_MODE = STANDARD>
//class Mozzi
//{
//public:
// these are documented in .cpp file
//void start(unsigned int control_rate_hz = CONTROL_RATE);
void startMozzi(unsigned int control_rate_hz);
//template <unsigned int OUTPUT_OPTION>

void audioHook();


/** @ingroup core
This is where you put your control code. You need updateControl() somewhere in
your sketch, even if it's empty. updateControl() is called at the control rate
you set in startMozzi(). To save processor load, avoid any calculations here
which could be done in setup().
*/
void updateControl();


/** @ingroup core
This is where you put your audio code. updateAudio() has to keep up with the
AUDIO_RATE of 16384 Hz, so to keep things running smoothly, avoid doing any
calculations here which could be done in setup() or updateControl().
@return an audio sample, between -244 and 243 inclusive.
*/
int updateAudio();

//};

//extern Mozzi Mozzi1;




#endif /* MOZZIGUTS_H_ */
