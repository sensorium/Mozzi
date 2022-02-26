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
#include "teensyPinMap.h"

#if (IS_TEENSY3() && F_CPU != 48000000)
#warning                                                                       \
    "Mozzi has been tested with a cpu clock speed of 16MHz on Arduino and 48MHz on Teensy 3!  Results may vary with other speeds."
#endif

////// BEGIN analog input code ////////
#define MOZZI_FAST_ANALOG_IMPLEMENTED
ADC *adc; // adc object
uint8_t teensy_pin;   // TODO: this is actually a "channel" according to our terminology, but "channel" and "pin" are equal on this platform
int8_t teensy_adc=0;
#define getADCReading() adc->readSingle(teensy_adc)
#define channelNumToIndex(channel) teensyPinMap(channel)
uint8_t adcPinToChannelNum(uint8_t pin) {
  return pin;
}

void setupFastAnalogRead(int8_t speed) {
  adc->adc0->setAveraging(0);
  adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED); // could be HIGH_SPEED, noisier
#ifdef ADC_DUAL_ADCS
  adc->adc1->setAveraging(0);
  adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED);
#endif
}

void adc0_isr(void)
{
  advanceADCStep();
}

void setupMozziADC(int8_t speed) {
  adc = new ADC();
  adc->adc0->enableInterrupts(adc0_isr);
#ifdef ADC_DUAL_ADCS
  adc->adc1->enableInterrupts(adc0_isr);
#endif
}

void adcStartConversion(uint8_t channel) {
  teensy_pin = channel; // remember for startSecondADCReadOnCurrentChannel()
#ifdef ADC_DUAL_ADCS
  if (adc->adc0->checkPin(teensy_pin)) teensy_adc = 0;
  else teensy_adc=1;
#endif
  adc->startSingleRead(teensy_pin,teensy_adc);
}

static void startSecondADCReadOnCurrentChannel() {
  adc->startSingleRead(teensy_pin,teensy_adc);
}

////// END  analog input code ////////



//// BEGIN AUDIO OUTPUT code ///////
IntervalTimer timer1;

#if (EXTERNAL_AUDIO_OUTPUT != true) // otherwise, the last stage - audioOutput() - will be provided by the user
#if IS_TEENSY3()
#include "AudioConfigTeensy3_12bit.h"
#elif IS_TEENSY4()
#include "AudioConfigTeensy4.h"
#endif
inline void audioOutput(const AudioOutput f) {
  analogWrite(AUDIO_CHANNEL_1_PIN, f.l()+AUDIO_BIAS);
#if (AUDIO_CHANNELS > 1)
  analogWrite(AUDIO_CHANNEL_2_PIN, f.r()+AUDIO_BIAS);
#endif
}
#endif

static void startAudio() {
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
//// END AUDIO OUTPUT code ///////
