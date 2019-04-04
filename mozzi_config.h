#ifndef MOZZI_CONFIG_H
#define MOZZI_CONFIG_H

/*
Edit this file if you want to choose your own configuration options.
*/


/**  @ingroup core
AUDIO_MODE holds the audio mode setting.
Select STANDARD (depreciated), STANDARD_PLUS or HIFI audio output mode in the Mozzi/mozzi_config.h file with
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
#define AUDIO_MODE STANDARD
//#define AUDIO_MODE STANDARD_PLUS
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
#define AUDIO_RATE 16384
//#define AUDIO_RATE 32768
//#define AUDIO_RATE 65536 // try on Teensy3/3.1


/** @ingroup core
Whether or not to use audio input.
Put \#define USE_AUDIO_INPUT false in Mozzi/mozzi_config.h to enable audio input on analog pin AUDIO_INPUT_PIN,
otherwise make it false, to save resources.
*/
#define USE_AUDIO_INPUT false



/** @ingroup core
This sets which analog input channel to use for audio input, if you have
\#define USE_AUDIO_INPUT  true
in mozz_config.h
*/
#define AUDIO_INPUT_PIN 0

//AUDIO_INPUT_CHANNEL = analogPinToChannel(AUDIO_INPUT_PIN)

/** @ingroup core
This sets an option for stereo output, a hack which requires
variables audio_signal_1 and audio_signal_2 to be set in updateAudio(),
instead of returning a single audio value as is usual for standard mono.
You need to have \#define STEREO_HACK true in mozzi_config.h
*/
#define STEREO_HACK false


/**
  Uncomment this line if you want to use Mozzi with an external DAC of type MCP4922. Note that you will need the MCP49XX library (see AudioConfigMCP4922 for details.
*/
//#define EXTERNAL_DAC

#ifdef EXTERNAL_DAC
#warning Mozzi is configured for using an external DAC, edit mozzi_config.h if you want to change this
#endif


#endif        //  #ifndef MOZZI_CONFIG_H
