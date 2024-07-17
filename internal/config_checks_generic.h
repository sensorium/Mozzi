/*
 * config_checks_generic.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2023-2024 Thomas Friedrichsmeier and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

/** @page hardware Hardware and configuration
 *
 * Mozzi works on many different platforms, but not the same output modes are available on all hardware. For boards specific notes
 * and configuration options, see the relevant sub-pages in this section:
 *
 *   - @subpage hardware_avr
 *   - @subpage hardware_esp32
 *   - @subpage hardware_esp8266
 *   - @subpage hardware_mbed
 *   - @subpage hardware_renesas
 *   - @subpage hardware_rp2040
 *   - @subpage hardware_samd
 *   - @subpage hardware_stm32duino
 *   - @subpage hardware_stm32_maple
 *   - (@subpage hardware_stm32_disambiguation)
 *   - @subpage hardware_teensy3
 *   - @subpage hardware_teensy4
*/

/** For Mozzi-internal use: Check configuration options for (some) invalid settings, and apply default for options that have not been set, so far.
 * */

#ifndef MOZZI_CONFIG_CHECK_GENERIC_H
#define MOZZI_CONFIG_CHECK_GENERIC_H

#include "../MozziConfigValues.h"  // in case not user-included
#include "mozzi_macros.h"

/// Step 0: Check for some stuff that user should never configure directly (but may be set, indirectly from the hardware-specific setups)
#if defined(BYPASS_MOZZI_OUTPUT_BUFFER)
#error "BYPASS_MOZZI_OUTPUT_BUFFER may not be customized via config"
#endif


//// Step 1: Apply missing defaults for generic config options (not the hardware specific ones)
#if not defined(MOZZI_COMPATIBILITY_LEVEL)
#define MOZZI_COMPATIBILITY_LEVEL MOZZI_COMPATIBILITY_2_0
#endif

#if not defined(MOZZI_AUDIO_CHANNELS)
#define MOZZI_AUDIO_CHANNELS MOZZI_MONO
#endif

//MOZZI_AUDIO_MODE -> hardware specific
//MOZZI_AUDIO_RATE -> hardware specific

#if not defined(MOZZI_CONTROL_RATE)
#  if defined(CONTROL_RATE) && (MOZZI_COMPATIBILITY_LEVEL < MOZZI_COMPATIBILITY_LATEST)
#    warning Please change CONTROL_RATE to MOZZI_CONTROL_RATE
#    define MOZZI_CONTROL_RATE CONTROL_RATE
#  else
#    define MOZZI_CONTROL_RATE 64
#  endif
#endif

//MOZZI_ANALOG_READ -> hardware specific, but we want to insert a warning, if not supported, and user has not explicitly configured anything
#if not defined(MOZZI_ANALOG_READ)
#define MOZZI__ANALOG_READ_NOT_CONFIGURED
#endif

#if not defined(MOZZI_AUDIO_INPUT)
#define MOZZI_AUDIO_INPUT MOZZI_AUDIO_INPUT_NONE
#endif

#if !MOZZI_IS(MOZZI_AUDIO_INPUT, MOZZI_AUDIO_INPUT_NONE) && !defined(MOZZI_AUDIO_INPUT_PIN)
#warning Using audio input, but no audio input pin defined, explicitly. Defaulting to pin 0.
#define MOZZI_AUDIO_INPUT_PIN 0
#endif

//MOZZI_PWM_RATE -> hardware specific
//MOZZI_AUDIO_PIN_1 -> hardware specific
//MOZZI_AUDIO_PIN_1_LOW -> hardware specific
//MOZZI_AUDIO_PIN_2 -> hardware specific
//MOZZI_AUDIO_PIN_2_LOW -> hardware specific


/// Step 2: Include the hardware specific checks-and-defaults-header
#if IS_AVR()
#include "config_checks_avr.h"
#elif IS_ESP32()
#include "config_checks_esp32.h"
#elif IS_ESP8266()
#include "config_checks_esp8266.h"
#elif IS_MBED()
#include "config_checks_mbed.h"
#elif IS_RENESAS()
#include "config_checks_renesas.h"
#elif IS_RP2040()
#include "config_checks_rp2040.h"
#elif IS_SAMD21()
#include "config_checks_samd21.h"
#elif IS_STM32DUINO()
#include "config_checks_stm32duino.h"
#elif IS_STM32MAPLE()
#include "config_checks_stm32maple.h"
#elif (IS_TEENSY3() || IS_TEENSY4())
#include "config_checks_teensy.h"
#else
#error Problem detecting hardware
#endif


/// Step 2b: Minimal special handling for MOZZI_OUTPUT_EXTERNAL_TIMED/CUSTOM
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_EXTERNAL_CUSTOM) && !defined(MOZZI_AUDIO_BITS)
#  define MOZZI_AUDIO_BITS 16
#endif



/// Step 3: Apply various generic checks that make sense on more than one platform
MOZZI_CHECK_POW2(MOZZI_AUDIO_RATE)
MOZZI_CHECK_POW2(MOZZI_CONTROL_RATE)

#if MOZZI_IS(MOZZI_AUDIO_INPUT, MOZZI_AUDIO_INPUT_STANDARD) && MOZZI_IS(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_NONE)
#error "MOZZI_AUDIO_INPUT depends on MOZZI_ANALOG_READ option"
#endif

#if MOZZI_IS(MOZZI_AUDIO_INPUT, MOZZI_AUDIO_INPUT_NONE) && defined(MOZZI_AUDIO_INPUT_PIN)
#warning "MOZZI_AUDIO_INPUT_PIN defined without MOZZI_AUDIO_INPUT"
#endif

#if (MOZZI_AUDIO_CHANNELS < MOZZI_MONO) || (MOZZI_AUDIO_CHANNELS > MOZZI_STEREO)
#error "MOZZI_AUDIO_CHANNELS outside of (currently) supported range"
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED) || MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_CUSTOM)
#warning "Mozzi is configured to use an external void 'audioOutput(const AudioOutput f)' function. Please define one in your sketch"
#endif

// Hardware-specific checks file should have more narrow checks for most options, below, but is not required to, so let's check for anything that is wildly out of scope:
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_2PIN_PWM, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_EXTERNAL_CUSTOM, MOZZI_OUTPUT_PDM_VIA_I2S, MOZZI_OUTPUT_PDM_VIA_SERIAL, MOZZI_OUTPUT_I2S_DAC, MOZZI_OUTPUT_INTERNAL_DAC)
MOZZI_CHECK_SUPPORTED(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_NONE, MOZZI_ANALOG_READ_STANDARD)

#if defined(MOZZI__ANALOG_READ_NOT_CONFIGURED)
#  if MOZZI_IS(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_NONE)
#    warning Asynchronous analog reads not implemented on this platform
#  endif
#  undef MOZZI__ANALOG_READ_NOT_CONFIGURED
#endif

#if defined(MOZZI_ANALOG_READ_RESOLUTION)
#  if (MOZZI_ANALOG_READ_RESOLUTION < 1) || (MOZZI_ANALOG_READ_RESOLUTION > 16)
//   NOTE: We could certainly allow more than 16 bits, but then the data type would need to be adjusted/adjustable, accrodingly.
#    error MOZZI_ANALOG_READ_RESOLUTION must be between 1 and 16 bits
#  endif
#endif

/// Step 4: Init Read-only defines that depend on other values
#if !defined(MOZZI_AUDIO_BIAS)
#define MOZZI_AUDIO_BIAS ((uint16_t) 1<<(MOZZI_AUDIO_BITS-1))
#endif

#if !defined(MOZZI_AUDIO_BITS_OPTIMISTIC)
#define MOZZI_AUDIO_BITS_OPTIMISTIC MOZZI_AUDIO_BITS
#endif

// TODO: Rename these defines
#if MOZZI_AUDIO_RATE == 8192
#define AUDIO_RATE_AS_LSHIFT 13
#define MICROS_PER_AUDIO_TICK 122
#elif MOZZI_AUDIO_RATE == 16384
#define AUDIO_RATE_AS_LSHIFT 14
#define MICROS_PER_AUDIO_TICK 61 // 1000000 / 16384 = 61.035, ...* 256 = 15625
#elif MOZZI_AUDIO_RATE == 32768
#define AUDIO_RATE_AS_LSHIFT 15
#define MICROS_PER_AUDIO_TICK 31 // = 1000000 / 32768 = 30.518, ...* 256 = 7812.6
#elif MOZZI_AUDIO_RATE == 65336
#define AUDIO_RATE_AS_LSHIFT 16
#define MICROS_PER_AUDIO_TICK 15
#else
#error Whoopsie, not LSHIFT defined for this audio rate. Please report and/or fix
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_CUSTOM)
#define BYPASS_MOZZI_OUTPUT_BUFFER true
#endif

/// Step 5: Patch up some backwards compatibility issues as far as config-related
#if MOZZI_COMPATIBILITY_LEVEL < MOZZI_COMPATIBILITY_LATEST
#  define AUDIO_RATE MOZZI_AUDIO_RATE
#  if !defined(CONTROL_RATE)
#    define CONTROL_RATE MOZZI_CONTROL_RATE
#  endif
#endif

/// Step 6: Some more checks that need to be at the end, because of requiring end of the foodchain headers
// TODO: Rather move this up again, and make AudioOutputStorage_t a primary config option
#include "../AudioOutput.h"
static_assert(MOZZI_AUDIO_BITS <= (8*sizeof(AudioOutputStorage_t)), "Configured MOZZI_AUDIO_BITS is too large for the internal storage type");

#endif
