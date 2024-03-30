/*
 * config_checks_teensy.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2023-2024 Thomas Friedrichsmeier and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

#ifndef CONFIG_CHECK_TEENSY_H
#define CONFIG_CHECK_TEENSY_H

/** @ingroup hardware
 * @page hardware_teensy3 Mozzi on Teensy 3.0/3.1/3.2/3.4/3.5/LC boards.
 *
 * port by Tim Barrass
 *
 * @note
 * For Teensy 4.x see @ref hardware_teensy4
 *
 * @section teensy3_status Port status and ussage notes
 * This port requires the following two libraries (which should be part of a default installtion, however):
 * - [Timer library](https://github.com/loglow/IntervalTimer) for Teensy 3.* by Daniel Gilbert
 * - [ADC library](http://github.com/pedvide/ADC) by Pedro Villanueva
 *
 * Some of the differences for Teensy 3.* which will affect users include:
 * - twi_nonblock code by Marije Baalman for non-blocking I2C is not compatible with Teensy 3.0/3.1/3.2.
 *
 * @section teensy3_output Output modes
 * The following audio modes (see @ref MOZZI_AUDIO_MODE) are currently supported on this hardware:
 *   - MOZZI_OUTPUT_EXTERNAL_TIMED
 *   - MOZZI_OUTPUT_EXTERNAL_CUSTOM
 *   - MOZZI_OUTPUT_INTERNAL_DAC
 *
 * The default mode is @ref teensy3_internal_dac .
 *
 * @section teensy3_internal_dac MOZZI_OUTPUT_INTERNAL_DAC
 * Output is to the inbuilt DAC. Output resolution is 12 bits. Mono, only. The DAC output pin differs from board to boards.
 * In most cases the appropriate pin will be set outmatically. If needed, you can specify the DAC pin, explicitly:
 *
 * @code
 * #define MOZZI_AUDIO_PIN_1       ...  // Default: A14, A12, or A21, depending on board
 * @endcode
 *
 * @section teensy3_external MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM
 * See @ref external_audio
*/


/** @ingroup hardware
 * @page hardware_teensy4 Mozzi on Teensy 4.x boards.
 *
 * port by Thomas Combriat
 *
 * @note
 * For Teensy 3.x see @ref hardware_teensy3
 *
 * @section teensy4_status Port status and ussage notes
 * This port requires the following two libraries (which should be part of a default installtion, however):
 * - [Timer library](https://github.com/loglow/IntervalTimer) for Teensy 4.* by Daniel Gilbert
 * - [ADC library](http://github.com/pedvide/ADC) by Pedro Villanueva
 *
 * Contrary to the Teensy 3, the Teensy 4 do not have any DAC. The output is done on pin A8 (PWM) by default (see below).
 * twi_nonblock is not ported.
 *
 * @section teensy3_output Output modes
 * The following audio modes (see @ref MOZZI_AUDIO_MODE) are currently supported on this hardware:
 *   - MOZZI_OUTPUT_EXTERNAL_TIMED
 *   - MOZZI_OUTPUT_EXTERNAL_CUSTOM
 *   - MOZZI_OUTPUT_PWM
 *
 * The default mode is @ref teensy4_pwm .
 *
 * @section teensy4_pwm MOZZI_OUTPUT_PWM
 * Output is to a GPIO pin (or two in stereo). The output resolution is fixed at 10 bits, and a 146484 kHz carrier frequency.
 * The output pins can be configured as:
 *
 * @code
 * #define MOZZI_AUDIO_PIN_1       ...  // Left / mono output pin. Default: A8
 * // For stereo, only:
 * #define MOZZI_AUDIO_PIN_2       ...  // Right channel output pin. Default: A9
 * @endcode
 *
 * @section teensy4_external MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM
 * See @ref external_audio
*/


#if !(IS_TEENSY3() || IS_TEENSY4())
#error This header should be included for Teensy (3.x or 4.x) boards, only
#endif



#if IS_TEENSY3()
#  include "disable_2pinmode_on_github_workflow.h"
#  if !defined(MOZZI_AUDIO_MODE)
#    define MOZZI_AUDIO_MODE MOZZI_OUTPUT_INTERNAL_DAC
#  endif
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_EXTERNAL_CUSTOM, MOZZI_OUTPUT_INTERNAL_DAC)
#elif IS_TEENSY4()
#  include "disable_2pinmode_on_github_workflow.h"
#  if !defined(MOZZI_AUDIO_MODE)
#    define MOZZI_AUDIO_MODE MOZZI_OUTPUT_PWM
#  endif
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_EXTERNAL_CUSTOM, MOZZI_OUTPUT_PWM)
#endif

#if !defined(MOZZI_AUDIO_RATE)
#define MOZZI_AUDIO_RATE 32768
#endif

#if defined(MOZZI_PWM_RATE)
#error Configuration of MOZZI_PWM_RATE is not currently supported on this platform (always same as MOZZI_AUDIO_RATE)
#endif

#if !defined(MOZZI_ANALOG_READ)
#  define MOZZI_ANALOG_READ MOZZI_ANALOG_READ_STANDARD
#endif

MOZZI_CHECK_SUPPORTED(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_NONE, MOZZI_ANALOG_READ_STANDARD)

#include "teensyPinMap.h"

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
#  define MOZZI_AUDIO_BITS 12   // not configurable
#  if !defined(MOZZI_AUDIO_PIN_1)
#    if defined(__MKL26Z64__)
#      define MOZZI_AUDIO_PIN_1 A12
#    elif defined(__MK20DX128__) || defined(__MK20DX256__)
#      define MOZZI_AUDIO_PIN_1 A14
#    elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
#      define MOZZI_AUDIO_PIN_1 A21
#    else
#      error DAC pin not know for this board. Please define MOZZI_AUDIO_PIN_1 as appropriate
#    endif
#  endif
#  include "disable_stereo_on_github_workflow.h"
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_CHANNELS, 1)
#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM)
#  define MOZZI_AUDIO_BITS 10   // not configurable
#  if !defined(MOZZI_AUDIO_PIN_1)
#    define MOZZI_AUDIO_PIN_1 A8
#  endif
#  if !defined(MOZZI_AUDIO_PIN_2)
#    define MOZZI_AUDIO_PIN_2 A9
#  endif
#endif

//TODO: Not 100% sure this is correct in all cases.
#define MOZZI__INTERNAL_ANALOG_READ_RESOLUTION 10

#endif        //  #ifndef CONFIG_CHECK_TEENSY_H
