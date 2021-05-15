#ifndef AUDIOCONFIGALL_H
#define AUDIOCONFIGALL_H

/**
* Include Platform dependent AudioConfig
* When using EXTERNAL_AUDIO_OUTPUT it is up to the corresponding implementation to decide to include this, the platform specific include or
* a separate configuration file.
* 
* Do not put any other DEFINES here - they should go to mozzi_config.h or hareware_defines.h 
*/

// TODO review this section - potentially move to hardware_defines.h 

// External output e.g. to DAC
#if (EXTERNAL_AUDIO_OUTPUT == true)
    #if !defined(EXTERNAL_AUDIO_BITS)
    #define EXTERNAL_AUDIO_BITS 16
    #endif
    #define AUDIO_BITS EXTERNAL_AUDIO_BITS
    #define AUDIO_BIAS (1 << (AUDIO_BITS - 1))
#else // EXTERNAL_AUDIO_OUTPUT==false
    #if IS_TEENSY3()
    #include "AudioConfigTeensy3_12bit.h"
    #elif IS_STM32()
    #include "AudioConfigSTM32.h"
    #elif IS_ESP8266()
    #include "AudioConfigESP8266.h"
    #elif IS_ESP32()
    #include "AudioConfigESP32.h"
    #elif IS_SAMD21()
    #include "AudioConfigSAMD21.h"
    #elif IS_AVR() && (AUDIO_MODE == STANDARD)
    #include "AudioConfigStandard9bitPwm.h"
    #elif IS_AVR() && (AUDIO_MODE == STANDARD_PLUS)
    #include "AudioConfigStandardPlus.h"
    #elif IS_AVR() && (AUDIO_MODE == HIFI)
    #include "AudioConfigHiSpeed14bitPwm.h"
    #elif IS_MBED() 
    #include "AudioConfigArmMbed.h"
    #elif IS_RP2040()  
    #include "AudioConfigRP2040.h"
    #else
    #error Undefined Architecture
    #endif
#endif

#endif