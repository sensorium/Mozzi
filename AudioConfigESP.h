#ifndef AUDIOCONFIGESP_H
#define AUDIOCONFIGESP_H

#if not IS_ESP8266()
#error This header should be included for ESP architecture, only
#endif

// AUDIO output modes. See README.md
#define PDM_VIA_I2S 1
#define PDM_VIA_SERIAL 2
#define EXTERNAL_DAC_VIA_I2S 3    // output via external DAC connected to I2S (PT8211 or similar)

#define ESP_AUDIO_OUT_MODE PDM_VIA_SERIAL
#define PDM_RESOLUTION 1   // 1 corresponds to 32 PDM clocks per sample, 2 corresponds to 64 PDM clocks (and more will be overkill)

#if (ESP_AUDIO_OUT_MODE == EXTERNAL_DAC_VIA_I2S)
#define PDM_RESOLUTION 1   // fix at 1, allows for easier coding
#endif

#if (AUDIO_MODE == HIFI)
#error HIFI mode is not available for this CPU architecture (but check ESP_AUDI_OUT_MODE, and PDM_RATE)
#endif

#if (STEREO_HACK == true)
#if (ESP_AUDIO_OUT_MODE != EXTERNAL_DAC_VIA_I2S)
#error Stereo is not available for the configured audio output mode
#endif
#endif

#define AUDIO_BIAS ((uint16_t) 1<<(15))

#endif        //  #ifndef AUDIOCONFIGESP_H
