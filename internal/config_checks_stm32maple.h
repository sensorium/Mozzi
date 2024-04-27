/*
 * config_checks_stm32maple.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2023-2024 Thomas Friedrichsmeier and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

#ifndef CONFIG_CHECKS_STM32MAPLE_H
#define CONFIG_CHECKS_STM32MAPLE_H

/**
 * @page hardware_stm32_maple Mozzi on STM32-boards with libmaple based core.
 * port by Thomas Friedrichsmeier
 *
 * @note
 * Be sure to understand the info given at @ref hardware_stm32_disambiguation . This page is about using Mozzi with the STM32 "libmaple based" core.
 *
 * @note
 * This port may look similar to, but uses a different default GPIO pinout than @ref @hardware_stm32duino !
 *
 * @section stm32_maple_status Status and peculiarities of this port
 * Compiles for and runs on a STM32F103C8T6 blue pill board, with a bunch of caveats (see below), i.e. on a board _without_ a
 * real DAC. Should probably run on any other board supported by [Roger Clark's libmaple-based core](https://github.com/rogerclarkmelbourne/Ardu0ino_STM32) (although this theory is untested).
 *
 * @note that at the time of this writing, [Stev Strong's slightliy more recent fork of this core](https://github.com/stevstrong/Arduino_STM32/) does *not* work with
 * Mozzi, apparently due to a bug in pwmWrite().
 *
 * - If you want to use MIDI, be sure to replace "MIDI_CREATE_DEFAULT_INSTANCE()" with "MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI)" (or Serial2)
 * - @ref MOZZI_AUDIO_INPUT_STANDARD is implemented in theory, but untested (feedback welcome)
 * - getAudioInput() and mozziAnalogRead() return values in the STM32's full ADC resolution of 0-4095 rather than AVR's 0-1023.
 *- twi_nonblock is not ported
 *
 * @section stm32_output_modes Output modes
 *
 * The following audio modes (see @ref MOZZI_AUDIO_MODE) are currently supported on this hardware:
 *   - MOZZI_OUTPUT_EXTERNAL_TIMED
 *   - MOZZI_OUTPUT_EXTERNAL_CUSTOM
 *   - MOZZI_OUTPUT_PWM
 *   - MOZZI_OUTPUT_PWM_2PIN
 *
 * The default mode is @ref stm32_maple_pwm .
 *
 * @section stm32_maple_pwm MOZZI_OUTPUT_PWM
 * Standard pulse width modulated output to one (mono) or two (stereo, see @ref MOZZI_AUDIO_CHANNELS) GPIO pins. Default pinout: PB8 (mono/left), PB9 (right channel in stereo).
 * This mode uses two hardware timers: One for the PWM (Timer 4 when using the default pin configuration), and a second for updating the output at audio rate.
 * The default audio resolution is 10 bits, which results in a carrier frequency of ~70kHz on a 72MHz CPU. On slower boards you will have to descrease this.
 * The following settings may be costumized, if desired:
 *
 * @code
 * #define MOZZI_AUDIO_PIN_1       ...  // Left / mono output pin. Default: PB8
 * #define MOZZI_AUDIO_PWM_TIMER   ...  // Must be set ot the hardware timer connected to the above pin. Default: 4
 * #define MOZZI_AUDIO_UPDATE_TIMER ... // Second hardware timer to claim. Default 2
 * #define MOZZI_AUDIO_BITS        ...  // Output resolution in bits. Default is 10
 * // For stereo, only:
 * #define MOZZI_AUDIO_PIN_2       ...  // Right channel output pin. This *must* be connected to the same hardware timer as MOZZI_AUDIO_PIN_1 ! Default: PB9
 * @endcode
 *
 * @section stm32_maple_2pin_pwm MOZZI_OUTPUT_2PIN_PWM
 * This mode is very similar to @ref stm32_maple_pwm, but splitting output for a single channel across two GPIO pins for better resolution. For details on the required
 * hardware setup, and configuration tradeoffs, see @ref avr_2pin_pwm . Stereo output is not available in this mode.
 * Output is at 2*7 bits at up to 560kHz carrier frequency (but limited to 5 times audio rate).
 *
 * Customizable configuration options:
 * @code
 * #define MOZZI_AUDIO_PIN_1       ...  // High byte of the output. Default: PB8
 * #define MOZZI_AUDIO_PIN_2       ...  // Low byte of the output. Default: PB9
 * #define MOZZI_AUDIO_PWM_TIMER   ...  // Must be set to the number of the hardware timer connect to the above pins. Default: 4
 * #define MOZZI_AUDIO_UPDATE_TIMER ... // Second hardware timer to claim. Default TIM2
 * #define MOZZI_AUDIO_BITS_PER_CHANNEL  ...  // Bits per pin. Default is 7
 * @endcode
 *
 * @section stm32_maple_external MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM
 * See @ref external_audio
 * The (single) hardware timer claimed for MOZZI_OUTPUT_EXTERNAL_TIMED may be configured using "MOZZI_AUDIO_UPDATE_TIMER" (default: TIM2).
*/


#if not IS_STM32MAPLE()
#error This header should be included for STM32 (libmaple based core), only
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
#    define MOZZI_AUDIO_UPDATE_TIMER 2
#  endif
#  if !defined(MOZZI_AUDIO_PWM_TIMER)
#    define MOZZI_AUDIO_PWM_TIMER 4
#  endif
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM)
#  if !defined(MOZZI_AUDIO_PIN_1)
#    define MOZZI_AUDIO_PIN_1 PB8
#  endif
#  if (MOZZI_AUDIO_CHANNELS > 1) && !defined(MOZZI_AUDIO_PIN_1)
#    define MOZZI_AUDIO_PIN_2 PB9
#  endif
#  if !defined(MOZZI_AUDIO_BITS)
#    define MOZZI_AUDIO_BITS 10
#  endif
#  define MOZZI_AUDIO_BITS_PER_CHANNEL MOZZI_AUDIO_BITS
#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_2PIN_PWM)
#  if !defined(MOZZI_AUDIO_PIN_1)
#    define MOZZI_AUDIO_PIN_1 PB8
#  endif
#  if !defined(MOZZI_AUDIO_PIN_1_LOW)
#    define MOZZI_AUDIO_PIN_1_LOW PB9
#  endif
#  include "disable_stereo_on_github_workflow.h"
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_CHANNELS, 1)
#  if !defined(MOZZI_AUDIO_PER_CHANNEL)
#    define MOZZI_AUDIO_PER_CHANNEL 7
#  endif
#  define MOZZI_AUDIO_BITS MOZZI_AUDIO_BITS_PER_CHANNEL * 2
#endif

#if !defined(MOZZI_ANALOG_READ)
#define MOZZI_ANALOG_READ MOZZI_ANALOG_READ_STANDARD
#endif

// TODO: This probably isn't correct for all boards!
#define MOZZI__INTERNAL_ANALOG_READ_RESOLUTION 12

#endif        //  #ifndef CONFIG_CHECKS_STM32MAPLE_H
