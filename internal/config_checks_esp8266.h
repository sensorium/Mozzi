/*
 * config_checks_esp8266.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2023-2024 Thomas Friedrichsmeier and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

#ifndef CONFIG_CHECK_ESP8266_H
#define CONFIG_CHECK_ESP8266_H

/**
 * @page hardware_esp8266 Mozzi on ESP8266-based boards.
 *
 * port by Thomas Friedrichsmeier
 *
 * @section esp8266_status Port status and notes
 *   - Since flash memory is not built into the ESP8266, but connected, externally, it is much too slow for keeping wave tables, audio samples, etc. Instead, these are kept in RAM on this platform.
 *   - Asynchronous analog reads are not implemented. `mozziAnalogRead()` relays to `analogRead()`. @ref MOZZI_AUDIO_INPUT is not available
 *   - twi_nonblock is not ported
 *   - Note that the ESP8266 pins can output less current than the other supported CPUs. The maximum is 12mA, with a recommendation to stay below 6mA.
 *      - WHEN CONNECTING A HEADPHONE, DIRECTLY, USE APPROPRIATE CURRENT LIMITING RESISTORS (>= 500Ohms).
 *   - _Any_ WiFi-activity can cause severe spikes in power consumption. This can cause audible "ticking" artifacts, long before any other symptoms.
 *      - If you do not require WiFi in your sketch, you should turn it off, _explicitly_, using @code WiFi.mode(WIFI_OFF) @endcode.
 *   - A juicy enough, well regulated power supply, and a stabilizing capacitor between VCC and Gnd can help a lot.
 *   - As the (PDM) output signal is digital, a single (fast!) transistor can be used to amplify it to an independent voltage level.
 *   - audioHook() calls `yield()` once for every audio sample generated. Thus, as long as your audio output buffer does not run empty, you should not need any additional `yield()`s inside `loop()`.
 *
 * @section esp8266_output Output modes
 * The following audio modes (see @ref MOZZI_AUDIO_MODE) are currently supported on this hardware:
 *   - MOZZI_OUTPUT_EXTERNAL_TIMED
 *   - MOZZI_OUTPUT_EXTERNAL_CUSTOM
 *   - MOZZI_OUTPUT_PDM_VIA_I2S
 *   - MOZZI_OUTPUT_PDM_VIA_SERIAL
 *   - MOZZI_OUTPUT_I2S_DAC
 *
 * The default mode is @ref esp8266_pdm_via_serial .
 *
 * @note
 * This port really does not currently come with a PWM mode!
 *
 * @section esp8266_pdm_via_serial MOZZI_OUTPUT_PDM_VIA_SERIAL
 * Output is coded using pulse density modulation, and sent via GPIO2 (Serial1 TX).
 *   - This output mode uses timer1 for queuing audio sample, so that timer is not available for other uses.
 *   - Note that this mode has slightly lower effective analog output range than @ref esp8266_pdm_via_i2s, due to start/stop bits being added to the output stream.
 *   - Supports mono output, only, pins not configurable.
 *
 * The option @ref MOZZI_PDM_RESOLTUON (default value 2, corresponding to 64 ones and zeros per audio sample) can be used to adjust the output resolution.
 * Obviously higher values demand more computation power.
 *
 * @section esp8266_pdm_via_serial MOZZI_OUTPUT_PDM_VIA_I2S
 * Output is coded using pulse density modulation, and sent via the I2S pins. The I2S data out pin (GPIO3, which is also "RX") will have the output,
 * but *all* I2S output pins (RX, GPIO2 and GPIO15) will be affected. Mozzi tries to set GPIO2 and GPIO15 to input mode, and *at the time of this writing*, this allows
 * I2S output on RX even on boards such as the ESP01 (where GPIO15 is tied to Ground). However, it seems safest to assume that this mode may not be useable on boards where
 * GPIO2 or GPIO15 are not available as output pins.
 *
 * Supports mono output, only, pins not configurable.
 *
 * Resolution may be controlled using MOZZI_PDM_RESOLUTION (see above; default value is 2).
 *
 * @section esp8266_i2s_dac MOZZI_OUTPUT_I2S_DAC
 * Output is sent to an external DAC (such as a PT8211), digitally coded. This is the only mode that supports stereo (@ref MOZZI_AUDIO_CHANNELS). It also needs the least processing power.
 * The pins cannot be configured (GPIO3/RX, GPIO2, and GPIO15).
 *
 * @section esp8266_external MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM
 * See @ref external_audio
*/

#if not IS_ESP8266()
#error This header should be included for ESP architecture, only
#endif

#include "disable_2pinmode_on_github_workflow.h"
#if !defined(MOZZI_AUDIO_MODE)
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_PDM_VIA_SERIAL
#endif
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_EXTERNAL_CUSTOM, MOZZI_OUTPUT_PDM_VIA_I2S, MOZZI_OUTPUT_PDM_VIA_SERIAL, MOZZI_OUTPUT_I2S_DAC)

#if !defined(MOZZI_AUDIO_RATE)
#define MOZZI_AUDIO_RATE 32768
#endif

#if !defined(MOZZI_AUDIO_BITS)
#  define MOZZI_AUDIO_BITS 16
#endif

#if !defined(MOZZI_ANALOG_READ)
#  define MOZZI_ANALOG_READ MOZZI_ANALOG_READ_NONE
#endif

MOZZI_CHECK_SUPPORTED(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_NONE)
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_INPUT, MOZZI_AUDIO_INPUT_NONE)

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S, MOZZI_OUTPUT_PDM_VIA_SERIAL)
#  if !defined(PDM_RESOLUTION)
#    define MOZZI_PDM_RESOLUTION 2
#  endif
#  include "disable_stereo_on_github_workflow.h"
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_CHANNELS, 1)
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_BITS, 16)
#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC)
#  define MOZZI_PDM_RESOLUTION 1   // DO NOT CHANGE THIS VALUE! Not actually PDM coded, but this define is useful to keep code simple.
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_BITS, 16)
#endif

#define PDM_RESOLUTION 2   // 1 corresponds to 32 PDM clocks per sample, 2 corresponds to 64 PDM clocks, etc. (and at some level you're going hit the hardware limits)

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S, MOZZI_OUTPUT_I2S_DAC)
// NOTE: On ESP / output via I2S, we simply use the I2S buffer as the output
// buffer, which saves RAM, but also simplifies things a lot
// esp. since i2s output already has output rate control -> no need for a
// separate output timer
#define BYPASS_MOZZI_OUTPUT_BUFFER true
#endif

#define MOZZI__INTERNAL_ANALOG_READ_RESOLUTION 10

#endif        //  #ifndef CONFIG_CHECK_ESP8266_H
