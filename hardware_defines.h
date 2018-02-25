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
#define IS_TEENSY3() (defined(__MK20DX128__) || defined(__MK20DX256__) || defined(TEENSYDUINO))  // 32bit arm-based Teensy
#define IS_STM32() (defined(__arm__) && !IS_TEENSY3)  // STM32 boards (note that only the maple based core is supported at this time. If another cores is to be supported in the future, this define should be split.
#define IS_ESP8266() (defined(ESP8266))

#if !(IS_AVR() || IS_TEENSY3() || IS_STM32() || IS_ESP8266())
#error Your hardware is not supported by Mozzi or not recognized. Edit hardware_defines.h to proceed.
#endif

// Hardware detail defines
#if IS_STM32()
#define NUM_ANALOG_INPUTS 16  // probably wrong, but mostly needed to allocate an array of readings
#elif IS_ESP8266()
#define NUM_ANALOG_INPUTS 1
#endif

#if IS_ESP8266()
// TODO HACK: Just disable these for now
#define ATOMIC_BLOCK(x) if(true || x)
#define ATOMIC_RESTORESTATE true

#define PGMSPACE_INCLUDE_H <Arduino.h>  // dummy because not needed
#define CONSTTABLE_READ(x) (*(x))
#define CONSTTABLE_READ_DWORD(x) (*(x))
#define CONSTTABLE_READ_WORD(x) (*(x))
#define CONSTTABLE_STORAGE
#define ATOMIC_INCLUDE_H <Arduino.h>  // dummy because not available
#else
#define PGMSPACE_INCLUDE_H <avr/pgmspace.h>
#define CONSTTABLE_READ_DWORD(x) pgm_read_dword(x)
#define CONSTTABLE_READ_WORD(x) pgm_read_word_near(x)
#define CONSTTABLE_READ(x) pgm_read_byte_near(x)
#define CONSTABLE_STORAGE __attribute__((section(".progmem.data")))
#define ATOMIC_INCLUDE_H <util/atomic.h>
#endif

#endif /* HARDWARE_DEFINES_H_ */
