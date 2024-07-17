/*
 * config_checks_avr.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2023-2024 Thomas Friedrichsmeier and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

/* For Mozzi-internal use: Apply hardware specific config defaults and config checks: AVR */

/**
 * @page hardware_avr Mozzi on classic Arduino, Teensy 2.x, Arduino Mega, and other 8 bit "AVR"/ATMEGA architecture boards
 *
 * @section avr_status Port status and notes
 * This is the original "port" of Mozzi, and thus very elaborate. The main challenges on this platform, compared to other MCUs, are limitations in
 * flash, RAM, and CPU power.
 *
 * @section avr_output Output modes
 * The following audio modes (see @ref MOZZI_AUDIO_MODE) are currently supported on this hardware:
 *   - MOZZI_OUTPUT_PWM
 *   - MOZZI_OUTPUT_2PIN_PWM
 *   - MOZZI_OUTPUT_EXTERNAL_TIMED
 *   - MOZZI_OUTPUT_EXTERNAL_CUSTOM
 *
 * In all modes, except MOZZI_OUTPUT_EXTERNAL_CUSTOM, Timer 1 is claimed, and is not available for any other purpose.
 * This means, among other things, that pins 9 and 10 cannot be used for analogWrite(), while pins 3 and 11 should still be available
 * @em except in MOZZI_OUTPUT_2PIN_PWM mode (as Timer2 is also claimed, then; for definite info on which pin is connected to which timer,
 * check a suitable reference, as your board may differ). See *Mozzi>examples>11.Communication>Sinewave_PWM_pins_HIFI*. for an example
 * of emulating analogWrite() on any digital pin, without the need for a hardware timer.
 *
 * @section avr_pwm MOZZI_OUTPUT_PWM
 * For MOZZI_OUTPUT_PWM, output is restricted to pins that are hardware-attached to Timer 1, but can be configured
 * within this tight limit. In particular, the default setup on the
 * Arduino UNO is:
 *    - Mono: Pin 9 -> configurable using MOZZI_AUDIO_PIN_1
 *    - Stereo: Pin 9 (left) and Pin 10 (right)  -> configurable using MOZZI_AUDIO_PIN_1 and MOZZI_AUDIO_PIN_2
 * For pinouts on other boards, refer to config/known_16bit_timers.
 *
 * Rate of the PWM output can be controlled separately from MOZZI_AUDIO_RATE: MOZZI_PWM_RATE.
 *
 * The available sample resolution is 488, i.e. almost 9 bits, providing some headroom above the 8 bit table resolution
 * currently used by the oscillators. You can look at the TimerOne library for more info about how interrupt rate and pwm resolution relate.
 *
 * @section avr_2pin_pwm  MOZZI_OUTPUT_2PIN_PWM
 * In this mode, output is split across two pins (again, both connected to Timer 1), with each outputting 7 bits for a total of 14. This allows for
 * much better dynamic range, providing much more definition than can be achieved using @ref avr_pwm , while using
 * only modestly more processing power. Further, it allows for a much higher PWM carrier rate can be much higher (125 kHz by default; see @ref
 * MOZZI_PWM_RATE), which is well beyond the audible range.
 *
 * On the downside, this mode requires an extra hardware timer (Timer2 in addition to Timer1), which may increase compatibility
 * problems with other Arduino libraries that also require a timer. Further, a more elaborate hardware setup is needed, making it less convenient
 * for rapid prototyping. For circuit details, see https://sensorium.github.io/Mozzi/learn/output/ .
 *
 * On the classic Arduino Uno, the default pinout in this mode is:
 *    - Pin 9 (high bits) and Pin 10 (low bits) -> configurable using MOZZI_AUDIO_PIN_1 and MOZZI_AUDIO_PIN_1_LOW
 *
 * Here is table of the default pins on some other boards. Rows with an x on it have actually been tested (and importantly, the outcome recoreded;
 * input welcome on further boards.)
 *
 * resistor.....3.9k......499k \n
 * x................9..........10...............Arduino Uno \n
 * x................9..........10...............Arduino Duemilanove \n
 * x................9..........10...............Arduino Nano  \n
 * x................9..........10...............Arduino Leonardo  \n
 * x................9..........10...............Ardweeny  \n
 * x................9..........10...............Boarduino  \n
 * x...............11.........12...............Freetronics EtherMega  \n
 * .................11.........12...............Arduino Mega  \n
 * .................14.........15...............Teensy  \n
 * .............B5(14)...B6(15)...........Teensy2  \n
 * x...........B5(25)...B6(26)...........Teensy2++  \n
 * .................13.........12...............Sanguino  \n
 *
 * For pinouts on other AVR boards, config/known_16bit_timers might contain some hints.
 *
 * Rate of the PWM output can be controlled separately from @ref MOZZI_AUDIO_RATE, and is much higher (125kHz), by default: @ref MOZZI_PWM_RATE.
 *
 * The default sample resolution is 7 bits per pin, for a total of 14 bits. This can be configured within hardware limits (@ref MOZZI_PWM_RATE) using
 * @ref MOZZI_AUDIO_BITS_PER_CHANNEL, but beware that increasing this will require even more accuracy in our output resistors (see the linked documentation, above).
 *
 * @section avr_external MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM
 * See @ref external_audio
*/


#if not defined(MOZZI_AUDIO_MODE)
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_PWM
#endif

#if not defined(MOZZI_AUDIO_RATE)
#define MOZZI_AUDIO_RATE 16384
#endif

#if not defined(MOZZI_ANALOG_READ)
#define MOZZI_ANALOG_READ MOZZI_ANALOG_READ_STANDARD
#endif

// Pins for regular PWM output
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM)
#  if !defined(MOZZI_AUDIO_PIN_1)
#define MOZZI_AUDIO_PIN_1 TIMER1_A_PIN
#define MOZZI_AUDIO_PIN_1_REGISTER OCR1A
#  endif
#  if (MOZZI_AUDIO_CHANNELS > 1) && !defined(MOZZI_AUDIO_PIN_2)
#define MOZZI_AUDIO_PIN_2 TIMER1_B_PIN
#define MOZZI_AUDIO_PIN_2_REGISTER OCR1B
#  endif

#  if !defined(MOZZI_PWM_RATE)
#define MOZZI_PWM_RATE 32768
#  endif

#define MOZZI_AUDIO_BITS 8
#define MOZZI_AUDIO_BITS_OPTIMISTIC 9
#define MOZZI_AUDIO_BIAS ((uint8_t) 244)
#endif

// Pins for 2 pin HIFI PWM output
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_2PIN_PWM)
#  if !defined(MOZZI_AUDIO_PIN_1)
#define MOZZI_AUDIO_PIN_1 TIMER1_A_PIN
#define MOZZI_AUDIO_PIN_1_REGISTER OCR1A
#  endif
#  if !defined(MOZZI_AUDIO_PIN_1_LOW)
#define MOZZI_AUDIO_PIN_1_LOW TIMER1_B_PIN
#define MOZZI_AUDIO_PIN_1_LOW_REGISTER OCR1B
#  endif

#  if !defined(MOZZI_PWM_RATE)
#define MOZZI_PWM_RATE 125000
#  endif

#  if !defined(MOZZI_AUDIO_BITS_PER_CHANNEL)
#define MOZZI_AUDIO_BITS_PER_CHANNEL 7
#  endif

#define MOZZI_AUDIO_BITS (2*MOZZI_AUDIO_BITS_PER_CHANNEL)
#endif

#define MOZZI__INTERNAL_ANALOG_READ_RESOLUTION 10

// Step 2: Check
// NOTE: This step is not technically required, but a good idea in any port

MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_2PIN_PWM, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_EXTERNAL_CUSTOM)

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM)
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_CHANNELS, MOZZI_MONO, MOZZI_STEREO)
#endif
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_2PIN_PWM)
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_CHANNELS, MOZZI_MONO)
#endif

/** should we enforce the following?
#if (MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM) || MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_2PIN_PWM))
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_RATE, 16384, 32768)
#endif */

MOZZI_CHECK_SUPPORTED(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_NONE, MOZZI_ANALOG_READ_STANDARD)
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_INPUT, MOZZI_AUDIO_INPUT_NONE, MOZZI_AUDIO_INPUT_STANDARD)
#include "../config/known_16bit_timers.h"

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_2PIN_PWM)
#  if defined(TIMER1_C_PIN)
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_PIN_1, TIMER1_A_PIN, TIMER1_B_PIN, TIMER1_C_PIN);
#  else
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_PIN_1, TIMER1_A_PIN, TIMER1_B_PIN);
#  endif
#endif
