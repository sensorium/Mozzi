/*
 * MozziGuts.cpp
 *
 * Copyright 2012 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi by Tim Barrass is licensed under a Creative Commons
 * Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */

#if !(IS_TEENSY3() || IS_TEENSY4())
#  error "Wrong implementation included for this platform"
#endif

// required from http://github.com/pedvide/ADC for Teensy 3.*
#include "IntervalTimer.h"
#include <ADC.h>

#if (IS_TEENSY3() && F_CPU != 48000000)
#warning                                                                       \
    "Mozzi has been tested with a cpu clock speed of 16MHz on Arduino and 48MHz on Teensy 3!  Results may vary with other speeds."
#endif

ADC *adc; // adc object
uint8_t teensy_pin;
int8_t teensy_adc=0;


////// BEGIN AUDIO INPUT code ////////
#if (USE_AUDIO_INPUT == true)
static void startFirstAudioADC() {
  adc->adc0->startSingleRead(
      AUDIO_INPUT_PIN); // ADC lib converts pin/channel in startSingleRead
}

static void startSecondAudioADC() {
  adc->adc0->startSingleRead(AUDIO_INPUT_PIN);
}

static void receiveSecondAudioADC() {
  if (!input_buffer.isFull())
    input_buffer.write(adc->adc0->readSingle());
}

void adc0_isr(void)
{
  advanceADCStep();
}
#endif
////// END AUDIO INPUT code ////////



IntervalTimer timer1;

static void startAudio() {
  adc->adc0->setAveraging(0);
  adc->adc0->setConversionSpeed(
				ADC_CONVERSION_SPEED::MED_SPEED); // could be HIGH_SPEED, noisier
#ifdef ADC_DUAL_ADCS
  adc->adc1->setAveraging(0);
  adc->adc1->setConversionSpeed(
				ADC_CONVERSION_SPEED::MED_SPEED);
#endif
  
#if IS_TEENSY3()
  analogWriteResolution(12);
#elif IS_TEENSY4()
  analogWriteResolution(10);
#  if (!EXTERNAL_AUDIO_OUTPUT)
  analogWriteFrequency(AUDIO_CHANNEL_1_PIN, 146484.38f);
#    if (AUDIO_CHANNELS > 1)
  analogWriteFrequency(AUDIO_CHANNEL_2_PIN, 146484.38f);
#    endif  // end #if (AUDIO_CHANNELS > 1) 
#  endif  // end #if (!EXTERNAL_AUDIO_OUTPUT)
#endif
  timer1.begin(defaultAudioOutput, 1000000. / AUDIO_RATE);
}

void stopMozzi() {
  timer1.end();
  interrupts();
}
