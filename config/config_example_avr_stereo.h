/* Configuration example

This example is targetted at the AVR platform (Arduino Uno & friends), only!

This example shows setting up stereo mode on AVR. */

#include "MozziConfigValues.h"  // for named option values

#define MOZZI_AUDIO_CHANNELS MOZZI_STEREO

// should you wish to customize the output pins:
//#define AUDIO_AUDIO_PIN_1 TIMER1_A_PIN
//#define MOZZI_AUDIO_PIN_1_REGISTER OCR1A   // must also specify the hardware register responsible for this pin
//#define AUDIO_AUDIO_PIN_2 TIMER1_B_PIN
//#define MOZZI_AUDIO_PIN_2_REGISTER OCR1B   // must also specify the hardware register responsible for this pin
