/*
 * MozziConfigValues.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2023-2024 Thomas Friedrichsmeier and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
 */

/** @file MozziConfigValues.h 
 * This file keeps a list of named configuration values. 
 *
 * Note that these are all given as defines, instead of e.g. const ints or enum values, because they need to be usable at preprocessor level, in order to control conditional compilation.
*/

#ifndef MOZZICONFIG_VALUES_H
#define MOZZICONFIG_VALUES_H



#define MOZZI_MONO 1
#define MOZZI_STEREO 2

// We try to use distinct values as much as possible so we can catch semantic errors like "#define MOZZI_AUDIO_MODE MOZZI_STEREO"
#define MOZZI_OUTPUT_PWM 101
#define MOZZI_OUTPUT_2PIN_PWM 102
#define MOZZI_OUTPUT_EXTERNAL_TIMED 103
#define MOZZI_OUTPUT_EXTERNAL_CUSTOM 104
#define MOZZI_OUTPUT_PDM_VIA_I2S 105
#define MOZZI_OUTPUT_PDM_VIA_SERIAL 106
#define MOZZI_OUTPUT_I2S_DAC 107
#define MOZZI_OUTPUT_INTERNAL_DAC 108

#define MOZZI_AUDIO_INPUT_NONE 201
#define MOZZI_AUDIO_INPUT_STANDARD 202

#define MOZZI_ANALOG_READ_NONE 301
#define MOZZI_ANALOG_READ_STANDARD 302

#define MOZZI_I2S_FORMAT_PLAIN 401
#define MOZZI_I2S_FORMAT_LSBJ 402

// defined with some space in between, just in case. This should be numerically ordered.
#define MOZZI_COMPATIBILITY_1_1 1100
#define MOZZI_COMPATIBILITY_2_0 2000
#define MOZZI_COMPATIBILITY_LATEST 9000 // May be upped, arbitrarily

// For convenience
#include "hardware_defines.h"


#endif
