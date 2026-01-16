#ifndef MOZZI_CONFIG_CHECKS_CH32_H
#define MOZZI_CONFIG_CHECKS_CH32_H

#if !defined(MOZZI_AUDIO_MODE)
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_PWM
#endif

// Configure Audio Bits based on Mode
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM)
    #if !defined(MOZZI_AUDIO_BITS)
    #define MOZZI_AUDIO_BITS 16 // Internal calc at 16, output scaled to 8
    #endif
#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_2PIN_PWM)
    #if !defined(MOZZI_AUDIO_BITS)
    #define MOZZI_AUDIO_BITS 16 // Full 16-bit output
    #endif
#endif

#if !defined(MOZZI_AUDIO_RATE)
#define MOZZI_AUDIO_RATE 16384
#endif

#if !defined(MOZZI_ANALOG_READ)
#define MOZZI_ANALOG_READ MOZZI_ANALOG_READ_NONE
#endif

#define MOZZI__INTERNAL_ANALOG_READ_RESOLUTION 10

#endif