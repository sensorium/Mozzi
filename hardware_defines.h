/*
 * hardware_defines.h.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2023-2024 Thomas Friedrichsmeier and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
 */


#ifndef HARDWARE_DEFINES_H_
#define HARDWARE_DEFINES_H_

#include "Arduino.h"

/* Macros to tell apart the supported platforms. The advantages of using these are, rather than the underlying defines
- Easier to read and write
- Compiler protection against typos
- Easy to extend for new but compatible boards */

// "Classic" Arduino boards
#if (defined(__AVR__))
#define IS_AVR() 1
#else
#define IS_AVR() 0
#endif

// SAMD
#if (defined(ARDUINO_ARCH_SAMD))
#define IS_SAMD21() 1
#else
#define IS_SAMD21() 0
#endif

// 32bit arm-based Teensy
#if (defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__) || defined(__MKL26Z64__))
#define IS_TEENSY3() 1
#else
#define IS_TEENSY3() 0
#endif

// Teensy4 (no DAC)
#if (defined(__IMXRT1062__))
#define IS_TEENSY4() 1
#else
#define IS_TEENSY4() 0
#endif

// RP2040 (Raspberry Pi Pico and friends)
#if (defined(ARDUINO_ARCH_RP2040))
#define IS_RP2040() 1
#else
#define IS_RP2040() 0
#endif

// STM32 boards (libmaple based)
// https://github.com/stevstrong/Arduino_STM32
#if (defined(__arm__) && !IS_TEENSY3() && !IS_TEENSY4() && __has_include("libmaple/libmaple.h"))
#define IS_STM32MAPLE() 1
#else
#define IS_STM32MAPLE() 0
#endif

// Mbed OS based boards
#if (defined(ARDUINO_ARCH_MBED))
#define IS_MBED() 1
#else
#define IS_MBED() 0
#endif

// Arduino Giga
#if (IS_MBED() && defined(ARDUINO_GIGA))
#define IS_GIGA() 1
#else
#define IS_GIGA() 0
#endif

// Arduino Uno R4 (Renesas arch)
#if (defined(ARDUINO_FSP))
#define IS_RENESAS() 1
#else
#define IS_RENESAS() 0
#endif

#if (defined(__arm__) && !IS_STM32MAPLE() && !IS_TEENSY3() && !IS_TEENSY4() && !IS_RP2040() && !IS_SAMD21() && !IS_MBED() && !IS_RENESAS())
#define IS_STM32DUINO() 1
#else
#define IS_STM32DUINO() 0
#endif

#if (defined(ESP8266))
#define IS_ESP8266() 1
#else
#define IS_ESP8266() 0
#endif

#if (defined(ESP32))
#define IS_ESP32() 1
#else
#define IS_ESP32() 0
#endif

#if !(IS_AVR() || IS_TEENSY3() || IS_TEENSY4() || IS_STM32MAPLE() || IS_STM32DUINO() || IS_ESP8266() || IS_SAMD21() || IS_ESP32() || IS_RP2040() || IS_MBED() || IS_RENESAS())
// TODO: add an exception for MOZZI_OUTPUT_EXTERNAL_CUSTOM
#error Your hardware is not supported by Mozzi or not recognized. Edit hardware_defines.h to proceed.
#endif

// Hardware detail defines
#if IS_STM32MAPLE()
#define NUM_ANALOG_INPUTS 16  // probably wrong, but mostly needed to allocate an array of readings
#elif IS_ESP8266()
#define NUM_ANALOG_INPUTS 1
#endif

#if IS_ESP8266() || IS_ESP32()
#define CACHED_FUNCTION_ATTR IRAM_ATTR
#else
#define CACHED_FUNCTION_ATTR
#endif

#if IS_STM32MAPLE()
// This is a little silly, but with Arduino 1.8.13, including this header inside MozziGuts.cpp does not work (fails to detect the proper include path).
// Putting it here, instead, seem to work.
#include <STM32ADC.h>
#endif

#endif /* HARDWARE_DEFINES_H_ */
