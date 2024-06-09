/*
 * config_checks_renesas.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2023-2024 Thomas Friedrichsmeier and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

#ifndef CONFIG_CHECK_RENESAS_H
#define CONFIG_CHECK_RENESAS_H

/**
 * @page hardware_renesas Mozzi on Arduino Uno R4 - Renesas.
 *
 * port by Thomas Combriat
 *
 * @section renesas_status Port status and notes
 * Compiles and runs using Arduino's standard library (Renesas 0.8.7 at the time of this writing).
 *
 * A few particularities:
 *   - Because this board has an on-board DAC (A0), but only one, STEREO is not implemented and Mozzi uses this pin. Usage of other pins using PWM for instance is not implemented yet.
 *   - getAudioInput() and mozziAnalogRead() return values in the Renesas' full ADC resolution of 0-16384 rather than AVR's 0-1023. *This might change in the near future for speed purposes.*
 *
 * @section rensesas_output Output modes
 * The following audio modes (see @ref MOZZI_AUDIO_MODE) are currently supported on this hardware:
 *   - MOZZI_OUTPUT_EXTERNAL_TIMED
 *   - MOZZI_OUTPUT_EXTERNAL_CUSTOM
 *   - MOZZI_OUTPUT_INTERNAL_DAC
 *
 * The default mode is @ref renesas_internal_dac. Further modes may be added in the future.
 *
 * @section renesas_internal_dac MOZZI_OUTPUT_INTERNAL_DAC
 * This uses the inbuild DAC on the board on pin A0. Mono output only, and the pin is not configurable. Audio resolution is also fixed at 12 bits (which is what the board supports).
 *
 * This mode claims two timers (but it is not hardcoded, which ones).
 *
 * @section renesas_external MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM
 * MOZZI_OUTPUT_EXTERNAL_TIMED claimes one timer, MOZZI_OUTPUT_EXTERNAL_CUSTOM does not claim any timer.
 * See @ref external_audio
*/

#if not IS_RENESAS()
#error This header should be included for RENESAS (Arduino Uno R4) architecture, only
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
#  define MOZZI_ANALOG_READ MOZZI_ANALOG_READ_STANDARD
#endif

MOZZI_CHECK_SUPPORTED(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_NONE, MOZZI_ANALOG_READ_STANDARD)

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
#  if !defined(MOZZI_AUDIO_BITS)
#    define MOZZI_AUDIO_BITS 12
#  endif
#  if !defined(MOZZI_AUDIO_PIN_1)
#    define MOZZI_AUDIO_PIN_1 A0
#  endif
#  define BYPASS_MOZZI_OUTPUT_BUFFER true
#  include "disable_stereo_on_github_workflow.h"
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_CHANNELS, 1)
#endif

#define MOZZI__INTERNAL_ANALOG_READ_RESOLUTION 14

#endif        //  #ifndef CONFIG_CHECK_RENESAS_H
