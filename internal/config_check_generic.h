/** For Mozzi-internal use: Check configuration options for (some) invalid settings, and apply default for options that have not been set, so far.
 * */

#ifndef MOZZI_CONFIG_CHECK_GENERIC_H
#define MOZZI_CONFIG_CHECK_GENERIC_H


//// Step 1: Apply missing defaults for generic config options (not hardware specific)
#if not defined(MOZZI_COMPATIBILITY_LEVEL)
#define MOZZI_COMPATIBILITY_LEVEL MOZZI_COMPATIBILITY_2_0
#endif

#if not defined(MOZZI_AUDIO_CHANNELS)
#define MOZZI_MONO
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

// Hardware-specific checks file should have more narrow checks for most options, below, but is not required to:
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_2PIN_PWM, MOZZI_OUTPUT_EXTERNAL,MOZZI_OUTPUT_PDM_VIA_I2S, MOZZI_OUTPUT_PDM_VIA_SERIAL, MOZZI_OUTPUT_I2S_DAC, MOZZI_OUTPUT_INTERNAL_DAC)
MOZZI_CHECK_SUPPORTED(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_NONE, MOZZI_ANALOG_READ_STANDARD)


// TODO: much more


/// Step 4: Patch up some backwards compatibility issues as far as config-related
#if MOZZI_COMPATIBILITY_LEVEL < MOZZI_COMPATIBILITY_LATEST
#define AUDIO_RATE MOZZI_AUDIO_RATE
#define CONTROL_RATE MOZZI_CONTROL_RATE
#endif

#endif
