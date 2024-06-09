#ifdef FOR_DOXYGEN_ONLY
/** @file */

/*! @defgroup config Mozzi Configuration options */

/** @ingroup config
 * @page config_main Mozzi Configuration
 *
 * @section config_general Configuring Mozzi
 *
 * Mozzi configuration options include details such as audio rate, number of audio channels (mono or stereo), output generation method and many others,
 * where details on the available options differ between the different platforms (see @ref hardware), and may include additional options beyond those listed, here.
 *
 * @note
 * It is generally safe to leave the Mozzi Configuration unchanged, and that's very much recommended _until_ you have a very specific need to customize something.
 * Contrary to past versions of Mozzi, all configuration options have a (usually sensible) default value, so you do not have to configure _anything_, unless you
 * actually want to change something.
 *
 * Configuring Mozzi is mostly done using various preprocessor definitions. This approach is used to move as much of the processing involved to compile time, in order
 * to save Flash, RAM, and CPU use at runtime. This section lists various global options, but in addition, most ports allow additional hardware dependent
 * configuration options. See @ref hardware.
 *
 * Several configuration examples are provided in the "config" folder of the Mozzi sources. You may want to look at these, first. The general approach is as follows:
 *
 * @code
 * #include <MozziConfigValues.h>                      // include this first, for named option values
 * #define MOZZI_AUDIO_CHANNELS MOZZI_STEREO           // set to stereo mode
 *
 * #include <Mozzi.h>                                  // *after* all configuration options, include the main Mozzi headers
 * @endcode
 *
 * Alternatively, if a suitable configuration example exists, use:
 * @code
 * #include <config/config_example_avr_stereo.h>       // again, do this, before including the main headers
 * @endcode
 *
 * @note
 * Should you include Mozzi headers in more than one compilation unit (i.e. more than one .cpp file) inside the same sketch, you *must* use identical
 * configuration options at the top of each file!
 *
 * TODO: Fix and complete Doxygen coverage
*/

/** @ingroup config
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
#define MOZZI_COMPATIBILITY_LEVEL FOR_DOXYGEN_ONLY

/** @ingroup config
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
 * Refer to the @ref hardware specific documentation for which modes are supported on your hardware, and further details!
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
#define MOZZI_AUDIO_MODE FOR_DOXYGEN_ONLY

/** @ingroup config
 * @def MOZZI_AUDIO_CHANNELS
 *
 * This sets allows to change from a single/mono audio output channel to
 * stereo output. To actually generate two channels, your updateAudio()-function
 * should return a StereoOutput(). Sketches returning a MonoOutput() in a stereo
 * config, or vice versa will continue to work, but will generate a warning a
 * compile time.
 *
 * @note This option superseeds the earlier STEREO_HACK in Mozzi < 1.1
 *
 * @note At the time of this writing, only MOZZI_MONO and MOZZI_STEREO are supported. The value of
 *       MOZZI_MONO is 1 and the value of MOZZI_STEREO is 2, so future extensions are also expected
 *       to set this to the number of available channels, and it's ok to use numerical comparison. */
#define MOZZI_AUDIO_CHANNELS FOR_DOXYGEN_ONLY


/** @ingroup config
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
 * For compatibility reasons, the option MOZZI_AUDIO_RATE is automatically set to the same value as this option, and you will find some uses of that in old (pre Mozzi 2.0) code examples.
 * It is advised to use only MOZZI_AUDIO_RATE in new code, however.
 * TODO: Only do the above, for MOZZI_COMPATIBILITY_LEVEL < MOZZI_COMPATIBILITY_2_0?
 */
#define MOZZI_AUDIO_RATE FOR_DOXYGEN_ONLY


/** @ingroup config
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
 * TODO: If a definition of MOZZI_CONTROL_RATE is detected, apply that with a warning.
*/
#define MOZZI_CONTROL_RATE FOR_DOXYGEN_ONLY


/** @ingroup config
 * @def MOZZI_ANALOG_READ
 *
 * Whether to compile in support for non-blocking analog reads. This is enabled by default on platforms that support it, but may be
 * disabled, explicitly, to save resources, or in order to implement custom read schemes (e.g. with IO multiplexing).
 *
 * For simplicity, mozziAnalogRead() is always defined, but when MOZZI_ANALOG_READ s are disabled or unsupported, it simply relays
 * to Arduino's regular analogRead(). It is thus quite recommended _not_ to depend on mozziAnalogRead() when disabling this.
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
#define MOZZI_ANALOG_READ FOR_DOXYGEN_ONLY


/** @ingroup config
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
#define MOZZI_AUDIO_INPUT FOR_DOXYGEN_ONLY


/** @ingroup config
 * @def MOZZI_AUDIO_INPUT_PIN
 *
 * This sets which analog input channel to use for audio input, if you have enabled MOZZI_AUDIO_INPUT, above.
 * Not all pins may be available for this, be sure to check the documentation for your platform.
*/
#define MOZZI_AUDIO_INPUT_PIN FOR_DOXYGEN_ONLY


/** @ingroup config
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
#define MOZZI_PWM_RATE FOR_DOXYGEN_ONLY


/** @ingroup config
 * @def MOZZI_AUDIO_BITS_PER_CHANNEL
 *
 * <em>Only for MOZZI_AUDIO_MODE MOZZI_OUTPUT_2PIN_PWM</em>. Sample resolution per channel to use in 2 pin output, given in bits (i.e. total resolution is twice as much).
 * Defaults to 7 bits per channel. Note that increasing this requires very, very well matched output resistors.
 *
 * See @ref hardware_avr for a more detailed description.
*/
#define MOZZI_AUDIO_BITS_PER_CHANNEL FOR_DOXYGEN_ONLY


/** @ingroup config
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
#define MOZZI_AUDIO_PIN_1 FOR_DOXYGEN_ONLY


/***************************************** ADVANCED SETTTINGS -- External audio output ******************************************
 *
 * The settings in the following section applies to MOZZI_OUTPUT_EXTERNAL_TIMED, and MOZZI_OUTPUT_EXTERNAL_CUSTOM, only. 
 *
********************************************************************************************************************************/

/** @ingroup audio_output
 *
 * @page external_audio External audio output
 * @details
 * <em>Only for @ref MOZZI_AUDIO_MODE set to MOZZI_OUTPUT_EXTERNAL_TIMED or MOZZI_OUTPUT_EXTERNAL_CUSTOM</em>. Most (all?) platforms support
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
 *
 * @see config
*/


/** @ingroup config
 * @def MOZZI_AUDIO_BITS
 *
 * Output resolution of audio samples. In most cases you should leave this value untouched (for the defaults that get applied, see @ref hardware .
 * However, for MOZZI_AUDIO_MODE s MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM you way wish to customize the default value
 * of 16 bits.
 *
 * @note
 * At the time of this writng single audio samples are stored as "int", unconditionally. This limits MOZZI_AUDIO_BITS to a maximum of 16 bits on
 * some 8 bit boards!
 */
#define MOZZI_AUDIO_BITS FOR_DOXYGEN_ONLY

#endif
