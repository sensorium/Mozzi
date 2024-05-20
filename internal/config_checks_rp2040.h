/*
 * config_checks_rp2040.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2023-2024 Thomas Friedrichsmeier and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

#ifndef CONFIG_CHECK_RP2040_H
#define CONFIG_CHECK_RP2040_H

/**
 * @page hardware_rp2040 Mozzi on RP2040 (Raspberry Pi Pico)
 *
 * port by Thomas Friedrichsmeier
 *
 * @section rp2040_status Port status and notes
 * Compiles and runs using [this core](https://github.com/earlephilhower/arduino-pico). Can probably be ported to the Mbed core for RP2040, relatively easily,
 * as it relies mostly on the RP2040 SDK API. Tested on a Pi Pico.
 *
 *   - This is a recent addition, implementation details may still change (currently just PWM driven by a timer; this may be worth changing to a DMA driven output)
 *   - Wavetables and samples are not kept in progmem on this platform. While apparently speed (of the external flash) is not much of an issue, the data always seems to be copied into RAM, anyway.
 *   - Note that getAudioInput() and mozziAnalogRead() return values in the RP2040's full ADC resolution of 0-4095 rather than AVR's 0-1023.
 *   - twi_nonblock is not ported
 *   - Code uses only one CPU core
 *
 * @section rp2040_output Output modes
 * The following audio modes (see @ref MOZZI_AUDIO_MODE) are currently supported on this hardware:
 *   - MOZZI_OUTPUT_EXTERNAL_TIMED
 *   - MOZZI_OUTPUT_EXTERNAL_CUSTOM
 *   - MOZZI_OUTPUT_PWM
 *   - MOZZI_OUTPUT_I2S_DAC
 *
 * The default mode is @ref rp2040_pwm .
 *
 * @section rp2040_pwm MOZZI_OUTPUT_PWM
 * Audio output is written to pin 0 (mono) or 0 and 1 (stereo), by default, with 11 bits of ouput resolution.
 * One hardware timer interrupt and one DMA channel are claimed (number not hardcoded), a non-exclusive handler is installed on DMA_IRQ_0.
 *
 * Configuration options:
 * @code
 * #define MOZZI_AUDIO_PIN_1   ...  // default is 0
 * #define MOZZI_AUDIO_BITS    ...  // output resolution (bits); default is 11
 * // additionally, for stereo:
 * #define MOZZI_AUDIO_PIN_2   ...  // default is 1; this must be on the same PWM slice as the first pin (i.e. neighboring)
 * @endcode
 *
 * @section rp2040_i2s_dac MOZZI_OUTPUT_I2S_DAC
 * Output to an external DAC, connected via I2S. This uses 16 bit (per audio channel), but can be changed to 8, 16, 24 (left aligned) and 32 resolution.
 * Both plain I2S and LSBJ format (PT8211 needs this, for instance) are available. Plain format is used by default. The GPIO pins to use can be configured,
 * - almost - freely (see below). Two DMA channels are claimed (numbers not hardcoded), non-exclusive handlers are installed on DMA_IRQ_0.
 *
 * Configuration options:
 * @code
 * #define MOZZI_AUDIO_BITS    ...  // available values are 8, 16 (default), 24 (LEFT ALIGN in 32 bits type!!) and 32 bits
 * #define MOZZI_I2S_PIN_BCK                        ... // /BLCK) default is 20
 * //#define MOZZI_I2S_PIN_WS (MOZZI_I2S_PIN_BCK+1) ... // CANNOT BE CHANGED, HAS TO BE NEXT TO pBCLK, i.e. default is 21
 * #define MOZZI_I2S_PIN_DATA                       ... // (DOUT) default is 22
 * #define MOZZI_I2S_FORMAT                         ... // may be MOZZI_I2S_FORMAT_LSBJ or MOZZI_I2S_FORMAT_PLAIN (default)
 * @endcode
 *
 * @note
 * The MOZZI_I2S_FORMAT_LSBJ option may require a relatively recent git-hub checkout of the arduino-pico core.
 *
 * @section rp2040_external MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM
 * See @ref external_audio
*/

#if not IS_RP2040()
#error This header should be included for RP2040 architecture (Raspberry Pi Pico and others), only
#endif

#include "disable_2pinmode_on_github_workflow.h"
#if !defined(MOZZI_AUDIO_MODE)
#  define MOZZI_AUDIO_MODE MOZZI_OUTPUT_PWM
#endif
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_EXTERNAL_CUSTOM, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_I2S_DAC)

#if !defined(MOZZI_AUDIO_RATE)
#define MOZZI_AUDIO_RATE 32768
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM)
#  if !defined(MOZZI_AUDIO_BITS)
#    define MOZZI_AUDIO_BITS 16 // PWMAudio expects 16bits
#  endif
#  if !defined(MOZZI_AUDIO_PIN_1)
#    define MOZZI_AUDIO_PIN_1 0
#  endif
#  if !defined(MOZZI_AUDIO_PIN_2)
#    define MOZZI_AUDIO_PIN_2 1
#  endif
#  define BYPASS_MOZZI_OUTPUT_BUFFER true
#  define MOZZI_RP2040_BUFFERS 8       // number of DMA buffers used
#  define MOZZI_RP2040_BUFFER_SIZE 256  // total size of the buffer, in samples
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC)
#  if !defined(MOZZI_AUDIO_BITS)
#    define MOZZI_AUDIO_BITS 16
#  endif
#  if !defined(MOZZI_I2S_PIN_BCK)
#    define MOZZI_I2S_PIN_BCK 20
#  endif
//#  define MOZZI_IS_PIN_WS(MOZZI_I2S_PIN_BCK + 1)  // implicit
#  if !defined(MOZZI_I2S_PIN_DATA)
#    define MOZZI_I2S_PIN_DATA 22
#  endif
#  if !defined(MOZZI_I2S_FORMAT)
#    define MOZZI_I2S_FORMAT MOZZI_I2S_FORMAT_PLAIN
#  endif
MOZZI_CHECK_SUPPORTED(MOZZI_I2S_FORMAT, MOZZI_I2S_FORMAT_PLAIN, MOZZI_I2S_FORMAT_LSBJ)
#  define BYPASS_MOZZI_OUTPUT_BUFFER true
#  define MOZZI_RP2040_BUFFERS 8       // number of DMA buffers used
#  define MOZZI_RP2040_BUFFER_SIZE 256  // total size of the buffer, in samples
#endif

#if !defined(MOZZI_ANALOG_READ)
#  define MOZZI_ANALOG_READ MOZZI_ANALOG_READ_STANDARD
#endif

#define MOZZI__INTERNAL_ANALOG_READ_RESOLUTION 12

MOZZI_CHECK_SUPPORTED(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_NONE, MOZZI_ANALOG_READ_STANDARD)
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_INPUT, MOZZI_AUDIO_INPUT_NONE, MOZZI_ANALOG_READ_STANDARD)

#endif        //  #ifndef CONFIG_CHECK_RP2040_H
