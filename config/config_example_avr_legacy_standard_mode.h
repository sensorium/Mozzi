/* Configuration example

This example is targetted at the AVR platform (Arduino Uno & friends), only!

Set configuration options according to the mode that was formerly known as "STANDARD" (not STANDARD_PLUS). */

#include "MozziConfigValues.h"  // for named option values

#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_MODE_PWM
#define MOZZI_AUDIO_RATE 16384
#define MOZZI_PWM_RATE 16384

// should you wish to customize the output pin:
//#define AUDIO_AUDIO_PIN_1 TIMER1_A_PIN
//#define MOZZI_AUDIO_PIN_1_REGISTER OCR1A   // must also specify the hardware register responsible for this pin
