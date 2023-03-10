#ifndef AUDIOCONFIGESP32_H
#define AUDIOCONFIGESP32_H

#include <driver/i2s.h>
#include <soc/adc_channel.h>

#if not IS_ESP32()
#error This header should be included for ESP32 architecture, only
#endif

#if (AUDIO_MODE == HIFI)
#error HIFI mode is not available for this CPU architecture (but check ESP32_AUDIO_OUT_MODE, and PDM_RESOLUTION)
#endif

// Audio IO options
#define INTERNAL_DAC 1 // output using internal DAC via I2S, output on pin 26
#define PT8211_DAC 2   // OBSOLETE: use PT8211_DAC instead
#define PDM_VIA_I2S 3  // output PDM coded sample on the I2S data pin (pin 33, by default, configurable, below)
#define I2S_DAC_AND_I2S_ADC 4  // output using an external DAC , input with exteran ADC - both via I2S

// Set output & input mode
#define ESP32_AUDIO_OUT_MODE INTERNAL_DAC

// For external I2S output, only: I2S_PINS
#define ESP32_I2S_BCK_PIN 26
#define ESP32_I2S_WS_PIN 25
#define ESP32_I2S_DATA_PIN 33
#define ESP32_I2S_DATA_PIN_IN 32  // relevant with I2S_ADC


// Preferred I2S port: note that this might be taken into consideration if there is a choice
// The Internal ADC and DAC only work on port 0 and can not be used at the same time!
const i2s_port_t i2s_num = I2S_NUM_0;


// Select the data range of the ADC
//#define ADC_VALUE(in) (in) // no scaling
#define ADC_VALUE(in) ((in + 32768)*0.015625) // scale to 0 to 1023

// Optionally select a higher Sample Rate
//#define ESP32_AUDIO_RATE 8000


/// User config end. Do not modify below this line
///--------------------------------------------------------------------------------------------------------

#if (ESP32_AUDIO_OUT_MODE == INTERNAL_DAC)
#define AUDIO_BITS 8
#define PDM_RESOLUTION 1
#elif (ESP32_AUDIO_OUT_MODE == PT8211_DAC  || ESP32_AUDIO_OUT_MODE == I2S_DAC_AND_I2S_ADC)
#define AUDIO_BITS 16
#define PDM_RESOLUTION 1
#elif (ESP32_AUDIO_OUT_MODE == PDM_VIA_I2S)
#define AUDIO_BITS 16
#define PDM_RESOLUTION 4
#else
#error Invalid output mode configured in AudioConfigESP32.h
#endif

/// Activate Analog Input
#if (ESP32_AUDIO_OUT_MODE == PT8211_DAC_AND_INTERNAL_ADC || ESP32_AUDIO_OUT_MODE == I2S_DAC_AND_I2S_ADC || ESP32_AUDIO_OUT_MODE == INTERNAL_DAC_AND_I2S_ADC)
#  define MOZZI_FAST_ANALOG_IMPLEMENTED // not yet
#endif

/// Logic for Output Buffer
#define IS_INTERNAL_DAC() (ESP32_AUDIO_OUT_MODE==INTERNAL_DAC || ESP32_AUDIO_OUT_MODE==INTERNAL_DAC_AND_I2S_ADC)
#define IS_I2S_DAC() (ESP32_AUDIO_OUT_MODE==I2S_DAC || ESP32_AUDIO_OUT_MODE==I2S_DAC_AND_INTERNAL_ADC||ESP32_AUDIO_OUT_MODE==I2S_DAC_AND_I2S_ADC)
#define IS_PDM() (ESP32_AUDIO_OUT_MODE==PDM_VIA_I2S) 

#define AUDIO_BIAS ((uint16_t) 1<<(AUDIO_BITS-1))
#define BYPASS_MOZZI_OUTPUT_BUFFER true

// Use defined rate as new AUDIO_RATE_PLATFORM_DEFAULT
#ifdef ESP32_AUDIO_RATE
#  undef AUDIO_RATE_PLATFORM_DEFAULT 
#  define AUDIO_RATE_PLATFORM_DEFAULT ESP32_AUDIO_RATE
#endif

// If ADC is available we support getAudioInput()
#if ESP32_AUDIO_OUT_MODE == I2S_DAC_AND_I2S_ADC
int getAudioInput();
#endif



#endif        //  #ifndef AUDIOCONFIGESP_H
