#ifndef AUDIOCONFIGESP32_H
#define AUDIOCONFIGESP32_H

#if not IS_ESP32()
#error This header should be included for ESP32 architecture, only
#endif

#if (AUDIO_MODE == HIFI)
#error HIFI mode is not available for this CPU architecture (but check ESP32_AUDIO_OUT_MODE, and PDM_RESOLUTION)
#endif

// Audio output options
#define INTERNAL_DAC 1 // output using internal DAC via I2S, output on pin 26
#define PT8211_DAC 2 // output using an external PT8211 DAC via I2S
#define PDM_VIA_I2S 3 // output PDM coded sample on the I2S data pin (pin 33, by default, configurable, below)

// Set output mode
#define ESP32_AUDIO_OUT_MODE INTERNAL_DAC

// For external I2S output, only: I2S_PINS
#define ESP32_I2S_BCK_PIN 26
#define ESP32_I2S_WS_PIN 25
#define ESP32_I2S_DATA_PIN 33

#include <driver/i2s.h>
const i2s_port_t i2s_num = I2S_NUM_0;
/// User config end. Do not modify below this line

#if (ESP32_AUDIO_OUT_MODE == INTERNAL_DAC)
#define AUDIO_BITS 8
#define PDM_RESOLUTION 1
#elif (ESP32_AUDIO_OUT_MODE == PT8211_DAC)
#define AUDIO_BITS 16
#define PDM_RESOLUTION 1
#elif (ESP32_AUDIO_OUT_MODE == PDM_VIA_I2S)
#define AUDIO_BITS 16
#define PDM_RESOLUTION 4
#else
#error Invalid output mode configured in AudioConfigESP32.h
#endif

#define AUDIO_BIAS ((uint16_t) 1<<(AUDIO_BITS-1))
#define BYPASS_MOZZI_OUTPUT_BUFFER true

#endif        //  #ifndef AUDIOCONFIGESP_H
