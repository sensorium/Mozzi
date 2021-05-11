
/**
 * @file Mozzi.cpp
 * @brief Compatibility Layer to support the old API
 * @version 0.1
 * @date 2021-05-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */
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

