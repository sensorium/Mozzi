#ifndef AUDIOCONFIGMBED_H
#define AUDIOCONFIGMBED_H

#if not IS_MBED()
#error This header should be included for MBED OS boards, only
#endif

#if (AUDIO_MODE == HIFI)
#error HIFI mode is not available for this CPU architecture (but several high quality output options are available)
#endif

// Audio output options
#define INTERNAL_DAC 1 // output using internal DAC via I2S, output on pin 26
//#define PT8211_DAC 2 // output using an external PT8211 DAC via I2S
//#define PDM_VIA_I2S 3 // output PDM coded sample on the I2S data pin (pin 33, by default, configurable, below)

// Set output mode
#define MBED_AUDIO_OUT_MODE INTERNAL_DAC

// For external I2S output, only: I2S_PINS
//#define ESP32_I2S_BCK_PIN 26
//#define ESP32_I2S_WS_PIN 25
//#define ESP32_I2S_DATA_PIN 33

//#include <driver/i2s.h>
//const i2s_port_t i2s_num = I2S_NUM_0;
/// User config end. Do not modify below this line

#if (MBED_AUDIO_OUT_MODE == INTERNAL_DAC)
#define AUDIO_BITS 12
#define AUDIO_CHANNEL_1_PIN A12
#define AUDIO_CHANNEL_2_PIN A13
/*#elif (ESP32_AUDIO_OUT_MODE == PT8211_DAC)
#define AUDIO_BITS 16
#define PDM_RESOLUTION 1
#elif (ESP32_AUDIO_OUT_MODE == PDM_VIA_I2S)
#define AUDIO_BITS 16
#define PDM_RESOLUTION 4 */
#else
#error Invalid output mode configured in AudioConfigMBED.h
#endif

#define AUDIO_BIAS ((uint16_t) 1<<(AUDIO_BITS-1))
#define BYPASS_MOZZI_OUTPUT_BUFFER true

#endif        //  #ifndef AUDIOCONFIGMBED_H
