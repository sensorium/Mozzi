#ifndef HARDWARE_DEFINES_H_
#define HARDWARE_DEFINES_H_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

/* Macros to tell apart the supported platforms. The advantages of using these are, rather than the underlying defines
- Easier to read and write
- Compiler protection against typos
- Easy to extend for new but compatible boards */

#define IS_AVR() (defined(__AVR__))  // "Classic" Arduino boards
#define IS_SAMD21() (defined(ARDUINO_ARCH_SAMD))
#define IS_TEENSY3() (defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__) || defined(__MKL26Z64__))  // 32bit arm-based Teensy
#define IS_TEENSY4() (defined(__IMXRT1062__)) // Teensy4 (no DAC)
#define IS_STM32() (defined(__arm__) && !IS_TEENSY3() && !IS_SAMD21() && !IS_TEENSY4() && !IS_RP2040())  // STM32 boards (note that only the maple based core is supported at this time. If another cores is to be supported in the future, this define should be split.
#define IS_ESP8266() (defined(ESP8266))
#define IS_ESP32() (defined(ESP32))
#define IS_RP2040() (defined(ARDUINO_ARCH_RP2040))

#if !(IS_AVR() || IS_TEENSY3() || IS_TEENSY4() || IS_STM32() || IS_ESP8266() || IS_SAMD21() || IS_ESP32() || IS_RP2040())
#error Your hardware is not supported by Mozzi or not recognized. Edit hardware_defines.h to proceed.
#endif

// Hardware detail defines
#if IS_STM32()
#define NUM_ANALOG_INPUTS 16  // probably wrong, but mostly needed to allocate an array of readings
#elif IS_ESP8266()
#define NUM_ANALOG_INPUTS 1
#endif

#if IS_AVR()
#define AUDIO_RATE_PLATFORM_DEFAULT 16384
#else
#define AUDIO_RATE_PLATFORM_DEFAULT 32768
#endif

#if IS_ESP8266()
#define CACHED_FUNCTION_ATTR ICACHE_RAM_ATTR
#elif IS_ESP32()
#define CACHED_FUNCTION_ATTR IRAM_ATTR
#else
#define CACHED_FUNCTION_ATTR
#endif

#if IS_STM32()
// This is a little silly, but with Arduino 1.8.13, including this header inside MozziGuts.cpp does not work (fails to detect the proper include path).
// Putting it here, instead, seem to work.
#include <STM32ADC.h>
#endif

#endif /* HARDWARE_DEFINES_H_ */
