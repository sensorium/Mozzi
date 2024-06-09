/*
 * config_checks_esp32.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2023-2024 Thomas Friedrichsmeier and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

#ifndef CONFIG_CHECK_ESP32_H
#define CONFIG_CHECK_ESP32_H

/**
 * @page hardware_esp32 Mozzi on ESP32-based boards.
 *
 * port by Dieter Vandoren and Thomas Friedrichsmeier
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
 *
 * The default mode is @ref esp32_internal_dac .
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
 * This mode outputs to a PT8211 (or compatible) I2S DAC, which allows for very high quality (mono or stereo) output. Communication needs the BCK, WS, and DATA(out) pins
 * of one I2S interface. Presumably, any pins qualify, and you can configure this using:
 * @code
 * #define MOZZI_I2S_PIN_BCK  ... // (default: 26)
 * #define MOZZI_I2S_PIN_WS   ... // (default: 15)
 * #define MOZZI_I2S_PIN_DATA ... // (default: 33)
 * #define MOZZI_I2S_PORT     ... // (default: I2S_NUM_0)
 * @endcode
 *
 * See the note above (@ref esp_internal_dac) regarding pin numbering. Also, please always test the default pinout, should a custom setting fail!
 *
 * As a technical note, I2S support in the ESP32 SDK has been reworked since this was implemented in Mozzi, and Mozzi uses the "legacy" implementation "i2s.h".
 * This should not be an issue, unless you want to connect additional I2S components, yourself. In which case contributions are certainly welcome!
 *
 * @section esp32_pdm_via_i2s MOZZI_OUTPUT_PDM_VIA_I2S
 * This mode uses the same setup as @ref esp32_i2s_dac, but rather than using an external DAC, the communication signal itself is modulated in PDM
 * (pulse density modulation) encoded form. Thus not extra hardware is needed, and the signal is output on the DATA pin (see above). The BCK and
 * WS pins are also claimed, but should be left non-connected, and do not produce anything meaningful. This can only be used in mono mode.
 *
 * Output resolution may be adjusted by defining MOZZI_PDM_RESOLUTION , where the default value of 4 means that each audio sample is encoded into four 32 bit blocks
 * of ones and zeros. Obviously, more is potentially better, but at the cost of considerable computation power.
 *
 * @section esp32_external MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM
 * See @ref external_audio
*/

#if not IS_ESP32()
#error This header should be included for ESP32 architecture, only
#endif

#include "disable_2pinmode_on_github_workflow.h"
#if !defined(MOZZI_AUDIO_MODE)
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_INTERNAL_DAC
#endif
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_EXTERNAL_CUSTOM, MOZZI_OUTPUT_PDM_VIA_I2S, MOZZI_OUTPUT_I2S_DAC, MOZZI_OUTPUT_INTERNAL_DAC)

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

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC, MOZZI_OUTPUT_PDM_VIA_I2S)
#  if !defined(MOZZI_I2S_PIN_BCK)
#    define MOZZI_I2S_PIN_BCK 26
#  endif
#  if !defined(MOZZI_I2S_PIN_WS)
#    define MOZZI_I2S_PIN_WS 25
#  endif
#  if !defined(MOZZI_I2S_PIN_DATA)
#    define MOZZI_I2S_PIN_DATA 33
#  endif
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC, MOZZI_OUTPUT_I2S_DAC, MOZZI_OUTPUT_PDM_VIA_I2S)
#  include <driver/i2s.h>
#  if !defined(MOZZI_IS2_PORT)
#    define MOZZI_I2S_PORT I2S_NUM_0
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
#  if !defined(MOZZI_PDM_RESOLUTION)
#    define MOZZI_PDM_RESOLUTION 8
#  endif
#else
#  define MOZZI_PDM_RESOLUTION 1  // unconditionally, no other value allowed
#endif

// All modes besides timed external bypass the output buffer!
#if !MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED)
#  define BYPASS_MOZZI_OUTPUT_BUFFER true
#endif

#define MOZZI__INTERNAL_ANALOG_READ_RESOLUTION 12

#endif        //  #ifndef CONFIG_CHECK_ESP32_H
