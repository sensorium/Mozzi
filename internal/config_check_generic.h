/** For Mozzi-internal use: Check configuration options for (some) invalid settings, and apply default for options that have not been set, so far.
 * */

#ifndef MOZZI_CONFIG_CHECK_GENERIC_H
#define MOZZI_CONFIG_CHECK_GENERIC_H

#include <MozziConfigValues.h>  // in case not user-included

//// Step 1: Apply missing defaults for generic config options (not the hardware specific ones)
#if not defined(MOZZI_COMPATIBILITY_LEVEL)
#define MOZZI_COMPATIBILITY_LEVEL MOZZI_COMPATIBILITY_2_0
#endif

#if not defined(MOZZI_AUDIO_CHANNELS)
#if (MOZZI_COMPATIBILITY_LEVEL <= MOZZI_COMPATIBILITY_1_1) && (STEREO_HACK == true)
#warning Use of STEREO_HACK is deprecated. Use AUDIO_CHANNELS STEREO, instead.
#define MOZZI_AUDIO_CHANNELS MOZZI_STEREO
#else
#define MOZZI_MONO
#endif
#endif

//MOZZI_AUDIO_MODE -> hardware specific
//MOZZI_AUDIO_RATE -> hardware specific

#if not defined(MOZZI_CONTROL_RATE)
#define MOZZI_CONTROL_RATE 64
#endif

//MOZZI_ANALOG_READ -> hardware specific

#if not defined(MOZZI_AUDIO_INPUT)
#define MOZZI_AUDIO_INPUT_DISABLED
#endif

#if not defined(MOZZI_AUDIO_INPUT_PIN)
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
#else
// TODO

#endif


/// Step 2b: Minimal special handling for MOZZI_OUTPUT_EXTERNAL
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL) && !defined(MOZZI_AUDIO_BITS)
#define MOZZI_AUDIO_BITS 16
#endif



/// Step 3: Apply various generic checks that make sense on more than one platform
MOZZI_CHECK_POW2(MOZZI_AUDIO_RATE)
MOZZI_CHECK_POW2(MOZZI_CONTROL_RATE)

#if MOZZI_IS(MOZZI_AUDIO_INPUT, MOZZI_AUDIO_INPUT_STANDARD) && MOZZI_IS(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_NONE)
#error "MOZZI_AUDIO_INPUT depends on MOZZI_ANALOG_READ option"
#endif

#if !MOZZI_IS(MOZZI_AUDIO_INPUT, MOZZI_AUDIO_INPUT_NONE) && defined(MOZZI_AUDIO_INPUT_PIN)
#warning "MOZZI_AUDIO_INPUT_PIN defined without MOZZI_AUDIO_INPUT"
#endif

#if (MOZZI_AUDIO_CHANNELS < MOZZI_MONO) || (MOZZI_AUDIO_CHANNELS > MOZZI_STEREO)
#error "MOZZI_AUDIO_CHANNELS outside of (currently) supported range"
#endif

// Hardware-specific checks file should have more narrow checks for most options, below, but is not required to, so let's check for anything that is wildly out of scope:
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_2PIN_PWM, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_EXTERNAL_CUSTOM, MOZZI_OUTPUT_PDM_VIA_I2S, MOZZI_OUTPUT_PDM_VIA_SERIAL, MOZZI_OUTPUT_I2S_DAC, MOZZI_OUTPUT_INTERNAL_DAC)
MOZZI_CHECK_SUPPORTED(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_NONE, MOZZI_ANALOG_READ_STANDARD)
static_assert(MOZZI_AUDIO_BITS <= (4*sizeof(AudioOutputStorage_t)), "Configured MOZZI_AUDIO_BITS is too large for the internal storage type");



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
#if MOZZI_AUDIO_RATE == 16384
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


/// Step 5: Patch up some backwards compatibility issues as far as config-related
#if MOZZI_COMPATIBILITY_LEVEL < MOZZI_COMPATIBILITY_LATEST
#define AUDIO_RATE MOZZI_AUDIO_RATE
#define CONTROL_RATE MOZZI_CONTROL_RATE
#endif




#endif
