#ifndef AUDIOCONFIGMBED_H
#define AUDIOCONFIGMBED_H

#if not IS_MBED()
#error This header should be included for MBED OS boards, only
#endif

#if (AUDIO_MODE == HIFI)
#error HIFI mode is not available for this CPU architecture (but several high quality output options are available)
#endif

// Audio output options
#define INTERNAL_DAC 1 // output using internal DAC driven via DMA. Output is only possible on the DAC pins (A12, and A13 on the Giga)
#define TIMED_PWM 2    // BROKEN: output using PWM updated by a timer at audio rate. Less efficient, but should be most portable, and allows to use any PWM capable pin for output.
//#define PDM_VIA_I2S 3 // output PDM coded sample on the I2S data pin

// Set output mode
#define MBED_AUDIO_OUT_MODE INTERNAL_DAC

/// User config end. Do not modify below this line

#if (MBED_AUDIO_OUT_MODE == INTERNAL_DAC)
#define AUDIO_BITS 12
#define AUDIO_CHANNEL_1_PIN A12
#define AUDIO_CHANNEL_2_PIN A13
#define BYPASS_MOZZI_OUTPUT_BUFFER true
#elif (MBED_AUDIO_OUT_MODE == TIMED_PWM)
#define AUDIO_BITS 16  // well, used internally, at least. The pins will not be able to actually produce this many bits
#define AUDIO_CHANNEL_1_PIN D8
#define AUDIO_CHANNEL_2_PIN D9
/*#elif (ESP32_AUDIO_OUT_MODE == PDM_VIA_I2S)
#define AUDIO_BITS 16
#define PDM_RESOLUTION 4 */
#else
#error Invalid output mode configured in AudioConfigMBED.h
#endif

#define AUDIO_BIAS ((uint16_t) 1<<(AUDIO_BITS-1))

#endif        //  #ifndef AUDIOCONFIGMBED_H
