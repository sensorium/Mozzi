#ifndef AUDIOCONFIGESP32_H
#define AUDIOCONFIGESP32_H

#if not IS_ESP32()
#error This header should be included for ESP32 architecture, only
#endif

#if (AUDIO_MODE == HIFI)
#error HIFI mode is not available for this CPU architecture (but check ESP32_AUDIO_OUT_MODE, and PDM_RESOLUTION)
#endif

#if (STEREO_HACK == true)
#error Stereo is not available for this CPU
#endif

#define AUDIO_BIAS ((uint16_t) 1<<(7))

// Audio output options
#define INTERNAL_DAC 1 // BUGGY !!! ESP32 internal DAC via I2S, requires AUDIO_RATE 32768, output on pin 26
#define PT8211_DAC 2 // ESP32 with PT8211 DAC via I2S, requires AUDIO_RATE 32768

// Set output mode
#define ESP32_AUDIO_OUT_MODE PT8211_DAC


#endif        //  #ifndef AUDIOCONFIGESP_H
