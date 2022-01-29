/*
 * mozzi_analog.cpp
 *
 * Copyright 2012 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */


#include "mozzi_config.h"
#include "mozzi_analog.h"

#include "hardware_defines.h"

/** NOTE: Since analog input code is heavily hardware dependent, and also heavily interweaved with AUDIO_INPUT,
 *  it was moved to MozziGuts.cpp / MozziGuts_impl_XYZ.hpp for better maintainability.
 *
 *  TODO: The (dis|re)connect functions below remain for now, as I'm not sure what to do about them. They were only ever
 *  implemented for AVR.
 */

void disconnectDigitalIn(uint8_t channel_num){
#if IS_AVR()
	DIDR0 |= 1<<channel_num;
#endif
}


void reconnectDigitalIn(uint8_t channel_num){
#if IS_AVR()
	DIDR0 &= ~(1<<channel_num);
#endif
}


void adcDisconnectAllDigitalIns(){
#if IS_AVR()
	for (uint8_t i = 0; i<NUM_ANALOG_INPUTS; i++){
		DIDR0 |= 1<<i;
	}
#endif
}


void adcReconnectAllDigitalIns(){
#if IS_AVR()
	for (uint8_t i = 0; i<NUM_ANALOG_INPUTS; i++){
		DIDR0 &= ~(1<<i);
	}
#endif
}


