/* Configuration example

This example is targetted at the AVR platform (Arduino Uno & friends), only!

The configuration formerly known as STANDARD_PLUS is still the default on AVR, so you
do not need to configure anything! This file just lists the relevant settings involved: */

#include "MozziConfigValues.h"  // for named option values

// all of these are the defaults on AVR, anyway, thus commented
//#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_MODE_PWM
//#define MOZZI_AUDIO_RATE 16384
//#define MOZZI_PWM_RATE 32768


// should you wish to customize the output pin:
//#define AUDIO_AUDIO_PIN_1 TIMER1_A_PIN
//#define MOZZI_AUDIO_PIN_1_REGISTER OCR1A   // must also specify the hardware register responsible for this pin
