#ifndef CONFIG_H
#define CONFIG_H

// this tells doxygen to document the #defines in this file
/** @file */

/**  @ingroup core
Select STANDARD or HIFI audio output mode in the Mozzi/config.h file with either
\#define AUDIO_MODE STANDARD
or 
\#define AUDIO_MODE HIFI
In Mozzi/config.h, comment one of these options in and the other out to set the audio mode.
*/

#define AUDIO_MODE STANDARD
//#define AUDIO_MODE HIFI

/** @ingroup core
Put \#define USE_AUDIO_INPUT true in mozzi_config.h to enable audio input on analog pin 0,
otherwise make it false, to save resources.  When audio input is used, don't read 
other analog channels as they'll interfere with the audio sampling.
*/
#define USE_AUDIO_INPUT  false

#endif        //  #ifndef CONFIG_H

