/*! @ingroup core 
 * @file MozziConfigExample.h
 *
 * @brief Mozzi Configuration
 *
 * @note
 * It is generally safe to leave the Mozzi Configuration unchanged, and that's very much recommended _until_ you have a very specific need to customize something.
 *
 * Usage:
 *   - either:
 *      1. Copy this file to your sketch directory
 *      2. Adjust as desired, uncommenting only those options that you intend to change from their default value
 *      3. #include it in your sketch **before** #include <MozziGuts.h>
 *         (must be included in each .cpp-file using Mozzi inside your sketch, if more than one)
 *   - or:
 *      1. Copy only the sections you want to customize
 *      2. Put them near the top of your sketch **before** #include <MozziGuts.h>
 *         (should your sketch have more than one .cpp-file, identical options need to be set before each #include <MozziGuts.h>)
 * 
 * TODO: Fix and complete Doxygen coverage
 * TODO: Probably the recommendation to copy this whole file is over the top, perhaps provide stripped-down examples, instead
 * TODO: Move all the hardware depended stuff to the corresponding config_checks_xyz.h, to increase the chances that doc and implementation remain in sync
*/

#include <MozziConfigValues.h>  // needed for the named option values

/** @ingroup core
 * @def MOZZI_COMPATIBILITY_LEVEL
 *
 * Mozzi generally tries to keep your old sketches working, but we continue to find new (and hopefully better) approaches to old problems.
 * Sometimes, keeping API compatibilty with the pre-existing solution may come with a smaller or larger penalty in terms of performance or code size.
 * Therefore - if your sketch supports it - you may be able to get some minor benefit from disabling compatibility code.
 *
 * Currently supported values are:
 *   - MOZZI_COMPATIBILITY_1_1 - try to support sketches written for Mozzi version 1.1 (or possibly lower); this is the default when including MozziGuts.h
 *   - MOZZI_COMPATIBILITY_2_0 - try to support sketches written for Mozzi version 2.0; this is - currently - the default when including Mozzi.h
 *   - MOZZI_COMPATIBILITY_LATEST - always live on the bleeding edge
 *
 * @note
 * MOZZI_COMPATIBILITY_V1_1 does not guarantee, that *everything* from Mozzi 1.1 will continue to work, just that we're doing a reasonable effort.
*/
//#define MOZZI_COMPATIBILITY_LEVEL MOZZI_COMPATIBILITY_LATEST

/** @ingroup core
 * @def MOZZI_AUDIO_MODE
 *
 * @brief Configure how Mozzi outputs generated sounds.
 *
 * @note
 * Not all options are available on all platforms, and several options require specific wiring or external components to function on top of this!
 * When customizing this, it is highly recommended to start experimenting with a simple and known-to-work sketch (such as a basic sinewave) to verify that your
 * hardware setup is correct. Similarly, if you observe problems running your "real" sketch, it is often a good idea ot test your sketch with the default audio mode,
 * too (by leaving this option, and preferrably all others, unset).
 *
 * TODO: link to specific documentation for each option/platform, here, and/or spell out, what is supported, where
 *
 * Supported values:
 *   - MOZZI_OUTPUT_PWM Output using pulse width modulation (PWM) on a GPIO pin. This is the default on most platforms.
 *                      On the Arduino Uno (more generally ATMEGA328P), this allows for a sample resolution of 488 (almost 9 bits) on pin 9.
 *                      Usable pins and resolution will be different on other boards.
 *   - MOZZI_OUTPUT_2PIN_PWM Output using pulse width modulation on two GPIO pins, where one pin represents the lower bits, and the other the higer bits of the sample.
 *                      On the Aduino Uno, this allows for 14 bits of resolution on pins 9 (low) and 10 (high). For further information (wiring etc.) see @ref hardware_avr_2pin.
 *   - MOZZI_OUTPUT_EXTERNAL_TIMED Output is not controlled by Mozzi itself, but left to the user sketch. This setting allows to completely customize the audio output, e.g.
 *                      for connecting to external DACs. For more detail, see @ref external_audio
 *   - MOZZI_OUTPUT_EXTERNAL_CUSTOM As above, but additionally bypassing Mozzi's sample buffer. For more detail, see @ref external_audio
 *   - MOZZI_OUTPUT_PDM_VIA_I2S Output pulse density modulated (PDM) samples via a (hardware) I2S interface (without a DAC connected to it).
 *   - MOZZI_OUTPUT_PDM_VIA_SERIAL Output pulse density modulated (PDM) samples via a hardware serial interface.
 *   - MOZZI_OUTPUT_I2S_DAC Output samples to a PT8211 (or compatible) DAC connected to a hardware I2S interface.
 *   - MOZZI_OUTPUT_INTERNAL_DAC Output to the interal DAC on boards that support one.
 *
 * TODO: Adding an R2R-DAC option would be cool,  http://blog.makezine.com/2008/05/29/makeit-protodac-shield-fo/ , some discussion on Mozzi-users.
*/
//#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_PWM


/** @ingroup core
 * @def MOZZI_AUDIO_CHANNELS
 *
 * This sets allows to change from a single/mono audio output channel to
 * stereo output. To actually generate two channels, your updateAudio()-function
 * should return a StereoOutput(). Sketches returning a MonoOutput() in a stereo
 * config, or vice versa will continue to work, but will generate a warning a
 * compile time.
 *
 * @note This option superseeds the earlier STEREO_HACK, which is still available at
 *       the time of this writing, but should not be used in new sketches.
 *
 * @note At the time of this writing, only MOZZI_MONO and MOZZI_STEREO are supported. The value of
 *       MOZZI_MONO is 1 and the value of MOZZI_STEREO is 2, so future extensions are also expected
 *       to set this to the number of available channels, and it's ok to use numerical comparison. */
//#define MOZZI_AUDIO_CHANNELS MOZZI_STEREO



/** @ingroup core
 * @def MOZZI_AUDIO_RATE
 * 
 * @brief Defines the audio rate, i.e. rate of samples output per second.
 *
 * The default rate on the classis Arduino Uno is 16384 Hz, but can be increased to 32768 Hz, subject to the caveats, detailed below. For most other platforms 32678 Hz
 * is the default, but even higher rates may be supported.
 *.
 * Increasing the rate allows for better frequency response, but generally all affects achievable sample bitdepth (especially from PWM output).
 * Also, of course, doubling the sample rate also halves the amount of time available to calculate the each sample, so it
 * may only be useful for relatively simple sketches. The increased frequency response can also make
 * unwanted artefacts of low resolution synthesis calculations more apparent, so it's not always a bonus.
 *
 * It is highly recommended to keep the audio rate a power of two (16384, 32678, 64536, etc.), as some internal calculations can be highly be optimised for speed, this way.
 *
 * @note
 * For compatibility reasons, the option AUDIO_RATE is automatically set to the same value as this option, and you will find some uses of that in old (pre Mozzi 2.0) code examples.
 * It is advised to use only MOZZI_AUDIO_RATE in new code, however.
 * TODO: Only do the above, if MozziGuts.h, rather than Mozzi.h was included?
 */
//#define MOZZI_AUDIO_RATE 32768



/** @ingroup core
 * @def MOZZI_CONTROL_RATE
 *
 * @brief Control rate setting.
 *
 * Mozzi's MOZZI_CONTROL_RATE sets how many times per second updateControl() is called.
 * MOZZI_CONTROL_RATE has a default of 64 Hz. It is useful to have MOZZI_CONTROL_RATE set at a power of 2 (such as 64,128,256 etc),
 * to have exact timing of audio and control operations. Non-power-of-2 MOZZI_CONTROL_RATE can cause glitches due to audio and control
 * events not lining up precisely.  If this happens a power of two MOZZI_CONTROL_RATE might solve it.
 *
 * Try to keep MOZZI_CONTROL_RATE low, for efficiency, though higher rates up to about 1000
 * can sometimes give smoother results, avoiding the need to interpolate
 * sensitive variables at audio rate in updateAudio().
 *
 * TODO: If a definition of CONTROL_RATE is detected, apply that with a warning.
*/
//#define MOZZI_CONTROL_RATE 256


/** @ingroup core
 * @def MOZZI_ANALOG_READ
 *
 * Whether to compile in support for non-blocking analog reads. This is enabled by default on platforms that support it, but may be
 * disabled, explicitly, to save resources, or in order to implement custom read schemes (e.g. with IO multiplexing).
 *
 * For simplicity, mozziAnalogRead() is always defined, but when MOZZI_ANALOG_READ s are disabled or unsupported, it simply relays
 * to Arduino's regular analogRead(). It is thus quite recommended _not_ to depend on mozziAnalogRead() when disabling this.
 * Also setupFastAnalogReads() continues to be defined, for your convenience, but is not called automatically. TODO: rethink that!
 *
 * As a rough estimate (your numbers may differ a bit, depending on compiler version, etc.), on an ATMEGA328P (aka Arduino Uno),
 * disabling analog reads saves 33 bytes of RAM and 340 bytes of FLASH. The performance savings are theorized to be neglegible, however.
 *
 * Currently allowed values are:
 *   - MOZZI_ANALOG_READ_NONE
 *     Disabled
 *   - MOZZI_ANALOG_READ_STANDARD
 *     Analog read implementation enabled (currently there is only the "standard" method, but future versions might allow additional choice, here).
*/
//#define MOZZI_ANALOG_READ MOZZI_ANALOG_READ_NONE


/** @ingroup core
 * @def MOZZI_AUDIO_INPUT
 *
 * Whether to enable built in audio input feature. This is not supported on all platforms, and
 * on platforms that do support it may come with a considerable performance overhead. Don't enable, unless you need this.
 *
 * Currently allowed values are:
 *   - MOZZI_AUDIO_INPUT_NONE
 *     No audio input
 *   - MOZZI_AUDIO_INPUT_STANDARD
 *     Audio input enabled (currently there is only the "standard" method, but future versions might allow additional choice, here).
 *     This mode implies that MOZZI_ANALOG_READ s are enabled and supported. You may have to call setupFastAnalogReads(FASTEST_ADC)
 *     after setupMozzi(), when using this.
 *
 * Further reading and config: @ref getAudioInput() @ref MOZZI_AUDIO_INPUT_PIN
*/
//#define MOZZI_AUDIO_INPUT MOZZI_AUDIO_INPUT_STANDARD



/** @ingroup core
 * @def MOZZI_AUDIO_INPUT_PIN
 *
 * This sets which analog input channel to use for audio input, if you have enabled MOZZI_AUDIO_INPUT, above.
 * Not all pins may be available for this, be sure to check the documentation for your platform.
*/
//#define MOZZI_AUDIO_INPUT_PIN 0




/***************************************** ADVANCED SETTTINGS --  AVR ***********************************************************
 *
 * The settings in the following section applies to the AVR architecture (classic Arduino, and further 8bit ATMEGA based boards),
 * only.
 *
 * It is generally not recommended to change anything, here, unless you really know, what you are doing and/or
 * are willing to rough it out by yourself.
 *
********************************************************************************************************************************/


/** @ingroup hardware
 * @page hardware_avr Mozzi on classic Arduino, Teensy (2.x and 3.x), Arduino Mega, and other 8 bit "AVR"/ATMEGA architecture boards
 *
 * The following audio modes (see @ref MOZZI_AUDIO_MODE) are currently supported on this hardware. In all cases, Timer 1 is claimed, and
 * is not available for any other purpose:
 *   - MOZZI_OUTPUT_PWM
 *   - MOZZI_OUTPUT_2PIN_PWM
 *   - MOZZI_OUTPUT_EXTERNAL_TIMED
 *   - MOZZI_OUTPUT_EXTERNAL_CUSTOM
 *
 * In all cases, except MOZZI_OUTPUT_EXTERNAL_CUSTOM, Timer 1 is claimed, and is not available for any other purpose.
 *
 * @section avr_pwm MOZZI_OUTPUT_PWM
 * For MOZZI_OUTPUT_PWM, output is restricted to pins that are hardware-attached to Timer 1, but can be configured
 * within this tight limit. In particular, the default setup on the
 * Arduino UNO is:
 *    - Mono: Pin 9 -> configurable using MOZZI_AUDIO_PIN_1
 *    - Stereo: Pin 9 (left) and Pin 10 (right)  -> configurable using MOZZI_AUDIO_PIN_1 and MOZZI_AUDIO_PIN_2
 * For pinouts on other boards, refer to config/known_16bit_timers.
 *
 * Rate of the PWM output can be controlled separately from MOZZI_AUDIO_RATE: MOZZI_PWM_RATE.
 *
 * The available sample resolution is 488, i.e. almost 9 bits, providing some headroom above the 8 bit table resolution
 * currently used by the oscillators. You can look at the TimerOne library for more info about how interrupt rate and pwm resolution relate.
 *
 * @section avr_2pin_pwm  MOZZI_OUTPUT_2PIN_PWM
 * In this mode, output is split across two pins (again, both connected to Timer 1), with each outputting 7 bits for a total of 14. This allows for
 * much better dynamic range, providing much more definition than can be achieved using @ref avr_pwm , while using
 * only modestly more processing power. Further, it allows for a much higher PWM carrier rate can be much higher (125 kHz by default; see @ref
 * MOZZI_PWM_RATE), which is well beyond the audible range.
 *
 * On the downside, this mode requires an extra hardware timer (Timer2 in addition to Timer1), which may increase compatibility
 * problems with other Arduino libraries that also require a timer. Further, a more elaborate hardware setup is needed, making it less convenient
 * for rapid prototyping:
 *
 * In hardware, add the two signals through a 3.9k resistor on the "high" pin and 499k resistor on "low" pin.
 * Use 0.5% resistors or select the most accurate from a batch. It is further recommended to place a 4.7nF capacitor between the summing junction
 * of the resistors and ground. This is discussed in much more detail on http://www.openmusiclabs.com/learning/digital/pwm-dac/dual-pwm-circuits/
 * Also, there are higher quality output circuits are on the site.
 *
 * On the classic Arduino Uno, the default pinout in this mode is:
 *    - Pin 9 (high bits) and Pin 10 (low bits) -> configurable using MOZZI_AUDIO_PIN_1 and MOZZI_AUDIO_PIN_1_LOW
 *
 * Here is table of the default pins on some other boards. Rows with an x on it have actually been tested (and importantly, the outcome recoreded;
 * input welcome on further boards.)
 *
 * resistor.....3.9k......499k \n
 * x................9..........10...............Arduino Uno \n
 * x................9..........10...............Arduino Duemilanove \n
 * x................9..........10...............Arduino Nano  \n
 * x................9..........10...............Arduino Leonardo  \n
 * x................9..........10...............Ardweeny  \n
 * x................9..........10...............Boarduino  \n
 * x...............11.........12...............Freetronics EtherMega  \n
 * .................11.........12...............Arduino Mega  \n
 * .................14.........15...............Teensy  \n
 * .............B5(14)...B6(15)...........Teensy2  \n
 * x...........B5(25)...B6(26)...........Teensy2++  \n
 * .................13.........12...............Sanguino  \n
 *
 * For pinouts on other AVR boards, config/known_16bit_timers might contain some hints.
 *
 * Rate of the PWM output can be controlled separately from MOZZI_AUDIO_RATE, and is much higher (125kHz), by default: MOZZI_PWM_RATE.
 * 
 * The default sample resolution is 7 bits per pin, for a total of 14 bits. This can be configured within hardware limits (@ref MOZZI_PWM_RATE) using
 * MOZZI_AUDIO_BITS_PER_CHANNEL, but beware that increasing this will require even more accuracy in our output resistors (see the linked documentation, above).
 *
 * @section avr_external MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM
 * See @ref external_audio
*/


/** @ingroup core
 * @def MOZZI_PWM_RATE
 *
 * <em>Only for MOZZI_AUDIO_MODE s MOZZI_OUTPUT_PWM and MOZZI_OUTPUT_2PIN_PWM</em>. On some platforms, the rate at which PWM signals are repeated may be higher
 * than that at with audio signals are produced (i.e. MOZZI_AUDIO_RATE). E.g. for MOZZI_OUTPUT_PWM on the classic Arduino, the pwm defaults to 32768 while the
 * audio rate defaults to 16384. The reasoning behind this is that 16384 Hz audio rate turned out to be te most useful compromise - in most casses - between
 * output quality, and available computing power. However, output at that rate produced high-frequency whine, audible to some people, which could be mitigated
 * by the higher PWM rate.
 *
 * In other words, increasing this improves the signal quality at less cost than doubling the audio rate itself. However, increasing this too far will limit the dynamic resolution of the samples that can be
 * written to the output pin(s): 2 ^ (output bits) * MOZZI_PWM_RATE cannot be higher than the CPU frequency!
*/
//#define MOZZI_PWM_RATE 65356

/** @ingroup core
 * @def MOZZI_AUDIO_BITS_PER_CHANNEL
 *
 * <em>Only for MOZZI_AUDIO_MODE MOZZI_OUTPUT_2PIN_PWM</em>. Sample resolution per channel to use in 2 pin output, given in bits (i.e. total resolution is twice as much).
 * Defaults to 7 bits per channel. Note that increasing this requires very, very well matched output resistors.
 *
 * See @ref hardware_avr for a more detailed description.
*/
//#define MOZZI_AUDIO_BITS_PER_CHANNEL 8

/** @ingroup core
 * @def MOZZI_AUDIO_PIN_1
 *
 * Only for MOZZI_AUDIO_MODE s MOZZI_OUTPUT_PWM and MOZZI_OUTPUT_2PIN_PWM: The IO pin to use as (first) audio output. This **must** be attached to Timer1.
 * When settings this, you alsso need to specify the output compare register responsible for this pin (either OCR1A or OCR1B).
 *
 * Example:
 * @code
 * #define MOZZI_AUDIO_PIN_1            TIMER1_B_PIN
 * #define MOZZI_AUDIO_PIN_1_REGISTER   OCR1B          // must also specify this, when customizing MOZZI_AUDIO_PIN_1
 * @endcode
 *
 * Equivalent definitions can be used to control the pin for the right audio channel (in stereo mode), or the low byte channel (in 2 Pin PWM mode):
 *
 * @code
 * #define MOZZI_AUDIO_PIN_2            [...]
 * #define MOZZI_AUDIO_PIN_2_REGISTER   [the matching OCR]
 * // or
 * #define MOZZI_AUDIO_PIN_1_LOW            [...]
 * #define MOZZI_AUDIO_PIN_1_LOW_REGISTER   [the matching OCR]
 * @endcode
 *
 * @see config/known_16bit_timers.h
 * */


/***************************************** ADVANCED SETTTINGS -- External audio output ******************************************
 *
 * The settings in the following section applies to MOZZI_OUTPUT_EXTERNAL_TIMED, and MOZZI_OUTPUT_EXTERNAL_CUSTOM, only. 
 *
********************************************************************************************************************************/

/** @ingroup hardware
 * @page external_audio External audio output
 *
 * <em>Only for MOZZI_AUDIO_MODE s MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM</em>. Most (all?) platforms support
 * output using an "external" function. When using this option, you will need to provide a suitable definition for audioOutput() in
 * your own sketch, yourself. Some understanding of the general Mozzi audio output architecture may be recommendable, when using this
 * mode: See @ref AudioOutput .
 *
 * In the more simple case (MOZZI_OUTPUT_EXTERNAL_TIMED), Mozzi will still take care of buffering the samples, and calling this function
 * at audio rate (hence "timed"). This generally involves use of a timer, which should be detailed in the @ref hardware details for
 * your platform.
 *
 * Should you desire even more control - perhaps because your board, or your external DAC already comes with a rate controlled DMA buffer -
 * using MOZZI_OUTPUT_EXTERNAL_CUSTOM also bypasses Mozzis sample buffer. In addition to audioOutput(), you will then need to provide
 * a definition for canBufferAudioOutput(), which will control the rate at which samples are produced. In essence, whenever
 * canBufferAudioOutput() returns true, Mozzi will call updateAudio(), and pass the produced sample to audioOutput(), unbuffered. It is
 * entirely your job to make sure that this actually happens at MOZZI_AUDIO_RATE, and / or an appropriate buffer gets used.
 *
 * One additional configuration setting is MOZZI_AUDIO_BITS, which defaults to 16 bits for this mode, but might be set higher, if your
 * hardware supports it.
*/


/** @ingroup core
 * @def MOZZI_AUDIO_BITS
 *
 * Output resolution of audio samples. In most cases you should leave this value untouched (for the defaults that get applied, see @ref hardware .
 * However, for MOZZI_AUDIO_MODE s MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM you way wish to customize the default value
 * of 16 bits.
 *
 * @note
 * At the time of this writng single audio samples are stored as "int", unconditionally. This limits MOZZI_AUDIO_BITS to a maximumg of 16 bits on
 * some 8 bit boards!
 */
//#define MOZZI_AUDIO_BITS 16




/***************************************** ADVANCED SETTTINGS -- ESP32 ***********************************************************
 *
 * The settings in the following section applies to the ESP32 architecture.
 *
 * It is generally not recommended to change anything, here, unless you really know, what you are doing and/or
 * are willing to rough it out by yourself.
 *
********************************************************************************************************************************/


/** @ingroup hardware
 * @page hardware_esp32 Mozzi on ESP32-based boards.
 *
 * The following audio modes (see @ref MOZZI_AUDIO_MODE) are currently supported on this hardware:
 *   - MOZZI_OUTPUT_EXTERNAL_TIMED
 *   - MOZZI_OUTPUT_EXTERNAL_CUSTOM
 *   - MOZZI_OUTPUT_PDM_VIA_I2S
 *   - MOZZI_OUTPUT_I2S_DAC
 *   - MOZZI_OUTPUT_INTERNAL_DAC
 *
 * The default mode is @ref esp32_internal_dac .
 *
 * @note
 * This port really does not currently come with a PWM mode!
 *
 * @section esp32_internal_dac MOZZI_OUTPUT_INTERNAL_DAC
 * The internal DAC has 8 bit resolution, and outputs to GPIO pins 25 and 26 (non-configurable). For simplicity of code, both pins are always used.
 * In a mono configuration, both pins output the same sample.
 *
 * TODO: We could really use this to hack in a 2 PIN mode!
 *
 * @note
 * The number 25 refers to "GPIO 25" or sometimes labelled "D25". Confusingly, many boards come with an additional, totally different numbering scheme on top of that.
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


/***************************************** ADVANCED SETTTINGS -- ESP8266 ***********************************************************
 *
 * The settings in the following section applies to the ESP8266 architecture.
 *
 * It is generally not recommended to change anything, here, unless you really know, what you are doing and/or
 * are willing to rough it out by yourself.
 *
********************************************************************************************************************************/


/** @ingroup hardware
 * @page hardware_esp8266 Mozzi on ESP32-based boards.
 *
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



/***************************************** ADVANCED SETTTINGS -- Arduino Giga/Portenta MBED ***********************************************************
 *
 * The settings in the following section applies to Arduino Giga/Portenta (MBED architecture)
 *
 * It is generally not recommended to change anything, here, unless you really know, what you are doing and/or
 * are willing to rough it out by yourself.
 *
********************************************************************************************************************************/


/** @ingroup hardware
 * @page hardware_mbed Mozzi on MBED-based boards (Arduino Giga / Portenta).
 *
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


/***************************************** ADVANCED SETTTINGS -- Arduino Uno R4 - Renesas ***********************************************************
 *
 * The settings in the following section applies to Arduino Uno R4 - Renesas
 *
 * It is generally not recommended to change anything, here, unless you really know, what you are doing and/or
 * are willing to rough it out by yourself.
 *
********************************************************************************************************************************/


/** @ingroup hardware
 * @page hardware_renesas Mozzi on Arduino Uno R4 - Renesas.
 *
 * The following audio modes (see @ref MOZZI_AUDIO_MODE) are currently supported on this hardware:
 *   - MOZZI_OUTPUT_EXTERNAL_TIMED
 *   - MOZZI_OUTPUT_EXTERNAL_CUSTOM
 *   - MOZZI_OUTPUT_INTERNAL_DAC
 *
 * The default mode is @ref renesas_internal_dac. Further modes may be added in the future.
  - Because this board has an on-board DAC (A0), but only one, STEREO is not implemented and Mozzi uses this pin. Usage of other pins using PWM for instance is not implemented yet.
  - Two timers are claimed by Mozzi when using the on-board DAC, one when using `EXTERNAL_AUDIO_OUTPUT`.
  - `mozziAnalogRead()` returns values in the Renesas' full ADC resolution of 0-16384 rather than AVR's 0-1023. *This might change in the near future for speed purposes.*

 * @section renesas_internal_dac MOZZI_OUTPUT_INTERNAL_DAC
 * This uses the inbuild DAC on the board on pin A0. Mono output only, and the pin is not configurable. Audio resolution is also fixed at 12 bits (which is what the board supports).
 *
 * This mode claims two timers (but it is not hardcoded, which ones).
 *
 * @section renesas_external MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM
 * MOZZI_OUTPUT_EXTERNAL_TIMED claimes one timer, MOZZI_OUTPUT_EXTERNAL_CUSTOM does not claim any timer.
 * See @ref external_audio
*/




/***************************************** ADVANCED SETTTINGS -- RP2040 (Raspberry Pi Pico) ***********************************************************
 *
 * The settings in the following section applies to RP2040 (Raspberry Pi Pico)
 *
 * It is generally not recommended to change anything, here, unless you really know, what you are doing and/or
 * are willing to rough it out by yourself.
 *
********************************************************************************************************************************/


/** @ingroup hardware
 * @page hardware_rp2040 Mozzi on RP2040 (Raspberry Pi Pico)
 *
 * The following audio modes (see @ref MOZZI_AUDIO_MODE) are currently supported on this hardware:
 *   - MOZZI_OUTPUT_EXTERNAL_TIMED
 *   - MOZZI_OUTPUT_EXTERNAL_CUSTOM
 *   - MOZZI_OUTPUT_PDM
 *   - MOZZI_OUTPUT_I2S_DAC
 *
 * The default mode is @ref rp2040_pdm .
 *
 * @section rp2040_pdm MOZZI_OUTPUT_PDM
 * Audio output is written to pin 0 (mono) or 0 and 1 (stereo), by default, with 11 bits of ouput resolution.
 * One hardware timer interrupt and one DMA channel are claimed (number not hardcoded), a non-exclusive handler is installed on DMA_IRQ_0.
 *
 * Configuration options:
 * @code
 * #define MOZZI_AUDIO_PIN_1   ...  // default is 0
 * #define MOZZI_AUDIO_BITS    ...  // output resolution (bits); default is 11
 * // additionally, for stereo:
 * #define MOZZI_AUDIO_PIN_2   ...  // default is 1
 * @endcode
 *
 * @section rp2040_i2s_dac MOZZI_OUTPUT_I2S_DAC
 * Output to an external DAC, connected via I2S. This uses 16 bit (per audio channel), but can be changed to 8, 16, 24 (left aligned) and 32 resolution.
 * Both plain I2S and LSBJ format (PT8211 needs this, for instance) are available. LSBJ format is used by default. The GPIO pins to use can be configured,
 * - almost - freely (see below). Two DMA channels are claimed (numbers not hardcoded), non-exclusive handlers are installed on DMA_IRQ_0.
 *
 * Configuration options:
 * @code
 * #define MOZZI_AUDIO_BITS    ...  // available values are 8, 16 (default), 24 (LEFT ALIGN in 32 bits type!!) and 32 bits
 * #define MOZZI_I2S_PIN_BCK                        ... // /BLCK) default is 20
 * //#define MOZZI_I2S_PIN_WS (MOZZI_I2S_PIN_BCK+1) ... // CANNOT BE CHANGED, HAS TO BE NEXT TO pBCLK, i.e. default is 21
 * #define MOZZI_I2S_PIN_DATA                       ... // (DOUT) default is 22
 * #define MOZZI_I2S_FORMAT                         ... // may be MOZZI_I2S_FORMAT_LSBJ (default) or MOZZI_I2S_FORMAT_PLAIN
 * @endcode
 *
 * @section rp2040_external MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM
 * See @ref external_audio
*/



/***************************************** ADVANCED SETTTINGS -- SAMD21 ***********************************************************
 *
 * The settings in the following section applies to SAMD21 boards
 *
 * It is generally not recommended to change anything, here, unless you really know, what you are doing and/or
 * are willing to rough it out by yourself.
 *
********************************************************************************************************************************/

/** @ingroup hardware
 * @page hardware_samd Mozzi on SAMD21 based boards (Arduino Circuitplayground M0 and others)
 *
 * The following audio modes (see @ref MOZZI_AUDIO_MODE) are currently supported on this hardware:
 *   - MOZZI_OUTPUT_EXTERNAL_TIMED
 *   - MOZZI_OUTPUT_EXTERNAL_CUSTOM
 *   - MOZZI_OUTPUT_INTERNAL_DAC
 *
 * The default mode is @ref samd_internal_dac , meaning, only boards with an inbuilt DAC are covered by default
 * (you could stil use one of the external output modes, however).
 *
 * @section samd_internal_dac MOZZI_OUTPUT_INTERNAL_DAC
 * Output resolution is 10 bits by default, and goes to pin DAC0. Only mono output is supported. Within the hardware limits of your board, you can configure the following:
 *
 * @code
 * #define MOZZI_AUDIO_PIN_1   ...  // default is DAC0
 * #define MOZZI_AUDIO_BITS    ...  // default is 10
 * @endcode
 *
 * @section samd_external MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM
 * See @ref external_audio
*/




/***************************************** ADVANCED SETTTINGS -- STM32(duino) ***********************************************************
 *
 * The settings in the following section applies to the STM32(duino) architecture.
 *
 * It is generally not recommended to change anything, here, unless you really know, what you are doing and/or
 * are willing to rough it out by yourself.
 *
********************************************************************************************************************************/

/** @ingroup hardware
* @page hardware_stm32_disambiguation Mozzi on STM32-based boards - disambiguation
*
*  * The situation on STM32-based boards is rather confusing, as there are several competing Arduino cores. Importantly:
*    - Some boards use dedicated cores (e.g. Arduino Giga / Portenta @ref hardware_mbed) etc. For those, see the relevant sections (if we support them).
*    - There is a series of libmaple-based cores, including [Roger Clark's libmaple-based core](https://github.com/rogerclarkmelbourne/Arduino_STM32). These are highly optimized,
*      and provide very complete support, but only for a limited number of boards. Unfortunately, at the time of this writing (2023/04), they are not available for installation
*      via the Arduino Board Manager, and they do not currently seem actively maintained.
*      For using these with Mozzi, see @ref hardware_stm32_maple
*    - A generic Arduino core for STM32 is the [STM32duino core](https://github.com/stm32duino/Arduino_Core_STM32). It supports a huge step of boards, and seems to have offical
*      backing by STM, but some features of the libmaple based cores are still lacking. To complete confusion, this core now uses the label "STM32duino", which used to be what
*      the libmaple cores above were known by (don't blame Mozzi for this mess!).
*      For using this with Mozzi, see @ref hardware_stm32duino
*
* */

/** @ingroup hardware
 * @page hardware_stm32duino Mozzi on STM32duino-based boards.
 *
 * @note
 * Be sure to understand the info given at @ref hardwware_stm32_disambiguation . This page is about using Mozzi with the STM32duino core.
 *
 * The following audio modes (see @ref MOZZI_AUDIO_MODE) are currently supported on this hardware:
 *   - MOZZI_OUTPUT_EXTERNAL_TIMED
 *   - MOZZI_OUTPUT_EXTERNAL_CUSTOM
 *   - MOZZI_OUTPUT_PWM
 *   - MOZZI_OUTPUT_PWM_2PIN
 *
 * The default mode is @ref stm32duino_pwm .
 *
 * @note
 * This port may look similar to, but uses a different default GPIO pinout than @hardware_stm32_maple !
 *
 * @section stm32duino_pwm MOZZI_OUTPUT_PWM
 * Standard pulse width modulated output to one (mono) or two (stereo, see @ref MOZZI_AUDIO_CHANNELS) GPIO pins. Default pinout: PA8 (mono/left), PA9 (right channel in stereo).
 * This mode uses two hardware timers: One for the PWM (Timer 3 when using the default pin configuration), and a second for updating the output at audio rate.
 * The default audio resolution is 10 bits, which results in a carrier frequency of ~70kHz on a 72MHz CPU. On slower boards you will have to descrease this.
 * The following settings may be costumized, if desired:
 *
 * @code
 * #define MOZZI_AUDIO_PIN_1       ...  // Left / mono output pin. Default: PA8
 * #define MOZZI_AUDIO_UPDATE_TIMER ... // Second hardware timer to claim, must not be the same of the timer for the above pin. Default TIM2
 * #define MOZZI_AUDIO_BITS        ...  // Output resolution in bits. Default is 10
 * // For stereo, only:
 * #define MOZZI_AUDIO_PIN_2       ...  // Right channel output pin. This *must* be connected to the same hardware timer as MOZZI_AUDIO_PIN_1 ! Default: PA9
 * @endcode
 *
 * @section stm32duino_pwm MOZZI_OUTPUT_2PIN_PWM
 * This mode is very similar to @ref stm32duino_pwm, but splitting output for a single channel across two GPIO pins for better resolution. For details on the required
 * hardware setup, and configuration tradeoffs, see @ref avr_2pin_pwm . Stereo output is not available in this mode.
 *
 * Customizable configuration options:
 * @code
 * #define MOZZI_AUDIO_PIN_1       ...  // High byte of the output. Default: PA8
 * #define MOZZI_AUDIO_PIN_1_LOW   ...  // Low byte of the output. Default: PA9
 * #define MOZZI_AUDIO_UPDATE_TIMER ... // Second hardware timer to claim. Default TIM2
 * #define MOZZI_AUDIO_BITS_PER_CHANNEL  ...  // Bits per pin. Default is 7
 * @endcode
 *
 * @section stm32duino_external MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM
 * See @ref external_audio .
 * The (single) hardware timer claimed for MOZZI_OUTPUT_EXTERNAL_TIMED may be configured using "MOZZI_AUDIO_UPDATE_TIMER" (default: TIM2).
*/



/***************************************** ADVANCED SETTTINGS -- STM32 (libmaple-based core) ***********************************************************
 *
 * The settings in the following section applies to the STM32 (libmaple-based core) architecture.
 *
 * It is generally not recommended to change anything, here, unless you really know, what you are doing and/or
 * are willing to rough it out by yourself.
 *
********************************************************************************************************************************/

/** @ingroup hardware
 * @page hardware_stm32_maple Mozzi on STM32duino-based boards.
 *
 * @note
 * Be sure to understand the info given at @ref hardwware_stm32_disambiguation . This page is about using Mozzi with the STM32 "libmaple based" core.
 *
 * @note
 * This port may look similar to, but uses a different default GPIO pinout than @hardware_stm32duino !
 *
 * The following audio modes (see @ref MOZZI_AUDIO_MODE) are currently supported on this hardware:
 *   - MOZZI_OUTPUT_EXTERNAL_TIMED
 *   - MOZZI_OUTPUT_EXTERNAL_CUSTOM
 *   - MOZZI_OUTPUT_PWM
 *   - MOZZI_OUTPUT_PWM_2PIN
 *
 * The default mode is @ref stm32_maple_pwm .
 *
 * @section stm32_maple_pwm MOZZI_OUTPUT_PWM
 * Standard pulse width modulated output to one (mono) or two (stereo, see @ref MOZZI_AUDIO_CHANNELS) GPIO pins. Default pinout: PB8 (mono/left), PB9 (right channel in stereo).
 * This mode uses two hardware timers: One for the PWM (Timer 4 when using the default pin configuration), and a second for updating the output at audio rate.
 * The default audio resolution is 10 bits, which results in a carrier frequency of ~70kHz on a 72MHz CPU. On slower boards you will have to descrease this.
 * The following settings may be costumized, if desired:
 *
 * @code
 * #define MOZZI_AUDIO_PIN_1       ...  // Left / mono output pin. Default: PB8
 * #define MOZZI_AUDIO_PWM_TIMER   ...  // Must be set ot the hardware timer connected to the above pin. Default: 4
 * #define MOZZI_AUDIO_UPDATE_TIMER ... // Second hardware timer to claim. Default 2
 * #define MOZZI_AUDIO_BITS        ...  // Output resolution in bits. Default is 10
 * // For stereo, only:
 * #define MOZZI_AUDIO_PIN_2       ...  // Right channel output pin. This *must* be connected to the same hardware timer as MOZZI_AUDIO_PIN_1 ! Default: PB9
 * @endcode
 *
 * @section stm32_maple_pwm MOZZI_OUTPUT_2PIN_PWM
 * This mode is very similar to @ref stm32_maple_pwm, but splitting output for a single channel across two GPIO pins for better resolution. For details on the required
 * hardware setup, and configuration tradeoffs, see @ref avr_2pin_pwm . Stereo output is not available in this mode.
 *
 * Customizable configuration options:
 * @code
 * #define MOZZI_AUDIO_PIN_1       ...  // High byte of the output. Default: PB8
 * #define MOZZI_AUDIO_PIN_2       ...  // Low byte of the output. Default: PB9
 * #define MOZZI_AUDIO_PWM_TIMER   ...  // Must be set to the number of the hardware timer connect to the above pins. Default: 4
 * #define MOZZI_AUDIO_UPDATE_TIMER ... // Second hardware timer to claim. Default TIM2
 * #define MOZZI_AUDIO_BITS_PER_CHANNEL  ...  // Bits per pin. Default is 7
 * @endcode
 *
 * @section stm32_maple_external MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM
 * See @ref external_audio
 * The (single) hardware timer claimed for MOZZI_OUTPUT_EXTERNAL_TIMED may be configured using "MOZZI_AUDIO_UPDATE_TIMER" (default: TIM2).
*/


/***************************************** ADVANCED SETTTINGS -- Teensy 3.x ***********************************************************
 *
 * The settings in the following section applies to Teensy 3.x boards
 *
 * It is generally not recommended to change anything, here, unless you really know, what you are doing and/or
 * are willing to rough it out by yourself.
 *
********************************************************************************************************************************/

/** @ingroup hardware
 * @page hardware_teensy3 Mozzi on Teensy 3.0/3.1/3.2/3.4/3.5/LC boards.
 *
 * @note
 * For Teensy 4.x see @ref hardware_teensy4
 *
 * @note
 * This port requires the following two libraries (which should be part of a default installtion, however):
 * - [Timer library](https://github.com/loglow/IntervalTimer) for Teensy 3.* by Daniel Gilbert
 * - [ADC library](http://github.com/pedvide/ADC) by Pedro Villanueva
 *
 * The following audio modes (see @ref MOZZI_AUDIO_MODE) are currently supported on this hardware:
 *   - MOZZI_OUTPUT_EXTERNAL_TIMED
 *   - MOZZI_OUTPUT_EXTERNAL_CUSTOM
 *   - MOZZI_OUTPUT_INTERNAL_DAC
 *
 * The default mode is @ref teensy3_internal_dac .
 *
 * @section teensy3_internal_dac MOZZI_OUTPUT_INTERNAL_DAC
 * Output is to the inbuilt DAC. Output resolution is 12 bits. Mono, only. The DAC output pin differs from board to boards.
 * In most cases the appropriate pin will be set outmatically. If needed, you can specify the DAC pin, explicitly:
 *
 * @code
 * #define MOZZI_AUDIO_PIN_1       ...  // Default: A14, A12, or A21, depending on board
 * @endcode
 *
 * @section teensy3_external MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM
 * See @ref external_audio
*/

/***************************************** ADVANCED SETTTINGS -- Teensy 4.x ***********************************************************
 *
 * The settings in the following section applies to Teensy 4.x boards
 *
 * It is generally not recommended to change anything, here, unless you really know, what you are doing and/or
 * are willing to rough it out by yourself.
 *
********************************************************************************************************************************/

/** @ingroup hardware
 * @page hardware_teensy4 Mozzi on Teensy 3.0/3.1/3.2/3.4/3.5/LC boards.
 *
 * @note
 * For Teensy 3.x see @ref hardware_teensy3
 *
 * @note
 * This port requires the following two libraries (which should be part of a default installtion, however):
 * - [Timer library](https://github.com/loglow/IntervalTimer) for Teensy 4.* by Daniel Gilbert
 * - [ADC library](http://github.com/pedvide/ADC) by Pedro Villanueva
 *
 * The following audio modes (see @ref MOZZI_AUDIO_MODE) are currently supported on this hardware:
 *   - MOZZI_OUTPUT_EXTERNAL_TIMED
 *   - MOZZI_OUTPUT_EXTERNAL_CUSTOM
 *   - MOZZI_OUTPUT_PWM
 *
 * The default mode is @ref teensy4_pwm .
 *
 * @section teensy4_pwm MOZZI_OUTPUT_PWM
 * Output is to a GPIO pin (or two in stereo). The output resolution is fixed at 10 bits, and a 146484 kHz carrier frequency.
 * The output pins can be configured as:
 *
 * @code
 * #define MOZZI_AUDIO_PIN_1       ...  // Left / mono output pin. Default: A8
 * // For stereo, only:
 * #define MOZZI_AUDIO_PIN_2       ...  // Right channel output pin. Default: A9
 * @endcode
 *
 * @section teensy4_external MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM
 * See @ref external_audio
*/
