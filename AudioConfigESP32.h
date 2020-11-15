#ifndef AUDIOCONFIGESP32_H
#define AUDIOCONFIGESP32_H

#if not IS_ESP32()
#error This header should be included for ESP32 architecture, only
#endif

#if (AUDIO_MODE == HIFI)
#error HIFI mode is not available for this CPU architecture (but check ESP32_AUDIO_OUT_MODE, and PDM_RESOLUTION)
#endif

// Audio output options
#define INTERNAL_DAC 1 // BUGGY !!! ESP32 internal DAC via I2S, requires AUDIO_RATE 32768, output on pin 26
#define PT8211_DAC 2 // ESP32 with PT8211 DAC via I2S, requires AUDIO_RATE 32768

// Set output mode
#define ESP32_AUDIO_OUT_MODE INTERNAL_DAC

// For external I2S output, only: I2S_PINS
#define ESP32_I2S_BCK_PIN 26
#define ESP32_I2S_WS_PIN 25
#define ESP32_I2S_DATA_PIN 33

/// User config end. Do not modify below this line

#if (ESP32_AUDIO_OUT_MODE == INTERNAL_DAC)
#define AUDIO_BITS 8
#elif (ESP32_AUDIO_OUT_MODE == PT8211_DAC)
#define AUDIO_BITS 16
#else
#error Invalid output mode configured in AudioConfigESP32.h
#endif

#define AUDIO_BIAS ((uint16_t) 1<<(AUDIO_BITS-1))

#endif        //  #ifndef AUDIOCONFIGESP_H
