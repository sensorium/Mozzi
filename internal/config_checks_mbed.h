/*
 * config_checks_mbed.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2023-2024 Thomas Friedrichsmeier and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

#ifndef CONFIG_CHECK_MBED_H
#define CONFIG_CHECK_MBED_H

/**
 * @page hardware_mbed Mozzi on MBED-based boards (Arduino Giga / Portenta).
 *
 * port by Thomas Friedrichsmeier & Thomas Combriat
 *
 * @section mbed_status Port status and notes
 * Compiles and runs using Arduino's standard and Arduino_AdvancedAnalog libraries. This port is still **experimental**, testing reveals
 * some instabilities for some configurations (in particular with @ref MOZZI_AUDIO_INPUT) that are under active investigations.
 *
 * This port is not complete yet, in particular:
 *   - Asynchroneous analog reads are not implemented (yet), `mozziAnalogRead()` relays to `analogRead()`. (Note that @ref MOZZI_AUDIO_INPUT @em is implemented!)
 *   - This port should support other MBED based Arduino boards like the Arduino Portenta, in *theory*, but the developer have only tested it on the Giga. Feedback welcome!
 *
 * @section mbed_output Output modes
 * The following audio modes (see @ref MOZZI_AUDIO_MODE) are currently supported on this hardware:
 *   - MOZZI_OUTPUT_EXTERNAL_TIMED
 *   - MOZZI_OUTPUT_EXTERNAL_CUSTOM
 *   - MOZZI_OUTPUT_PDM_VIA_SERIAL
 *   - MOZZI_OUTPUT_INTERNAL_DAC
 *
 * The default mode is @ref mbed_internal_dac .
 *
 * @section mbed_internal_dac MOZZI_OUTPUT_INTERNAL_DAC
 * This uses the inbuild DAC on the board. The default is setting is appropriate for the Arduino Giga: 12 bits going to A13 (3.5mm jack connector's tip),
 * and in stereo mode to pin A12 (3.5mm jack connector's first ring) additionally.
 *
 * For other boards is may be appropriate to customize:
 * @code
 * #define MOZZI_AUDIO_PIN_1   ...   // mono / left channel; default: A13
 * #define MOZZI_AUDIO_PIN_2   ...   // stereo only: right channel; default: A12
 * #define MOZZI_AUDIO_BITS    ...   // default is 12
 * @endcode
 *
 * @section mbed_pdm_via_serial MOZZI_PDM_VIA_SERIAL
 * Returns a pulse-density modulated (mono only) signal on one of the hardware UARTs of the board (Serial ports). Default is using the SERIAL2, on pin D18.
 * You can confiugre the pins to use, but it must be connected to a hardware UART. Output is written to the TX pin, only, but the RX pin needs to be
 * claimed as well. Beware of confusing pinout labelling, for instance SERIAL2_TX iss labelled "TX1" on the Arduino Giga. The audio resolution can be enhanced
 * using @ref MOZZI_PDM_RESOLUTION, which is described in more detail here: @esp32_pdm_via_i2s .
 *
 * Configuration options:
 * @code
 * #define MOZZI_SERIAL_PIN_TX ...    // default: SERIAL2_TX
 * #define MOZZI_SERIAL_PIN_RX ...    // *must* specify the matching one, if customizing the above; default: SERIAL2_RX
 * #define MOZZI_PDM_RESOLUTION ...   // default value is 2, for 2*32 ones and zeros per audio sample
 * @endcode
 *
 * @section mbed_external MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM
 * See @ref external_audio
*/


#if not IS_MBED()
#error This header should be included for MBED architecture, only
#endif

#include "disable_2pinmode_on_github_workflow.h"
#if !defined(MOZZI_AUDIO_MODE)
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_INTERNAL_DAC
#endif
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_EXTERNAL_CUSTOM, MOZZI_OUTPUT_PDM_VIA_SERIAL, MOZZI_OUTPUT_INTERNAL_DAC)

#if !defined(MOZZI_AUDIO_RATE)
#define MOZZI_AUDIO_RATE 32768
#endif

#if defined(MOZZI_PWM_RATE)
#error Configuration of MOZZI_PWM_RATE is not currently supported on this platform (always same as MOZZI_AUDIO_RATE)
#endif

#if !defined(MOZZI_ANALOG_READ)
#  define MOZZI_ANALOG_READ MOZZI_ANALOG_READ_NONE
#endif

// yes, correct: async "single" reads are not implemented, but audio input is
MOZZI_CHECK_SUPPORTED(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_NONE)
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_INPUT, MOZZI_AUDIO_INPUT_NONE, MOZZI_AUDIO_INPUT_STANDARD)

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
#  if !defined(MOZZI_AUDIO_BITS)
#    define MOZZI_AUDIO_BITS 12
#  endif
#  if !defined(MOZZI_AUDIO_PIN_1)
#    define MOZZI_AUDIO_PIN_1 A13
#  endif
#  if !defined(MOZZI_AUDIO_PIN_2)
#    define MOZZI_AUDIO_PIN_2 A12
#  endif
#endif

#if !defined(MOZZI_AUDIO_BITS)
#  define MOZZI_AUDIO_BITS 16
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_SERIAL)
#  if !defined(MOZZI_PDM_RESOLUTION)
#    define MOZZI_PDM_RESOLUTION 2
#  endif
#  if !defined(MOZZI_SERIAL_PIN_TX)
#    define MOZZI_SERIAL_PIN_TX SERIAL2_TX
#  endif
#  if !defined(MOZZI_SERIAL_PIN_RX)
#    define MOZZI_SERIAL_PIN_RX SERIAL2_RX
#  endif
#endif

// All modes besides timed external bypass the output buffer!
#if !MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED)
#  define BYPASS_MOZZI_OUTPUT_BUFFER true
#endif

// TODO: This value is correct for Arduino Giga and Arduino Portenta, but not necessarily everywhere else
#define MOZZI__INTERNAL_ANALOG_READ_RESOLUTION 16

#endif        //  #ifndef CONFIG_CHECK_MBED_H
