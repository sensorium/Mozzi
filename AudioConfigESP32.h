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

#endif        //  #ifndef AUDIOCONFIGESP_H
