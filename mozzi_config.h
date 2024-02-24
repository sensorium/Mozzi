#ifndef MOZZI_CONFIG_H
#define MOZZI_CONFIG_H
#include "hardware_defines.h"

/*
Edit this file if you want to choose your own configuration options.
*/


/**  @ingroup core
AUDIO_MODE holds the audio mode setting.
Select STANDARD (deprecated), STANDARD_PLUS or HIFI audio output mode in the Mozzi/mozzi_config.h file with
\#define AUDIO_MODE STANDARD_PLUS or \#define AUDIO_MODE HIFI.
In Mozzi/config.h, comment one of these options in and the others out to set the audio mode.

In STANDARD_PLUS mode the sample resolution is 488,
which provides some headroom above the 8 bit table resolution currently used by
the oscillators. You can look at utility/TimerOne library for more info about how
interrupt rate and pwm resolution relate.

HIFI audio mode enables much higher quality output by combining signals from pins 9 and 10.
For HIFI mode, edit Mozzi/mozzi_config.h to contain \#define AUDIO_MODE HIFI,
and comment out \#define AUDIO_MODE STANDARD and \#define AUDIO_MODE STANDARD_PLUS.

@note Teensy 3.* plays 12 bit audio in STANDARD or STANDARD_PLUS modes, and has no HIFI mode.
*/
//#define AUDIO_MODE STANDARD
#define AUDIO_MODE STANDARD_PLUS
//#define AUDIO_MODE HIFI


/** @ingroup core
Holds the audio rate setting.
AUDIO_RATE can be \#defined as 16384 or 32768 Hertz in Mozzi/mozzi_config.h.

Mozzi's original audio mode, now called STANDARD, uses 16384 Hz, chosen as a
compromise between the sample rate (interrupt rate) and sample bitdepth (pwm
width), which are interdependent due to the way pulse wave modulation is used to
generate the sound output.
An AUDIO_RATE of 32768 Hz works in STANDARD_PLUS and HIFI modes.
Of course, doubling the sample rate halves the amount of time available to calculate the each sample, so it
may only be useful for relatively simple sketches.  The increased frequency response can also make
unwanted artefacts of low resolution synthesis calculations more apparent, so it's not always a bonus.

Another factor which is important for Mozzi's operation is that with AUDIO_RATE
being a power of two, some internal calculations can be highly optimised for
speed.

In STANDARD and STANDARD_PLUS modes, the sample resolution is 488,
which provides some headroom above the 8 bit table resolution currently used by
the oscillators. You can look at the TimerOne library for more info about how
interrupt rate and pwm resolution relate.

HIFI audio mode enables much higher quality output by combining signals from pins 9 and 10.
For HIFI mode, edit Mozzi/mozzi_config.h to contain \#define AUDIO_MODE HIFI,
and comment out \#define AUDIO_MODE STANDARD and \#define AUDIO_MODE STANDARD_PLUS.

@todo Possible option for output to R/2R DAC circuit, like
http://blog.makezine.com/2008/05/29/makeit-protodac-shield-fo/ .
Mozzi-users list has a thread on this.
*/
#define AUDIO_RATE AUDIO_RATE_PLATFORM_DEFAULT
//#define AUDIO_RATE 16384 // default on AVR / classic Arduino
//#define AUDIO_RATE 32768 // default on most other platforms
//#define AUDIO_RATE 65536 // try on Teensy3/3.1 or other strong cpus


//#define USE_AUDIO_INPUT true

/** @ingroup core
This sets which analog input channel to use for audio input, if you have uncommented
\#define USE_AUDIO_INPUT  true
in mozz_config.h
@note You may have to call setupFastAnalogReads(FASTEST_ADC) after setupMozzi(), when using this.
*/
#define AUDIO_INPUT_PIN 0


/** @ingroup core
This sets allows to change from a single/mono audio output channel to
stereo output. To actually generate two channels, your updateAudio()-function
should return a StereoOutput(). Sketches returning a MonoOutput() in a stereo
config, or vice versa will continue to work, but will generate a warning a
compile time.

@note This option superseeds the earlier STEREO_HACK, which is still available at
      the time of this writing, but should not be used in new sketches.

@note At the time of this writing, only MONO and STEREO are supported. The value of
      MONO is 1 and the value of STEREO is 2, so future extensions are also expected
      to set this to the number of available channels. */
#define AUDIO_CHANNELS MONO
//#define AUDIO_CHANNELS STEREO

/** @ingroup core
Defining this option as true in mozzi_config.h allows to completely customize the audio output, e.g. for connecting to external DACs.
For more detail, @see AudioOuput .
*/
#define EXTERNAL_AUDIO_OUTPUT false
//#define EXTERNAL_AUDIO_OUTPUT true

/** @ingroup core
Only used when EXTERNAL_AUDIO_OUTPUT is set to true: The resolution to use for audio samples, internally. You will usually set this to match the
output resolution of your DAC. 16 is the default value, here. Note that 16 bits is also the maximum currently supported on AVR. */
//#define EXTERNAL_AUDIO_BITS 16

#endif        //  #ifndef MOZZI_CONFIG_H
