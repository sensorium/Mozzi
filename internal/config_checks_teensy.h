#ifndef CONFIG_CHECK_TEENSY_H
#define CONFIG_CHECK_TEENSY_H

#if !(IS_TEENSY3() || IS_TEENSY4())
#error This header should be included for Teensy (3.x or 4.x) boards, only
#endif

#if IS_TEENSY3()
#  if !defined(MOZZI_AUDIO_MODE)
#    define MOZZI_AUDIO_MODE MOZZI_OUTPUT_INTERNAL_DAC
#  endif
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_EXTERNAL_CUSTOM, MOZZI_OUTPUT_INTERNAL_DAC)
#elif IS_TEENSY4()
#  if !defined(MOZZI_AUDIO_MODE)
#    define MOZZI_AUDIO_MODE MOZZI_OUTPUT_PWM
#  endif
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_EXTERNAL_CUSTOM, MOZZI_OUTPUT_PWM)
#endif

#if !defined(MOZZI_AUDIO_RATE)
#define MOZZI_AUDIO_RATE 32768
#endif

#if defined(MOZZI_PWM_RATE)
#error Configuration of MOZZI_PWM_RATE is not currently supported on this platform (always same as AUDIO_RATE)
#endif

#if !defined(MOZZI_ANALOG_READ)
#  define MOZZI_ANALOG_READ MOZZI_ANALOG_READ_STANDARD
#endif

MOZZI_CHECK_SUPPORTED(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_NONE, MOZZI_ANALOG_READ_STANDARD)

#include "teensyPinMap.h"

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
#  define MOZZI_AUDIO_BITS 12   // not configurable
#  if !defined(MOZZI_AUDIO_PIN_1)
#    if defined(__MKL26Z64__)
#      define MOZZI_AUDIO_PIN_1 A12
#    elif defined(__MK20DX128__) || defined(__MK20DX256__)
#      define MOZZI_AUDIO_PIN_1 A14
#    elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
#      define MOZZI_AUDIO_PIN_1 A21
#    else
#      error DAC pin not know for this board. Please define MOZZI_AUDIO_PIN_1 as appropriate
#    endif
#  endif
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_CHANNELS, 1)
#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM)
#  define MOZZI_AUDIO_BITS 10   // not configurable
#  if !defined(MOZZI_AUDIO_PIN_1)
#    define MOZZI_AUDIO_PIN_1 A8
#  endif
#  if !defined(MOZZI_AUDIO_PIN_2)
#    define MOZZI_AUDIO_PIN_2 A9
#  endif
#endif

#endif        //  #ifndef CONFIG_CHECK_TEENSY_H
