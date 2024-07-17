/*
 * config_checks_samd21.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2023-2024 Thomas Friedrichsmeier and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

#ifndef CONFIG_CHECK_SAMD21_H
#define CONFIG_CHECK_SAMD21_H

/**
 * @page hardware_samd Mozzi on SAMD21 based boards (Arduino Circuitplayground M0 and others)
 *
 * port by Adrian Freed
 *
 * @section samd_status Port status and notes
 *   - @def MOZZI_ANALOG_READ and MOZZI_ANALOG_INPUT are not implemented (contributions welcome)
 *   - We don't have a lot of data, which boards this port has been tested on. Success or not, let us know, if you are using Mozzi on SAMD21 boards
 *
 * @section samd_output_modes Output modes
 * The following audio modes (see @ref MOZZI_AUDIO_MODE) are currently supported on this hardware:
 *   - MOZZI_OUTPUT_EXTERNAL_TIMED
 *   - MOZZI_OUTPUT_EXTERNAL_CUSTOM
 *   - MOZZI_OUTPUT_INTERNAL_DAC
 *
 * The default mode is @ref samd_internal_dac , meaning, only boards with an inbuilt DAC are covered by default
 * (you could stil use one of the external output modes, however).
 *
 * @section samd_internal_dac MOZZI_OUTPUT_INTERNAL_DAC
 * Output resolution is 10 bits by default, and goes to pin DAC0. Only mono output is supported. Within the hardware limits of your board, you can configure the following:
 *
 * @code
 * #define MOZZI_AUDIO_PIN_1   ...  // default is DAC0
 * #define MOZZI_AUDIO_BITS    ...  // default is 10
 * @endcode
 *
 * @section samd_external MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM
 * See @ref external_audio
*/

#if not IS_SAMD21()
#error This header should be included for SAMD21 architecture (Arduino Circuitplayground M0 and others), only
#endif

#include "disable_2pinmode_on_github_workflow.h"
#if !defined(MOZZI_AUDIO_MODE)
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_INTERNAL_DAC
#endif
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_EXTERNAL_CUSTOM, MOZZI_OUTPUT_INTERNAL_DAC)

#if !defined(MOZZI_AUDIO_RATE)
#define MOZZI_AUDIO_RATE 32768
#endif

#if defined(MOZZI_PWM_RATE)
#error Configuration of MOZZI_PWM_RATE is not currently supported on this platform (always same as MOZZI_AUDIO_RATE)
#endif

#if !defined(MOZZI_ANALOG_READ)
#  define MOZZI_ANALOG_READ MOZZI_ANALOG_READ_NONE
#endif

MOZZI_CHECK_SUPPORTED(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_NONE)

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
#  if !defined(MOZZI_AUDIO_BITS)
#    define MOZZI_AUDIO_BITS 10
#  endif
#  if !defined(MOZZI_AUDIO_PIN_1)
#    define MOZZI_AUDIO_PIN_1 DAC0
#  endif
#  include "disable_stereo_on_github_workflow.h"
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_CHANNELS, 1)
#endif

#define MOZZI__INTERNAL_ANALOG_READ_RESOLUTION 12

#endif        //  #ifndef CONFIG_CHECK_SAMD21_H
