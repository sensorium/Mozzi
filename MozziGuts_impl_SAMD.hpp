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

#if !(IS_SAMD21())
#  error "Wrong implementation included for this platform"
#endif

////// BEGIN analog input code ////////
//#define MOZZI_FAST_ANALOG_IMPLEMENTED // not yet
#define getADCReading() 0
#define channelNumToIndex(channel) channel
uint8_t adcPinToChannelNum(uint8_t pin) {
  return pin;
}
void adcStartConversion(uint8_t channel) {
#warning Fast analog read not implemented on this platform
}
void startSecondADCReadOnCurrentChannel() {
#warning Fast analog read not implemented on this platform
}
void setupFastAnalogRead(int8_t speed) {
#warning Fast analog read not implemented on this platform
}
void setupMozziADC(int8_t speed) {
#warning Fast analog read not implemented on this platform
}
////// END analog input code ////////



//// BEGIN AUDIO OUTPUT code ///////
// These are ARM SAMD21 Timer 5 routines to establish a sample rate interrupt
static bool tcIsSyncing() {
  return TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY;
}

static void tcReset() {
  // Reset TCx
  TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
  while (tcIsSyncing())
    ;
  while (TC5->COUNT16.CTRLA.bit.SWRST)
    ;
}
/* Not currently used, and does not compile with EXTERNAL_AUDIO_OUTPUT
static void tcEnd() {
  // Disable TC5
  TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  while (tcIsSyncing())
    ;
  tcReset();
  analogWrite(AUDIO_CHANNEL_1_PIN, 0);
} */

static void tcConfigure(uint32_t sampleRate) {
  // Enable GCLK for TCC2 and TC5 (timer counter input clock)
  GCLK->CLKCTRL.reg = (uint16_t)(GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 |
                                 GCLK_CLKCTRL_ID(GCM_TC4_TC5));
  while (GCLK->STATUS.bit.SYNCBUSY)
    ;

  tcReset();

  // Set Timer counter Mode to 16 bits
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16;

  // Set TC5 mode as match frequency
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;

  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_ENABLE;

  TC5->COUNT16.CC[0].reg = (uint16_t)(SystemCoreClock / sampleRate - 1);
  while (tcIsSyncing())
    ;

  // Configure interrupt request
  NVIC_DisableIRQ(TC5_IRQn);
  NVIC_ClearPendingIRQ(TC5_IRQn);
  NVIC_SetPriority(TC5_IRQn, 0);
  NVIC_EnableIRQ(TC5_IRQn);

  // Enable the TC5 interrupt request
  TC5->COUNT16.INTENSET.bit.MC0 = 1;
  while (tcIsSyncing())
    ;
}

void TC5_Handler(void) __attribute__((weak, alias("samd21AudioOutput")));

#ifdef __cplusplus
extern "C" {
#endif
void samd21AudioOutput() {
  defaultAudioOutput();
  TC5->COUNT16.INTFLAG.bit.MC0 = 1;
}
#ifdef __cplusplus
}
#endif

#if (EXTERNAL_AUDIO_OUTPUT != true)   // otherwise, the last stage - audioOutput() - will be provided by the user
#include "AudioConfigSAMD21.h"
inline void audioOutput(const AudioOutput f) {
  analogWrite(AUDIO_CHANNEL_1_PIN, f.l()+AUDIO_BIAS);
}
#endif

static void startAudio() {
#ifdef ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS
  {
    static const int CPLAY_SPEAKER_SHUTDOWN = 11;
    pinMode(CPLAY_SPEAKER_SHUTDOWN, OUTPUT);
    digitalWrite(CPLAY_SPEAKER_SHUTDOWN, HIGH);
  }

#endif
  analogWriteResolution(10);
#if (EXTERNAL_AUDIO_OUTPUT != true)
  analogWrite(AUDIO_CHANNEL_1_PIN, 0);
#endif

  tcConfigure(AUDIO_RATE);
}

void stopMozzi() {
  // TODO: implement me
  interrupts();
}
//// END AUDIO OUTPUT code ///////
