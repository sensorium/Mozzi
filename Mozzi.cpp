
/**
 * @file Mozzi.cpp
 * @brief Compatibility Layer to support the old API
 * @version 0.1
 * @date 2021-05-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "hardware_defines.h"
#if USE_LEGACY_GUTS == false
#include "Mozzi.h"

MozziClass Mozzi;

void startMozzi(int control_rate_hz) {
    return Mozzi.start(control_rate_hz);
}

void stopMozzi() {
    return Mozzi.stop();
}

void audioHook(){
    Mozzi.audioHook();
}

unsigned long audioTicks() {
    return Mozzi.audioTicks();
}


// Common Functionality - DO NOT ADD ANY PLATFORM SPECIFIC IFDEFS BELOW - You can opt in or provide your own implementation
// in the hardware-impl<platform>.cpp file !

/**
* Constructor for MozziControl:
* In the current configuration settings we support max 2 pins - so we automatically make them available
*/
#if (IS_AVR() || IS_TEENSY3() || IS_STM32() || IS_ESP8266() || IS_SAMD21() || IS_ESP32() || IS_RP2040() || IS_MBED())

MozziControl::MozziControl(){
    // intialize all cannels to to be undefined
    for (int j=0;j<AUDIO_CHANNELS;j++) {
        channel_pins[j] = -1;
    }

    // setup pin numbers
    channel_pins[0] = AUDIO_CHANNEL_1_PIN;

    #if AUDIO_CHANNELS > 1
        #ifdef AUDIO_CHANNEL_1_PIN_HIGH
            channel_pins[1] = AUDIO_CHANNEL_1_PIN_HIGH;
        #elif defined(AUDIO_CHANNEL_2_PIN)
            channel_pins[1] = AUDIO_CHANNEL_2_PIN;
        #endif
    #endif

}

#endif // Platforms



float debug_output;


#endif // USE_LEGACY_GUTS