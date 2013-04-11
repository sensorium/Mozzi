#ifndef CONFIG_H
#define CONFIG_H


/**  @ingroup core
Select STANDARD or HIFI audio output mode in the Mozzi/mozzi_config.h file with either
\#define AUDIO_MODE STANDARD
or 
\#define AUDIO_MODE HIFI
In Mozzi/config.h, comment one of these options in and the other out to set the audio mode.
*/

#define AUDIO_MODE STANDARD
//#define AUDIO_MODE HIFI

/** @ingroup core
Put \#define USE_AUDIO_INPUT true in Mozzi/mozzi_config.h to enable audio input on analog pin 0,
otherwise make it false, to save resources.  When audio input is used, don't read 
other analog channels as they'll interfere with the audio sampling.
*/
#define USE_AUDIO_INPUT  false

/** @ingroup core
CONTROL_RATE is \#defined as 64 by default.  You can change it in Mozzi/mozzi_config.h or in your sketch.
*/
#define CONTROL_RATE 64

#endif        //  #ifndef CONFIG_H

