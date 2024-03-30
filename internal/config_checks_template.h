/** NOTE Template only: Copy and adjust this file when adding a new port.
 *
 * More instructions on the process are to be found in MozziGuts_impl_template.hpp. Read those first!
 *
 * What this file shall do:
 * 1) Set default values for certain configurable options. Try to stick to the defines already in use as much as possible,
 *    so configuration is consistent across ports.
 * 2) Have some checks for configuration values that are not supported on this port
 * 3) Document some details of your port
 */

/*
 * config_checks_template.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2023-2024 Thomas Friedrichsmeier and the Mozzi Team
 * NOTE: In your port, don't forget to update the above copyright notice!
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

/** NOTE: If your port doesn't support MOZZI_OUTPUT_2PIN_PWM, add this include to make compilation of HIFI examples pass on the github runner */
#include "disable_2pinmode_on_github_workflow.h"
/** NOTE: All ports need to provide a default for this */
#if not defined(MOZZI_AUDIO_MODE)
#  define MOZZI_AUDIO_MODE MOZZI_OUTPUT_PWM
#endif
/** NOTE: And all ports should allow only supported output modes. Note that MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM
are usually trivial to implement, so we'll assume your port allows them, too: */
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_EXTERNAL_CUSTOM)

/** NOTE: All ports need to provide a default for this. 32768 is reasonable on most modern MCUs, but 16384 may be useful for weaker MCUs. */
#if not defined(MOZZI_AUDIO_RATE)
#  define MOZZI_AUDIO_RATE 32768
#endif

/** NOTE: *If* your port has an implementation for asynchronous analog reads, these shall be enabled by default. */
#if not defined(MOZZI_ANALOG_READ)
#  define MOZZI_ANALOG_READ MOZZI_ANALOG_READ_STANDARD
#endif
/** NOTE: *Otherwise* you may additionally document that as not-yet-supported like so: */
// MOZZI_CHECK_SUPPORTED(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ)

/** NOTE: This should be set to whichever resolution (in bits) mozziAnalogRead() returns values in by default in your implementation.
 * mozziAnalogRead<bits>() will shift the return value accordingly. Generally you will set this to the default hardware resolution for this platform.
 *
 * @em Optionally, you could to set this to the user configurable MOZZI_ANALOG_READ_RESOLUTION (if it has been defined), and configure your ADC reads,
 * accordingly, avoiding the extra shift operation:
 *
 * @code
#ifdef MOZZI_ANALOG_READ_RESOLUTION
#define MOZZI__INTERNAL_ANALOG_READ_RESOLUTION MOZZI_ANALOG_READ_RESOLUTION
#define MOZZI__IMPL_SET_ANALOG_READ_RESOLUTION
#else
#define MOZZI__INTERNAL_ANALOG_READ_RESOLUTION 16
#endif

[...]

//inside MozziGuts_impl_MPLATFORM, function setupMozziADC():
#ifdef MOZZI__IMPL_SET_ANALOG_READ_RESOLUTION
analogReadResolution(MOZZI_ANALOG_READ_RESOLUTION);
#endif
 * @endcode
*/
#define MOZZI__INTERNAL_ANALOG_READ_RESOLUTION 10

/** NOTE: Example for additional config options depending on a specific output mode: */
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_INTERNAL_DAC)
#  if !defined(MOZZI_AUDIO_PIN_1)
#    define MOZZI_AUDIO_PIN_1 5
#  endif
/** NOTE: All ports need to provide a default for this, for all audio modes _except_ for the external ones: */
#  if !defined(MOZZI_AUDIO_BITS)
#    define MOZZI_AUDIO_BITS 10
#  endif
/** NOTE: If only mono is supported in this output mode: */
#  include "disable_stereo_on_github_workflow.h"   // This allows stereo sketches to compile (in mono) in automated testing builds.
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_CHANNELS, MOZZI_MONO)
#endif


/** NOTE: You may also set further, implementation-specific defines, here. E.g. BYPASS_MOZZI_OUTPUT_BUFFER. Defines that are purely
 * specific to your port (i.e. only needed inside MozziGuts_impt_YOURPLATFORM.h, should be #undef'ed at the end of that file. */


/** NOTE: The following is an example documentation for your port. In order not to end up in the real documentation, the comment starts
 * with "/*", only. Change that to "/**" to enable documentation to be extracted. */
 
/* @ingroup hardware
 * @page hardware_MYPORT Mozzi on MYPORT architecture based boards (e.g. MOST PROMINENT BOARD)
 *
 * Port added by YOUR_NAME.
 *
 * @section MYPORT_status Status of this port and usage notes
 * This port has been tested on BOARD A, BOARD B, but is expected to work on other boards using this family of MCUs, too.
 * The default MOZZI_AUDIO_RATE is set to 32678 Hz. Asynchronous analog reads are not yet implemented, so be careful
 * not to use mozziAnalogRead() too much. Also @ref MOZZI_AUDIO_INPUT is not available.
 *
 * When connecting external circuitry, keep in mind that the GPIO pins on this CPU are rated at a max of 3 mA. Be careful
 * not to over-stress them. It is further recommended not to make use of feature X when using Mozzi, as it will
 * introduce considerable noise into the audio.
 *
 * Also, something else to keep in mind about this port.
 *
 * @section MYPORT_output_modes Output modes
 * The following audio modes (see @ref MOZZI_AUDIO_MODE) are currently supported on this hardware:
 *   - MOZZI_OUTPUT_EXTERNAL_TIMED
 *   - MOZZI_OUTPUT_EXTERNAL_CUSTOM
 *   - MOZZI_OUTPUT_INTERNAL_DAC
 *
 * The default mode is @ref MYPORT_internal_dac , meaning, only boards with an inbuilt DAC are covered by default
 * (you could stil use one of the external output modes, however).
 *
 * @section MYPORT_internal_dac MOZZI_OUTPUT_INTERNAL_DAC
 * Output resolution is 10 bits by default, and goes to pin DAC0. Hardware timer 1 is claimed by Mozzi.
 * Only mono output is supported. Within the hardware limits of your board, you can configure the following:
 *
 * @code
 * #define MOZZI_AUDIO_PIN_1   ...  // default is DAC0
 * #define MOZZI_AUDIO_BITS    ...  // default is 10
 * @endcode
 *
 * @section MYPORT_external MOZZI_OUTPUT_EXTERNAL_TIMED and MOZZI_OUTPUT_EXTERNAL_CUSTOM
 * See @ref external_audio
*/
