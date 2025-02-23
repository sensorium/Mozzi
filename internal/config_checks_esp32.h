/*
 * config_checks_esp32.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2023-2024 Thomas Friedrichsmeier and the Mozzi Team
 * Copyright 2025 Thomas Combriat and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

#ifndef CONFIG_CHECK_ESP32_H
#define CONFIG_CHECK_ESP32_H

/**
 * @page hardware_esp32 Mozzi on ESP32-based boards.
 *
 * port by Dieter Vandoren, Thomas Friedrichsmeier and Thomas Combriat
 *
 * @section esp32_status Port status and notes
 *   - Since flash memory is not built into the ESP32, but connected, externally, it is much too slow for keeping wave tables, audio samples, etc. Instead, these are kept in RAM on this platform.
 *   - Asynchronous analog reads are not implemented. `mozziAnalogRead()` relays to `analogRead()`. @ref MOZZI_AUDIO_INPUT is not implemented
 *   - twi_nonblock is not ported
 *   - WIFI-activity not yet tested, but likely the same notes as for ESP8266 apply, i.e. @em any Wifi activity is likely to introdcue considerable nose. Consider turning off WIFI.
 *   - The implementation of audioTicks() may be slightly inaccurate on this platform.
 *
 * @section esp32_output Output modes
 * The following audio modes (see @ref MOZZI_AUDIO_MODE) are currently supported on this hardware:
 *   - MOZZI_OUTPUT_EXTERNAL_TIMED
 *   - MOZZI_OUTPUT_EXTERNAL_CUSTOM
 *   - MOZZI_OUTPUT_PDM_VIA_I2S
 *   - MOZZI_OUTPUT_I2S_DAC
 *   - MOZZI_OUTPUT_INTERNAL_DAC
 *   - MOZZI_OUTPUT_PWM
 *
 * The default mode is @ref esp32_internal_dac for the ESP32 (that has a built-in DAC) and @ref esp32_pwm for all other ESP32 flavours.
 *
 * @section esp32_internal_dac MOZZI_OUTPUT_INTERNAL_DAC
 * The internal DAC has 8 bit resolution, and outputs to GPIO pins 25 and 26 (non-configurable). For simplicity of code, both pins are always used.
 * In a mono configuration, both pins output the same sample.
 *
 * TODO: We could really use this to hack in a 2 PIN mode!
 *
 * @note
 * The number 25 refers to "GPIO 25" or sometimes labelled "D25". Confusingly, many boards come with an additional, totally different numbering scheme on top of that.
 * Check a detailed pinout diagram, if in any doubt.
 *
 * Internally, the inbuilt DAC is connected via an I2S interface. Which interface number to use can be configured using:
 *
 * @code
 * #define MOZZI_I2S_PORT     ... // (default: I2S_NUM_0)
 * @endcode
 *
 * @section esp32_i2s_dac MOZZI_OUTPUT_I2S_DAC
 * This mode outputs to a PT8211 (or compatible) I2S DAC, which allows for very high quality (mono or stereo) output. By default this mode outputs a plain I2S signal, with BCK, WS and DATA lines defined (but configurable, see after), which is equivalent to defining:
 *
 * @code
 * #define MOZZI_I2S_FORMAT MOZZI_I2S_FORMAT_PLAIN
 * @endcode
 *
 * The mode can be changed to LSBJ format (the PT8211 DAC for instance uses this format) by using:
 *
 * @code
 * #define MOZZI_I2S_FORMAT MOZZI_I2S_FORMAT_LSBJ
 * @endcode
 *
 * In addition to the three pins referred above, MCLK pin, which is of use in some DAC can be defined. 
 * Presumably, any pins qualify, and you can configure this using:
 * @code
 * #define MOZZI_I2S_PIN_BCK  ... // (default: 26)
 * #define MOZZI_I2S_PIN_WS   ... // (default: 15)
 * #define MOZZI_I2S_PIN_DATA ... // (default: 33)
 * #define MOZZI_I2S_PIN_MCLK ... // (default: UNDEFINED)
 * #define MOZZI_I2S_PORT     ... // (default: I2S_NUM_0)
 * @endcode
 *
 * For further customization, the signals outputted on some of these pins can be inverted, to accomodate DAC that would not fall into the standard, by using:
 * @code
 * #define MOZZI_I2S_BCK_INV  ... // (default: 0)
 * #define MOZZI_I2S_WS_INV   ... // (default: 0 for PLAIN format, 1 for LSBJ)
 * #define MOZZI_I2S_MBCK_INV ... // (default: 0)
 * @endcode
 *
 * @section esp32_pdm_via_i2s MOZZI_OUTPUT_PDM_VIA_I2S
 * This mode uses the same setup as @ref esp32_i2s_dac, but rather than using an external DAC, the communication signal itself is modulated in PDM
 * (pulse density modulation) encoded form. Thus not extra hardware is needed, and the signal is output on the DATA pin (see above). The BCK and
 * WS pins are also claimed, but should be left non-connected, and do not produce anything meaningful. This can only be used in mono mode.
 *
 * Output resolution may be adjusted by defining MOZZI_PDM_RESOLUTION , where the default value of 4 means that each audio sample is encoded into four 32 bit blocks
 * of ones and zeros. Obviously, more is potentially better, but at the cost of considerable computation power.
 *
 *  @section esp32_pwm MOZZI_OUTPUT_PWM
 * This mode outputs a standard PWM signal (10 bits) cadenced at MOZZI_AUDIO_RATE. Allegedly, any pin should qualify but by default it is set to GPIO 15 (left/mono) and GPIO 16 (right). They can be customized by using:
 * @code
 * #define MOZZI_AUDIO_PIN_1  ... // (default: 15)
 * #define MOZZI_AUDIO_PIN_2  ... // (default: 16)
 * @endcode
 *
 * @section esp32_external MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM
 * See @ref external_audio
*/

#if not IS_ESP32()
#error This header should be included for ESP32 architecture, only
#endif

#include "disable_2pinmode_on_github_workflow.h"
#if !defined(MOZZI_AUDIO_MODE)
#  if CONFIG_IDF_TARGET_ESP32
#    define MOZZI_AUDIO_MODE MOZZI_OUTPUT_INTERNAL_DAC
#  else // only ESP32 carries an internal DAC
#    define MOZZI_AUDIO_MODE MOZZI_OUTPUT_PWM
#  endif
#endif
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_EXTERNAL_CUSTOM, MOZZI_OUTPUT_PDM_VIA_I2S, MOZZI_OUTPUT_I2S_DAC, MOZZI_OUTPUT_INTERNAL_DAC, MOZZI_OUTPUT_PWM)

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
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_INPUT, MOZZI_AUDIO_INPUT_NONE)

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC)
#  if !defined(MOZZI_I2S_FORMAT)
#    define MOZZI_I2S_FORMAT MOZZI_I2S_FORMAT_PLAIN
#  endif
#  if !defined(MOZZI_I2S_PIN_BCK)
#    define MOZZI_I2S_PIN_BCK 26
#  endif
#  if !defined(MOZZI_I2S_PIN_WS)
#    define MOZZI_I2S_PIN_WS 25
#  endif
#  if !defined(MOZZI_I2S_PIN_DATA)
#    define MOZZI_I2S_PIN_DATA 33
#  endif
#  if !defined(MOZZI_I2S_PIN_MCLK)
#    define MOZZI_I2S_PIN_MCLK I2S_GPIO_UNUSED
#  endif
#  if !defined(MOZZI_I2S_BCK_INV)
#    define MOZZI_I2S_BCK_INV 0
#  endif
#  if !defined(MOZZI_I2S_WS_INV)
#    if MOZZI_IS(MOZZI_I2S_FORMAT, MOZZI_I2S_FORMAT_PLAIN)
#      define MOZZI_I2S_WS_INV 0
#    elif MOZZI_IS(MOZZI_I2S_FORMAT, MOZZI_I2S_FORMAT_LSBJ)
#      define MOZZI_I2S_WS_INV 1
#    endif
#  endif
#  if !defined(MOZZI_I2S_MBCK_INV)
#    define MOZZI_I2S_MBCK_INV 0
#  endif
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC, MOZZI_OUTPUT_PDM_VIA_I2S)
#  if !defined(MOZZI_I2S_FORMAT)
#    define MOZZI_I2S_FORMAT MOZZI_I2S_FORMAT_PLAIN
#  endif
#  if !defined(MOZZI_I2S_PIN_BCK)
#    define MOZZI_I2S_PIN_BCK I2S_GPIO_UNUSED
#  endif
#  if !defined(MOZZI_I2S_PIN_WS)
#    define MOZZI_I2S_PIN_WS I2S_GPIO_UNUSED
#  endif
#  if !defined(MOZZI_I2S_PIN_DATA)
#    define MOZZI_I2S_PIN_DATA 33
#  endif
#  if !defined(MOZZI_I2S_PIN_MCLK)
#    define MOZZI_I2S_PIN_MCLK I2S_GPIO_UNUSED
#  endif
#  define MOZZI_I2S_WS_INV 0 // unused but need to be defined
#  define MOZZI_I2S_BCK_INV 0
#  define MOZZI_I2S_MBCK_INV 0
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE/*, MOZZI_OUTPUT_INTERNAL_DAC*/, MOZZI_OUTPUT_I2S_DAC, MOZZI_OUTPUT_PDM_VIA_I2S)
//#  include <driver/i2s.h>
#  if !defined(MOZZI_IS2_PORT)
#    define MOZZI_I2S_PORT I2S_NUM_0
#  endif
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM)
#  define MOZZI_AUDIO_BITS 10   // not configurable (could be 8 at AUDIO_RATE, working combinations are not specified on espressif's doc)
#  if !defined(MOZZI_AUDIO_PIN_1)
#    define MOZZI_AUDIO_PIN_1 15
#  endif
#  if !defined(MOZZI_AUDIO_PIN_2)
#    define MOZZI_AUDIO_PIN_2 16
#  endif
#endif

#if !defined(MOZZI_AUDIO_BITS)
#  if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
#    define MOZZI_AUDIO_BITS 8
#  else
#    define MOZZI_AUDIO_BITS 16
#  endif
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S)
#  define MOZZI_I2S_FORMAT MOZZI_I2S_FORMAT_LSBJ  // pdm codes are for LSBJ format
#  if !defined(MOZZI_PDM_RESOLUTION)
#    define MOZZI_PDM_RESOLUTION 8 
#  endif
#else
#  define MOZZI_PDM_RESOLUTION 1  // unconditionally, no other value allowed
#endif


// All modes besides timed external bypass the output buffer!
#if !MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_INTERNAL_DAC, MOZZI_OUTPUT_PWM)
#  define BYPASS_MOZZI_OUTPUT_BUFFER true
#endif

#define MOZZI__INTERNAL_ANALOG_READ_RESOLUTION 12

#endif        //  #ifndef CONFIG_CHECK_ESP32_H
