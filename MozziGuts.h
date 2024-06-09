/*
 * MozziGuts.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2012-2024 Tim Barrass and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
 */

#ifndef MOZZIGUTS_H_
#define MOZZIGUTS_H_

#include "Arduino.h"

#include "MozziConfigValues.h"

#if !(defined(MOZZI_H_) || defined(MOZZI_HEADERS_ONLY_H_))
#warning Direct inclusion of MozziGuts.h is deprecated. Use Mozzi.h, instead, and read about porting to Mozzi 2.0
#define MOZZI_COMPATIBILITY_LEVEL MOZZI_COMPATIBILITY_1_1
#endif

#include "hardware_defines.h"

#if IS_TEENSY3() || IS_TEENSY4()
// required from http://github.com/pedvide/ADC for Teensy 3.*
#include <ADC.h>
#endif

#include "internal/config_checks_generic.h"

#include "mozzi_analog.h"
#include "AudioOutput.h"

// TODO Mozzi 2.0: These typedef probably obsolete?
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

/*! @defgroup core Mozzi Core Functions

The bones of every Mozzi sketch. 

@ingroup core
Sets up the timers for audio and control rate processes, storing the timer
registers so they can be restored when Mozzi stops. startMozzi() goes in your sketch's
setup() routine.

This function intializes the timer(s) needed to move audio samples to the output according to the
configured @ref MOZZI_AUDIO_MODE .

@param control_rate_hz Sets how often updateControl() is called.  It must be a power of 2.
If no parameter is provided, control_rate_hz is set to MOZZI_CONTROL_RATE,
which has a default value of 64 (you can re-\#define it in your sketch).
The practical upper limit for control rate depends on how busy the processor is,
and you might need to do some tests to find the best setting.

@note startMozzi calls setupMozziADC(), which calls setupFastAnalogRead() and adcDisconnectAllDigitalIns(),
which disables digital inputs on all analog input pins.  All in mozzi_analog.h and easy to change if you need to (hack).
They are all called automatically and hidden away because it keeps things simple for a STANDARD_PLUS set up,
but if it turns out to be confusing, they might need to become visible again.
*/
void startMozzi(int control_rate_hz = MOZZI_CONTROL_RATE);



/** @ingroup core
Stops audio and control interrupts and restores the timers to the values they
had before Mozzi was started. This could be useful when using sensor libraries
which depend on the same timers as Mozzi.

A potentially better option for resolving timer conflicts involves using
non-blocking methods, such as demonstrated by the twowire_nonblock code in the
forked version of Mozzi on github, so sound production can continue while
reading sensors.

As it is, stopMozzi restores all the Timers used by Mozzi to their previous
settings. Another scenario which could be easily hacked in MozziGuts.hpp could
involve individually saving and restoring particular Timer registers depending
on which one(s) are required for other tasks.

@note This function is not actually implemented on all platforms.
*/
void stopMozzi();


#if (MOZZI_COMPATIBILITY_LEVEL <= MOZZI_COMPATIBILITY_1_1) && MOZZI_IS(MOZZI_AUDIO_CHANNELS, MOZZI_MONO)
AudioOutput_t updateAudio();
#else
/** @ingroup core
This is where you put your audio code. updateAudio() has to keep up with the
MOZZI_AUDIO_RATE of 16384 or 32768 Hz, so to keep things running smoothly, avoid doing any
calculations here which could be done in setup() or updateControl().
@return an audio sample.

While is possible (in mono sketches) to return a plain unscaled int, it is generally best to return
auto-scaled samples using MonoOutput::from8Bit(), MonoOutput::from16Bit(), MonoOutput::fromNbit(), or
their StereoOutput equivalents.
*/
AudioOutput updateAudio();
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
buffer.  Also, if \@ref MOZZI_AUDIO_INPUT is enabled in the config,
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

See getAudioInput(). The template parameter specifies the desired value range in bits. */
template<byte RES> uint16_t getAudioInput();

/** @ingroup analog

See getAudioInput(). Equivalent to getAudioInput<16>(). */
template<byte RES> inline uint16_t getAudioInput16() { return getAudioInput<16>(); }

/** @ingroup analog
This returns audio input from the input buffer, if
\@ref MOZZI_AUDIO_INPUT is enabled in the config (see also the related option MOZZI_AUDIO_INPUT_PIN).

The audio signal needs to be in the range 0 to VCC volts (i.e. 5 volts on Arduino Uno R3).
Circuits and discussions about biasing a signal
in the middle of this range can be found at
http://electronics.stackexchange.com/questions/14404/dc-biasing-audio-signal
and
http://interface.khm.de/index.php/lab/experiments/arduino-realtime-audio-processing/ .
A circuit and instructions for amplifying and biasing a microphone signal can be found at
http://www.instructables.com/id/Arduino-Audio-Input/?ALLSTEPS

@note The value range returned by this function follows the same rules as detailed in the documentation
      for mozziAnalogRead(): For portable code, define MOZZI_ANALGO_READ_RESOLUTION at the top of your
      sketch, or use the templated version of this function.

@return audio data from the input buffer
*/
#if defined(FOR_DOXYGEN_ONLY) || (!MOZZI_IS(MOZZI_AUDIO_INPUT, MOZZI_AUDIO_INPUT_NONE))
#if defined(FOR_DOXYGEN_ONLY) || defined(MOZZI_ANALOG_READ_RESOLUTION)
inline uint16_t getAudioInput() { return getAudioInput<MOZZI_ANALOG_READ_RESOLUTION>(); };
#else
MOZZI_DEPRECATED("2.0", "This use of getAudioInput() is not portable. Refer to the API documentation for suggested alternatives") inline uint16_t getAudioInput() { return getAudioInput<MOZZI__INTERNAL_ANALOG_READ_RESOLUTION>(); };
#endif
#endif


/** @ingroup core
An alternative for Arduino time functions like micros() and millis(). This is slightly faster than micros(),
and also it is synchronized with the currently processed audio sample (which, due to the audio
output buffer, could diverge up to 256/MOZZI_AUDIO_RATE seconds from the current time).
audioTicks() is updated each time an audio sample
is output, so the resolution is 1/MOZZI_AUDIO_RATE microseconds (61 microseconds when MOZZI_AUDIO_RATE is
16384 Hz).
@return the number of audio ticks since the program began.
*/
unsigned long audioTicks();



/** @ingroup core
An alternative for Arduino time functions like micros() and millis(). This is slightly faster than micros(),
and also it is synchronized with the currently processed audio sample (which, due to the audio
output buffer, could diverge up to 256/MOZZI_AUDIO_RATE seconds from the current time).
audioTicks() is updated each time an audio sample
is output, so the resolution is 1/MOZZI_AUDIO_RATE microseconds (61 microseconds when MOZZI_AUDIO_RATE is
16384 Hz).
@return the approximate number of microseconds since the program began.
@todo  incorporate mozziMicros() in a more accurate EventDelay()?
*/
unsigned long mozziMicros();

#ifndef _MOZZI_HEADER_ONLY
#include "internal/MozziGuts.hpp"
#endif

#endif /* MOZZIGUTS_H_ */
