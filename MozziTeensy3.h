/*
 * MozziTeensy3.h
 *
 * Copyright 2012 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi by Tim Barrass is licensed under a Creative Commons
 * Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */
#pragma once

#include "hardware_defines.h"
#if IS_TEENSY3() && USE_LEGACY_GUTS == false
#include "MozziCommon.h"

class MozziTeensy3  {
  public:
    void start(int control_rate_hz);
    void stop();
    int getAudioInput();
    unsigned long audioTicks();
    unsigned long mozziMicros();
    void audioHook();

#if (USE_AUDIO_INPUT == true)
    int getAudioInput();    
#endif


};

#endif
