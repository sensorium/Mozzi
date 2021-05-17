
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

PLATFORM_OUTPUT_CLASS Mozzi;
MozziCommon Common;

void startMozzi(int control_rate_hz) {
    Mozzi.start(control_rate_hz);
}

void stopMozzi() {
    Mozzi.stop();
}

void audioHook(){
    Mozzi.audioHook();
}

#if (USE_AUDIO_INPUT == true)
int getAudioInput(){
    return Mozzi.getAudioInput();
}
#endif

unsigned long mozziMicros(){
    return Common.mozziMicros();
}

unsigned long audioTicks(){
    return Common.audioTicks();
}

#endif // USE_LEGACY_GUTS