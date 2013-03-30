#ifndef CONFIG_H
#define CONFIG_H


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
\#define this as true in mozzi_config.h to enable audio input on analog pin 0,
otherwise make it false, to save resources.
*/
#define USE_AUDIO_INPUT  true
//#define USE_AUDIO_INPUT  false

#endif        //  #ifndef CONFIG_H

