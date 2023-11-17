#ifndef CONFIG_CHECK_MBED_H
#define CONFIG_CHECK_MBED_H

#if not IS_MBED()
#error This header should be included for MBED architecture, only
#endif

#if !defined(MOZZI_AUDIO_MODE)
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_INTERNAL_DAC
#endif
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_EXTERNAL_CUSTOM, MOZZI_OUTPUT_PDM_VIA_SERIAL, MOZZI_OUTPUT_INTERNAL_DAC)

#if !defined(MOZZI_AUDIO_RATE)
#define MOZZI_AUDIO_RATE 32768
#endif

#if defined(MOZZI_PWM_RATE)
#error Configuration of MOZZI_PWM_RATE is not currently supported on this platform (always same as AUDIO_RATE)
#endif

#if !defined(MOZZI_ANALOG_READ)
#  define MOZZI_ANALOG_READ MOZZI_ANALOG_READ_NONE
#endif

// yes, correct: async "single" reads are not implemented, but audio input is
MOZZI_CHECK_SUPPORTED(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_NONE)
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_INPUT, MOZZI_AUDIO_INPUT_NONE, MOZZI_AUDIO_INPUT_STANDARD)

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
#  if !defined(MOZZI_AUDIO_BITS)
#    define MOZZI_AUDIO_BITS 12
#  endif
#  if !defined(MOZZI_AUDIO_PIN_1)
#    define MOZZI_AUDIO_PIN_1 A13
#  endif
#  if !defined(MOZZI_AUDIO_PIN_2)
#    define MOZZI_AUDIO_PIN_2 A12
#  endif
#endif

#if !defined(MOZZI_AUDIO_BITS)
#  define MOZZI_AUDIO_BITS 16
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_SERIAL)
#  if !defined(MOZZI_PDM_RESOLUTION)
#    define MOZZI_PDM_RESOLUTION 2
#  endif
#  if !defined(MOZZI_SERIAL_PIN_TX)
#    define MOZZI_SERIAL_PIN_TX SERIAL2_TX
#  endif
#  if !defined(MOZZI_SERIAL_PIN_RX)
#    define MOZZI_SERIAL_PIN_RX SERIAL2_RX
#  endif
#endif

// All modes besides timed external bypass the output buffer!
#if !MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED)
#  define BYPASS_MOZZI_OUTPUT_BUFFER true
#endif

#endif        //  #ifndef CONFIG_CHECK_MBED_H
