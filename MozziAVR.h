#pragma once
/*
 * MozziAvr.cpp
 *
 * Copyright 2012 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi by Tim Barrass is licensed under a Creative Commons
 * Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */
#include "hardware_defines.h"
#if IS_AVR() && USE_LEGACY_GUTS == false

#include "utility/FrequencyTimer2.h"
#include "utility/TimerOne.h"
//#include "mozzi_config.h"    // at the top of all MozziGuts and analog files
//#include "AudioConfigAll.h"  // AVR needs to load different config files
#include "MozziCommon.h"
#include "mozzi_analog.h"


#if (F_CPU != 16000000)
#warning                                                                       \
    "Mozzi has been tested with a cpu clock speed of 16MHz on Arduino and 48MHz on Teensy 3!  Results may vary with other speeds."
#endif

  // Forward declarations for locally used methods
void setupMozziADC(uint8_t speed);
void startAudioStandard();
void startAudioHiFi();
void stopTimers();


/**
 * 
 */

class MozziAVR  {
  public:

    void start(int control_rate_hz) {
      setupMozziADC(); 
      Common.startControl(control_rate_hz);
      #if (AUDIO_MODE == STANDARD) || (AUDIO_MODE == STANDARD_PLUS)
        startAudioStandard();
      #elif (AUDIO_MODE == HIFI)
        startAudioHiFi();
      #endif
    }

    void stop() {
      stopTimers();
    }

    unsigned long audioTicks() {
      return Common.audioTicks();
    }

    unsigned long mozziMicros() { 
      return Common.mozziMicros();;
    }

    void audioHook()  {
      Common.audioHook();
    }

 #if (USE_AUDIO_INPUT == true)
    int getAudioInput() {
        return Common.audio_input;
    } 
#endif

};


#endif
