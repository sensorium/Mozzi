/* Configuration example

This example is targetted at the AVR platform (Arduino Uno & friends), only!

Set configuration options according to the mode that was formerly known as "HIFI".
Do read up on the required hardware circuitry! */

#include "MozziConfigValues.h"  // for named option values

#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_2PIN_PWM
//#define MOZZI_AUDIO_RATE 32768  // the default, in this mode
//#define MOZZI_PWM_RATE 125000   // the default, in this mode
//#define MOZZI_AUDIO_BITS_PER_CHANNEL 2 // the default, in this mode

// should you wish to customize the output pins:
//#define AUDIO_AUDIO_PIN_1 TIMER1_A_PIN
//#define MOZZI_AUDIO_PIN_1_REGISTER OCR1A   // must also specify the hardware register responsible for this pin
//#define AUDIO_AUDIO_PIN_1_LOW TIMER1_B_PIN
//#define MOZZI_AUDIO_PIN_1_LOW_REGISTER OCR1B   // must also specify the hardware register responsible for this pin
