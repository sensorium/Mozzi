/*
 * config_checks_stm32duino.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2023-2024 Thomas Friedrichsmeier and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

#ifndef CONFIG_CHECKS_STM32DUINO_H
#define CONFIG_CHECKS_STM32DUINO_H

/**
* @page hardware_stm32_disambiguation Mozzi on STM32-based boards - disambiguation
*
*  * The situation on STM32-based boards is rather confusing, as there are several competing Arduino cores. Importantly:
*    - Some boards use dedicated cores (e.g. Arduino Giga / Portenta @ref hardware_mbed) etc. For those, see the relevant sections (if we support them).
*    - There is a series of libmaple-based cores, including [Roger Clark's libmaple-based core](https://github.com/rogerclarkmelbourne/Arduino_STM32). These are highly optimized,
*      and provide very complete support, but only for a limited number of boards. Unfortunately, at the time of this writing (2023/04), they are not available for installation
*      via the Arduino Board Manager, and they do not currently seem actively maintained.
*      For using these with Mozzi, see @ref hardware_stm32_maple
*    - A generic Arduino core for STM32 is the [STM32duino core](https://github.com/stm32duino/Arduino_Core_STM32). It supports a huge set of boards, and seems to have offical
*      backing by STM, but some features of the libmaple based cores are still lacking. To complete confusion, this core now uses the label "STM32duino", which used to be what
*      the libmaple cores above were known by (don't blame Mozzi for this mess!).
*      For using this with Mozzi, see @ref hardware_stm32duino
* */

/**
 * @page hardware_stm32duino Mozzi on STM32duino-based boards.
 *
 * port by Thomas Friedrichsmeier
 *
 * @note
 * Be sure to understand the info given at @ref hardware_stm32_disambiguation . This page is about using Mozzi with the STM32duino core.
 *
 * @section stm32duino_status Port status and usage notes
 * Tested on a STM32F103C8T6 blue pill board as well as an STM32F411CE black pill board, i.e. on sboards _without_ a
 * real DAC. Compiles and runs, with a bunch of caveats (see below). Should probably run on any other board supported by the
 * [STM32duino core](https://github.com/stm32duino/Arduino_Core_STM32) (although this theory is untested).
 * When trying any other board, you probably want to check the platform specific settings (see below), carefully, importantly, whether the desired output resolution is
 * achievable, and whether the desired output pins are PWM capable.
 *
 * - @ref MOZZI_ANALOG_READ input implementation is somewhat experimental, and may not be able to service a whole lot of pins (contributions welcome)
 * - @ref MOZZI_AUDIO_INPUT is completely untested (but implemented in theory; feedback welcome!)
 * - getAudioInput() and mozziAnalogRead() return values in the STM32's full ADC resolution (the exact range depending on the board in use) rather than AVR's 0-1023.
 * - twi_nonblock is not ported
 *
 * @section stm32duino_output_modes Output modes
 * The following audio modes (see @ref MOZZI_AUDIO_MODE) are currently supported on this hardware:
 *   - MOZZI_OUTPUT_EXTERNAL_TIMED
 *   - MOZZI_OUTPUT_EXTERNAL_CUSTOM
 *   - MOZZI_OUTPUT_PWM
 *   - MOZZI_OUTPUT_PWM_2PIN
 *
 * The default mode is @ref stm32duino_pwm .
 *
 * @note
 * This port may look similar to, but uses a different default GPIO pinout than @hardware_stm32_maple !
 *
 * @section stm32duino_pwm MOZZI_OUTPUT_PWM
 * Standard pulse width modulated output to one (mono) or two (stereo, see @ref MOZZI_AUDIO_CHANNELS) GPIO pins. Default pinout: PA8 (mono/left), PA9 (right channel in stereo).
 * This mode uses two hardware timers: One for the PWM (Timer 3 when using the default pin configuration), and a second for updating the output at audio rate.
 * The default audio resolution is 10 bits, which results in a carrier frequency of ~70kHz on a 72MHz CPU. On slower boards you will have to descrease this.
 * The following settings may be costumized, if desired:
 *
 * @code
 * #define MOZZI_AUDIO_PIN_1       ...  // Left / mono output pin. Default: PA8
 * #define MOZZI_AUDIO_UPDATE_TIMER ... // Second hardware timer to claim, must not be the same of the timer for the above pin. Default TIM2
 * #define MOZZI_AUDIO_BITS        ...  // Output resolution in bits. Default is 10
 * // For stereo, only:
 * #define MOZZI_AUDIO_PIN_2       ...  // Right channel output pin. This *must* be connected to the same hardware timer as MOZZI_AUDIO_PIN_1 ! Default: PA9
 * @endcode
 *
 * @section stm32duino_pwm MOZZI_OUTPUT_2PIN_PWM
 * This mode is very similar to @ref stm32duino_pwm, but splitting output for a single channel across two GPIO pins for better resolution. For details on the required
 * hardware setup, and configuration tradeoffs, see @ref avr_2pin_pwm . Stereo output is not available in this mode.
 * Output is at 2*7 bits at up to 560kHz carrier frequency (but limited to 5 times audio rate).
 *
 * Customizable configuration options:
 * @code
 * #define MOZZI_AUDIO_PIN_1       ...  // High byte of the output. Default: PA8
 * #define MOZZI_AUDIO_PIN_1_LOW   ...  // Low byte of the output. Default: PA9
 * #define MOZZI_AUDIO_UPDATE_TIMER ... // Second hardware timer to claim. Default TIM2
 * #define MOZZI_AUDIO_BITS_PER_CHANNEL  ...  // Bits per pin. Default is 7
 * @endcode
 *
 * @section stm32duino_external MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM
 * See @ref external_audio .
 * The (single) hardware timer claimed for MOZZI_OUTPUT_EXTERNAL_TIMED may be configured using "MOZZI_AUDIO_UPDATE_TIMER" (default: TIM2).
*/

#if not IS_STM32DUINO()
#error This header should be included for STM32 (stm32duino.com core), only
#endif

#if !defined(MOZZI_AUDIO_MODE)
#  define MOZZI_AUDIO_MODE MOZZI_OUTPUT_PWM
#endif
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_EXTERNAL_CUSTOM, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_2PIN_PWM)

#if !defined(MOZZI_AUDIO_RATE)
#  define MOZZI_AUDIO_RATE 32768
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_2PIN_PWM, MOZZI_OUTPUT_EXTERNAL_TIMED)
#  if !defined(MOZZI_AUDIO_UPDATE_TIMER)
#    define MOZZI_AUDIO_UPDATE_TIMER TIM2
#  endif
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM)
#  if !defined(MOZZI_AUDIO_PIN_1)
#    define MOZZI_AUDIO_PIN_1 PA8
#  endif
#  if (MOZZI_AUDIO_CHANNELS > 1) && !defined(MOZZI_AUDIO_PIN_1)
#    define MOZZI_AUDIO_PIN_2 PA9
#  endif
#  if !defined(MOZZI_AUDIO_BITS)
#    define MOZZI_AUDIO_BITS 10
#  endif
#  define MOZZI_AUDIO_BITS_PER_CHANNEL MOZZI_AUDIO_BITS
#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_2PIN_PWM)
#  if !defined(MOZZI_AUDIO_PIN_1)
#    define MOZZI_AUDIO_PIN_1 PA8
#  endif
#  if !defined(MOZZI_AUDIO_PIN_1_LOW)
#    define MOZZI_AUDIO_PIN_1_LOW PA9
#  endif
#  include "disable_stereo_on_github_workflow.h"
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_CHANNELS, 1)
#  if !defined(MOZZI_AUDIO_PER_CHANNEL)
#    define MOZZI_AUDIO_BITS_PER_CHANNEL 7
#  endif
#  define MOZZI_AUDIO_BITS (MOZZI_AUDIO_BITS_PER_CHANNEL * 2)
#endif

#if !defined(MOZZI_ANALOG_READ)
#define MOZZI_ANALOG_READ MOZZI_ANALOG_READ_STANDARD
#endif

#define MOZZI__INTERNAL_ANALOG_READ_RESOLUTION ADC_RESOLUTION

#endif        //  #ifndef CONFIG_CHECKS_STM32DUINO_H
