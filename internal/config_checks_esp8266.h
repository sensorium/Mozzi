#ifndef CONFIG_CHECK_ESP8266_H
#define CONFIG_CHECK_ESP8266_H


#if not IS_ESP8266()
#error This header should be included for ESP architecture, only
#endif

#if !defined(MOZZI_AUDIO_MODE)
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_PDM_VIA_SERIAL
#endif
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_EXTERNAL_CUSTOM, MOZZI_OUTPUT_PDM_VIA_I2S, MOZZI_OUTPUT_PDM_VIA_SERIAL, MOZZI_OUTPUT_I2S_DAC)

#if !defined(MOZZI_AUDIO_RATE)
#define MOZZI_AUDIO_RATE 32768
#endif

#if !defined(MOZZI_AUDIO_BITS)
#  define MOZZI_AUDIO_BITS 16
#endif

#if !defined(MOZZI_ANALOG_READ)
#  define MOZZI_ANALOG_READ MOZZI_ANALOG_READ_NONE
#endif

MOZZI_CHECK_SUPPORTED(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_NONE)
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_INPUT, MOZZI_AUDIO_INPUT_NONE)

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S, MOZZI_OUTPUT_PDM_VIA_SERIAL)
#  if !defined(PDM_RESOLUTION)
#    define MOZZI_PDM_RESOLUTION 2
#  endif
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_CHANNELS, 1)
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_BITS, 16)
#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC)
#  define MOZZI_PDM_RESOLUTION 1   // DO NOT CHANGE THIS VALUE! Not actually PDM coded, but this define is useful to keep code simple.
MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_BITS, 16)
#endif

#define PDM_RESOLUTION 2   // 1 corresponds to 32 PDM clocks per sample, 2 corresponds to 64 PDM clocks, etc. (and at some level you're going hit the hardware limits)

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S, MOZZI_OUTPUT_I2S_DAC)
// NOTE: On ESP / output via I2S, we simply use the I2S buffer as the output
// buffer, which saves RAM, but also simplifies things a lot
// esp. since i2s output already has output rate control -> no need for a
// separate output timer
#define BYPASS_MOZZI_OUTPUT_BUFFER true
#endif

#endif        //  #ifndef CONFIG_CHECK_ESP8266_H
