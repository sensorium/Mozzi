#ifndef HARDWARE_DEFINES_H_
#define HARDWARE_DEFINES_H_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

// Macros to tell apart the supported platforms
#define IS_AVR() (defined(__AVR__))  // "Classic" Arduino boards
#define IS_TEENSY3() (defined(__MK20DX128__) || defined(__MK20DX256__) || defined(TEENSYDUINO))  // 32bit arm-based Teensy
#define IS_STM32() (defined(__arm__) && !IS_TEENSY3)  // STM32 boards (currently libmaple-based!)

#if !(IS_AVR() || IS_TEENSY3() || IS_STM32())
#error Your hardware is not supported by Mozzi or not recognized. Edit hardware_defines.h to proceed.
#endif

#endif /* HARDWARE_DEFINES_H_ */
