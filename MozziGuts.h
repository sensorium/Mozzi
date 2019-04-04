/*
 * MozziGuts.h
 *
 * Copyright 2012 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi by Tim Barrass is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */

#ifndef MOZZIGUTS_H_
#define MOZZIGUTS_H_

//#define F_CPU 8000000 // testing

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "hardware_defines.h"

#if IS_TEENSY3()
// required from http://github.com/pedvide/ADC for Teensy 3.*
#include <ADC.h>
#endif

#include "mozzi_analog.h"

#if not defined (CONTROL_RATE)
/** @ingroup core
Control rate setting.
Mozzi's CONTROL_RATE sets how many times per second updateControl() is called.
CONTROL_RATE has a default of 64 Hz, but it can be changed at the top of your sketch,
(before the \#includes), for example: \#define CONTROL_RATE 256.
It is useful to have CONTROL_RATE set at a power of 2 (such as 64,128,256 etc),
to have exact timing of audio and control operations.
Non-power-of-2 CONTROL_RATE can cause glitches due to audio and control
events not lining up precisely.  If this happens a power of two CONTROL_RATE might solve it.
Try to keep CONTROL_RATE low, for efficiency, though higher rates up to about 1000
can sometimes give smoother results, avoiding the need to interpolate
sensitive variables at audio rate in updateAudio().
*/
#define CONTROL_RATE 64
#endif



/** @ingroup core
Used to set AUDIO_MODE to STANDARD, STANDARD_PLUS, or HIFI.

STANDARD / STANDARD_PLUS
---------------
Use \#define AUDIO_MODE STANDARD_PLUS in Mozzi/config.h to select this
output configuration, which is nearly 9 bit sound (-244 to 243) at 16384 Hz sample rate (AUDIO_RATE) and
32768 Hz PWM rate. It uses Timer 1 for PWM and the sample updating routine (as an interrupt).

STANDARD is obsolete now, replaced by STANDARD_PLUS which is the default audio mode.
STANDARD mode uses 16384 Hz PWM rate with an output interrupt at the same frequency.
Some people can hear the PWM carrier frequency as an annoying whine.

STANDARD_PLUS mode uses 32768 Hz PWM rate, so the PWM carrier is out of hearing range.
In this mode every alternate interrupt is used for the sample update (unless you /#define AUDIO_RATE 32768 in mozzi_config.h),
which makes it slightly less efficient than STANDARD, but almost always better.

Advantages: Only uses one timer for audio, and one output pin.
Disadvantages: low dynamic range.

Below is a list of the Digital Pins used by Mozzi for STANDARD and STANDARD_PLUS audio out on different boards.
Those which have been tested and reported to work have an x.
Feedback about others is welcome.

x....9........Arduino Uno \n
x....9........Arduino Duemilanove \n
x....9........Arduino Nano  \n
x....9........Arduino Leonardo  \n
x....9........Ardweeny  \n
x....9........Boarduino  \n
x...11.......Freetronics EtherMega  *broken since Jan 2015 \n
x...11.......Arduino Mega  *broken since Jan 2015 \n
....14........Teensy  \n
x..B5........Teensy2  \n
x..B5(25)..Teensy2++  \n
x..A14.....Teensy 3.0, 3.1 and 3.2  \n
....13	.......Sanguino  \n

On Teensy 3.* STANDARD and STANDARD_PLUS are the same, providing 16384Hz sample rate and 12 bit resolution on pin A14/ADC.
The Teensy 3.* DAC output does not rely on PWM.

On AVR, there is the possibility to use an external DAC in order to have an Audio output which does not rely on PWM. For now, this is only implemented for MCP49XX DAC, you can enable this in mozzi_config.h. Please see details in AudioConfigExternalDAC.h.
*/


/** @ingroup core
Used to set AUDIO_MODE to HIFI.

HIFI for AVR  and STM32 (not for Teensy 3.*)
----
Use \#define AUDIO_MODE HIFI in Mozzi/config.h to set the audio mode to HIFI for output 14 bit sound at 16384 Hz sample rate and 125kHz PWM rate.
The high PWM rate of HIFI mode places the carrier frequency beyond audible range.

Also, 14 bits of dynamic range in HIFI mode provides more definition than the nearly 9 bits in STANDARD_PLUS mode.
HIFI mode takes about the same amount of processing time as STANDARD_PLUS mode, and should sound clearer and brighter.
However, it requires an extra timer to be used on the Arduino, which could increase the chances of
conflicts with other libraries or processes if they rely on Timer 2.

Timer 1 is used to provide the PWM output at 125kHz.
Timer 2 generates an interrupt at AUDIO_RATE 16384 Hz, which sets the Timer1 PWM levels.
HIFI mode uses 2 output pins, and sums their outputs with resistors, so is slightly less convenient for
rapid prototyping where you could listen to STANDARD_PLUS mode by connecting the single output pin
directly to a speaker or audio input (though a resistor of about 100 ohms is recommended).

The resistors needed for HIFI output are 3.9k and 499k, with 0.5% or better tolerance.
If you can only get 1% resistors, use a multimeter to find the most accurate.
Use two 1M resistors in parallel if you can't find 499k.

On 328 based Arduino boards, output is on Timer1, with the high byte on Pin 9 and low byte on Pin 10.
Add the signals through a 3.9k resistor on high byte pin (9) and 499k resistor on low byte pin (10).
Also, a 4.7nF capacitor is recommended between the summing junction of the resistors and ground.

This dual PWM technique is discussed on http://www.openmusiclabs.com/learning/digital/pwm-dac/dual-pwm-circuits/
Also, there are higher quality output circuits are on the site.

Advantages: should be higher quality sound than STANDARD_PLUS mode.  Doesn't need a notch filter on
the audio signal (like STANDARD which is now obsolete) because the carrier frequency is out of hearing range.

Disadvantages: requires 2 pins, 2 resistors and a capacitor, so it's not so quick to set up compared
to a rough, direct single-pin output in STANDARD_PLUS mode.

Pins and where to put the resistors on various boards for HIFI mode.
Boards tested in HIFI mode have an x, though most of these have been tested in STANDARD_PLUS mode
and there's no reason for them not to work in HIFI (unless the pin number is wrong or something).
Any reports are welcome. \n

resistor.....3.9k......499k \n
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

HIFI is not available/not required on Teensy 3.* or ARM.
*/

//enum audio_modes {STANDARD,STANDARD_PLUS,HIFI};
#define STANDARD 0
#define STANDARD_PLUS 1
#define HIFI 2


#include "mozzi_config.h" // User can change the config file to set audio mode

#if (AUDIO_MODE == STANDARD) && (AUDIO_RATE == 32768)
#error AUDIO_RATE 32768 does not work when AUDIO_MODE is STANDARD, try setting the AUDIO_MODE to STANDARD_PLUS in Mozzi/mozzi_config.h
#endif


#define CLOCK_TICKS_PER_AUDIO_TICK (F_CPU / AUDIO_RATE)


#if AUDIO_RATE == 16384
#define AUDIO_RATE_AS_LSHIFT 14
#define MICROS_PER_AUDIO_TICK 61 // 1000000 / 16384 = 61.035, ...* 256 = 15625
#elif AUDIO_RATE == 32768
#define AUDIO_RATE_AS_LSHIFT 15
#define MICROS_PER_AUDIO_TICK 31 // = 1000000 / 32768 = 30.518, ...* 256 = 7812.6
#endif

#ifdef EXTERNAL_DAC
#include"AudioConfigExternalDAC.h"
#endif
#if IS_TEENSY3()
#include "AudioConfigTeensy3_12bit.h"
#elif IS_STM32()
#include "AudioConfigSTM32.h"
#elif IS_ESP8266()
#include "AudioConfigESP.h"
#elif IS_SAMD21()
#include "AudioConfigSAMD21.h"
#elif IS_AVR()
#if (AUDIO_MODE == STANDARD)
#include "AudioConfigStandard9bitPwm.h"
#elif (AUDIO_MODE == STANDARD_PLUS)
#include "AudioConfigStandardPlus.h"
#elif (AUDIO_MODE == HIFI)
#include "AudioConfigHiSpeed14bitPwm.h"
#endif
#endif

// common numeric types
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

#if defined(__AVR__)
typedef unsigned char byte; // for arduino ide
typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned int uint16_t;
typedef signed int int16_t;
typedef unsigned long uint32_t;
typedef signed long int32_t;
#else
// Other supported arches add typedefs, here, unless already defined for that platform needed
#endif


/** @ingroup core
Sets up the timers for audio and control rate processes, storing the timer
registers so they can be restored when Mozzi stops. startMozzi() goes in your sketch's
setup() routine.

Contrary to earlier versions of Mozzi, this version does not take over Timer 0, and thus Arduino
functions delay(), millis(), micros() and delayMicroseconds() remain usable in theory. That said,
you should avoid these functions, as they are slow (or even blocking). For measuring time, refer
to mozziMircos(). For delaying events, you can use Mozzi's EventDelay() unit instead
(not to be confused with AudioDelay()).

In STANDARD mode, startMozzi() starts Timer 1 for PWM output and audio output interrupts,
and in STANDARD_PLUS and HIFI modes, Mozzi uses Timer 1 for PWM and Timer2 for audio interrupts.

The audio rate defaults to 16384 Hz, but you can experiment with 32768 Hz by changing AUDIO_RATE in mozzi_config.h.

@param control_rate_hz Sets how often updateControl() is called.  It must be a power of 2.
If no parameter is provided, control_rate_hz is set to CONTROL_RATE,
which has a default value of 64 (you can re-\#define it in your sketch).
The practical upper limit for control rate depends on how busy the processor is,
and you might need to do some tests to find the best setting.

@note startMozzi calls setupMozziADC(), which calls setupFastAnalogRead() and adcDisconnectAllDigitalIns(),
which disables digital inputs on all analog input pins.  All in mozzi_analog.h and easy to change if you need to (hack).
They are all called automatically and hidden away because it keeps things simple for a STANDARD_PLUS set up,
but if it turns out to be confusing, they might need to become visible again.
*/
void startMozzi(int control_rate_hz = CONTROL_RATE);



/** @ingroup core
Stops audio and control interrupts and restores the timers to the values they
had before Mozzi was started. This could be useful when using sensor libraries
which depend on the same timers as Mozzi.

A potentially better option for resolving timer conflicts involves using
non-blocking methods, such as demonstrated by the twowire_nonblock code in the
forked version of Mozzi on github, so sound production can continue while
reading sensors.

As it is, stopMozzi restores all the Timers used by Mozzi to their previous
settings. Another scenario which could be easily hacked in MozziGuts.cpp could
involve individually saving and restoring particular Timer registers depending
on which one(s) are required for other tasks. */
void stopMozzi();


// TB2017 deleted function, use startMozzi() instead
// /** @ingroup core
// Restores Mozzi audio and control interrupts, if they have been temporarily
// disabled with pauseMozzi().
// */
// void unPauseMozzi();


/** @ingroup core
This is where you put your audio code. updateAudio() has to keep up with the
AUDIO_RATE of 16384 Hz, so to keep things running smoothly, avoid doing any
calculations here which could be done in setup() or updateControl().
@return an audio sample.  In STANDARD modes this is between -244 and 243 inclusive.
In HIFI mode, it's a 14 bit number between -16384 and 16383 inclusive.
*/
#if (STEREO_HACK == true)
extern int audio_out_1, audio_out_2;
void updateAudio();
#else
int updateAudio();
#endif

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
*/
void audioHook();



/** @ingroup analog
This returns audio input from the input buffer, if
\#define USE_AUDIO_INPUT true is in the Mozzi/mozzi_config.h file.
The pin used for audio input is set in Mozzi/mozzi_config.h with
\#define AUDIO_INPUT_PIN 0 (or other analog input pin).
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
#if (USE_AUDIO_INPUT == true)
int getAudioInput();
#endif


/** @ingroup core
An alternative for Arduino time functions like micros() and millis(). This is slightly faster than micros(),
and also it is synchronized with the currently processed audio sample (which, due to the audio
output buffer, could diverge up to 256/AUDIO_RATE seconds from the current time).
audioTicks() is updated each time an audio sample
is output, so the resolution is 1/AUDIO_RATE microseconds (61 microseconds when AUDIO_RATE is
16384 Hz).
@return the number of audio ticks since the program began.
*/
unsigned long audioTicks();



/** @ingroup core
An alternative for Arduino time functions like micros() and millis(). This is slightly faster than micros(),
and also it is synchronized with the currently processed audio sample (which, due to the audio
output buffer, could diverge up to 256/AUDIO_RATE seconds from the current time).
audioTicks() is updated each time an audio sample
is output, so the resolution is 1/AUDIO_RATE microseconds (61 microseconds when AUDIO_RATE is
16384 Hz).
@return the approximate number of microseconds since the program began.
@todo  incorporate mozziMicros() in a more accurate EventDelay()?
*/
unsigned long mozziMicros();




// internal use
#if (AUDIO_MODE == HIFI)
static void setupTimer2();
#endif

#endif /* MOZZIGUTS_H_ */
