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

#include "mozzi_config.h" // User can change the config file to set audio mode
#include "TimerOne.h"
#include "TimerZero.h"
#include "FrequencyTimer2.h"

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
*/
//@defgroup tables Mozzi look-up-tables for audio waveforms, waveshaping, and control functions.

/** @ingroup core
Use \#define AUDIO_MODE STANDARD in Mozzi/config.h to select Mozzi's original audio
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
Use \#define AUDIO_MODE HIFI in Mozzi/config.h to set the audio mode to HIFI for output 14 bit sound at 16384 Hz sample rate and 125kHz PWM rate.
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

Advantages: higher quality sound than STANDARD mode.  Doesn't need a notch filter on 
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
x................9..........10...............Arduino Leonardo  \n
x................9..........10...............Ardweeny  \n
x................9..........10...............Boarduino  \n 
x...............11.........12...............Freetronics EtherMega  \n
.................11.........12...............Arduino Mega  \n
.................14.........15...............Teensy  \n
.............B5(14)...B6(15)...........Teensy2  \n
x...........B5(25)...B6(26)...........Teensy2++  \n
.................13.........12...............Sanguino  \n

*/
#define HIFI 1


#if (AUDIO_MODE == STANDARD) && (AUDIO_RATE == 32768)
#error AUDIO_RATE 32768 does not work when AUDIO_MODE is STANDARD, check settings in Mozzi/mozzi_config.h
#endif


#if AUDIO_RATE == 16384
#define AUDIO_RATE_AS_LSHIFT 14
#define MICROS_PER_AUDIO_TICK 61 // 1000000 / 16384 = 61.035, ...* 256 = 15625
#elif AUDIO_RATE == 32768
#define AUDIO_RATE_AS_LSHIFT 15
#define MICROS_PER_AUDIO_TICK 31 // = 1000000 / 32768 = 30.518, ...* 256 = 7812.6
#endif


#if AUDIO_MODE == STANDARD
#include "AudioConfigStandard9bitPwm.h"
#elif AUDIO_MODE == HIFI
#include "AudioConfigHiSpeed14bitPwm.h"
#endif

// common abbreviations
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;




/** @ingroup core
Sets up the timers for audio and control rate processes, storing the timer
registers so they can be restored when Mozzi stops. startMozzi() goes in your sketch's
setup() routine.

In STANDARD and HIFI modes, Mozzi uses Timer 0 for control interrupts 0, disabling Arduino
delay(), millis(), micros() and delayMicroseconds. 
For delaying events, you can use Mozzi's EventDelay() unit instead (not to be confused with AudioDelay()). 

In STANDARD mode, startMozzi() starts Timer 1 for PWM output and audio output interrupts,
and in HIFI mode, Mozzi uses Timer 1 for PWM and Timer2 for audio interrupts. 

The audio rate is currently fixed at 16384 Hz.

@param control_rate_hz Sets how often updateControl() is called. It can be any
power of 2 above and including 64. The practical upper limit for control rate
depends on how busy the processor is, and you might need to do some tests to
find the best setting. 

It's good to define CONTROL_RATE in your
sketches (eg. \#define CONTROL_RATE 128) because the literal numeric value is
necessary for Oscils to work properly, and it also helps to keep the
calculations in your sketch clear.

@todo See if there is any advantage to using 8 bit port, without pwm, with a resistor ladder 
(maybe use readymade resistor networks).
*/
void startMozzi(int control_rate_hz = CONTROL_RATE);



/** @ingroup core
Stops audio and control interrupts and restores the timers to the values they
had before Mozzi was started. This will enable the standard Arduino time
functions millis(), micros(), delay(), and delayMicroseconds(). This could be
useful when using sensor libraries which depend on the same timers as Mozzi. 

A potentially better option for resolving timer conflicts involves using
non-blocking methods, such as demonstrated by the twowire_nonblock code in the
forked version of Mozzi on github, so sound production can continue while
reading sensors.

As it is, pauseMozzi restores all the Timers used by Mozzi to their previous
settings. Another scenario which could be easily hacked in MozziGuts.cpp could
involve individually saving and restoring particular Timer registers depending
on which one(s) are required for other tasks, so for example the control
interrupt (Timer 0) could be suspended while audio continues.*/
void pauseMozzi();



/** @ingroup core
Restores Mozzi audio and control interrupts, if they have been temporarily
disabled with pauseMozzi(). This once more takes over Timer 0, and stops the
Arduino time functions millis(), micros(), delay(), and delayMicroseconds() from
working.
*/
void unPauseMozzi();

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


/** @ingroup core
This is required in Arduino's loop(). If there is room in Mozzi's output buffer,
audioHook() calls updateAudio() once and puts the result into the output
buffer.  Also, if \#define USE_AUDIO_INPUT true is in Mozzi/mozzi_config.h,
audioHook() takes care of moving audio input from the input buffer so it can be
accessed with getAudioInput() in your updateAudio() routine.
If other functions are called in loop() along with audioHook(), see if
they can be called less often by moving them into updateControl(), 
to save processing power. Otherwise it may be most efficient to
calculate a block of samples at a time by putting audioHook() in a loop of its
own, rather than calculating only 1 sample for each time your other functions
are called.
@todo Try pre-decrement positions and swap gap calc around
*/
void audioHook();


/** @ingroup analog
This returns audio input from the input buffer, if 
\#define USE_AUDIO_INPUT true is in the Mozzi/mozzi_config.h file.
Audio input is currently restricted to analog pin 0 (this may change in future).
The audio signal needs to be in the range 0 to 5 volts.  
Circuits and discussions about biasing a signal
in the middle of this range can be found at 
http://electronics.stackexchange.com/questions/14404/dc-biasing-audio-signal 
and
http://interface.khm.de/index.php/lab/experiments/arduino-realtime-audio-processing/ .
A circuit and instructions for amplifying and biasing a microphone signal can be found at
http://www.instructables.com/id/Arduino-Audio-Input/?ALLSTEPS
@return audio data from the input buffer
*/
#if USE_AUDIO_INPUT
int getAudioInput();
#endif

/**
A replacement for Arduino micros() which is disabled by Mozzi which takes over Timer 0 for control interrupts.
May be a useful hack for replacing micros() in time-related sensor libraries.
Will also incorporate for a more accurate EventDelay().
@return the approximate number of microseconds since the program began.
*/
unsigned long mozziMicros();

// internal use
static void setupTimer2();

#endif /* MOZZIGUTS_H_ */
