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

#include "HardwareTimer.h"

////// BEGIN analog input code ////////
#define MOZZI_FAST_ANALOG_IMPLEMENTED
//#include <STM32ADC.h>  // Disabled, here. See AudioConfigSTM32.h
STM32ADC adc(ADC1);
uint8_t stm32_current_adc_pin;   // TODO: this is actually a "channel" according to our terminology, but "channel" and "pin" are equal on this platform
#define getADCReading() adc.getData()
#define channelNumToIndex(channel) STM32PinMap(channel)
uint8_t adcPinToChannelNum(uint8_t pin) {
  return pin;
}

void adcStartConversion(uint8_t channel) {
  stm32_current_adc_pin = channel;
  adc.setPins(&stm32_current_adc_pin, 1);
  adc.startConversion();
}

static void startSecondADCReadOnCurrentChannel() {
  adc.setPins(&stm32_current_adc_pin, 1);
  adc.startConversion();
}

void stm32_adc_eoc_handler() {
  advanceADCStep();
}

void setupFastAnalogRead(int8_t speed) {
  // NOTE: These picks are pretty arbitrary. Further available options are 7_5, 28_5, 55_5, 71_5 and 239_5 (i.e. 7.5 ADC cylces, etc.)
  if (speed == FASTEST_ADC) adc.setSampleRate(ADC_SMPR_1_5);
  else if (speed == FASTER_ADC) adc.setSampleRate(ADC_SMPR_13_5);
  else (adc.setSampleRate(ADC_SMPR_41_5));
}

void setupMozziADC(int8_t speed) {
  adc.attachInterrupt(stm32_adc_eoc_handler);
}


inline uint8_t STM32PinMap(uint8_t pin)
{  
  if (pin > 15) return pin-8;
  else return pin;
}

////// END analog input code ////////



//// BEGIN AUDIO OUTPUT code ///////
#if (EXTERNAL_AUDIO_OUTPUT == true)
HardwareTimer audio_update_timer(2);
#else
HardwareTimer audio_update_timer(AUDIO_UPDATE_TIMER);
HardwareTimer audio_pwm_timer(AUDIO_PWM_TIMER);

#include "AudioConfigSTM32.h"
inline void audioOutput(const AudioOutput f) {
#  if (AUDIO_MODE == HIFI)
  pwmWrite(AUDIO_CHANNEL_1_PIN, (f.l()+AUDIO_BIAS) & ((1 << AUDIO_BITS_PER_CHANNEL) - 1));
  pwmWrite(AUDIO_CHANNEL_1_PIN_HIGH, (f.l()+AUDIO_BIAS) >> AUDIO_BITS_PER_CHANNEL);
#  else
  pwmWrite(AUDIO_CHANNEL_1_PIN, f.l()+AUDIO_BIAS);
#    if (AUDIO_CHANNELS > 1)
  pwmWrite(AUDIO_CHANNEL_2_PIN, f.r()+AUDIO_BIAS);
#    endif
#endif
}
#endif

static void startAudio() {
  audio_update_timer.pause();
  //audio_update_timer.setPeriod(1000000UL / AUDIO_RATE);
  // Manually calculate prescaler and overflow instead of using setPeriod, to avoid rounding errors
  uint32_t period_cyc = F_CPU / AUDIO_RATE;
  uint16_t prescaler = (uint16_t)(period_cyc / 65535 + 1);
  uint16_t overflow = (uint16_t)((period_cyc + (prescaler / 2)) / prescaler);
  audio_update_timer.setPrescaleFactor(prescaler);
  audio_update_timer.setOverflow(overflow);
  audio_update_timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  audio_update_timer.setCompare(TIMER_CH1,
                                1); // Interrupt 1 count after each update
  audio_update_timer.attachCompare1Interrupt(defaultAudioOutput);
  audio_update_timer.refresh();
  audio_update_timer.resume();

#if (EXTERNAL_AUDIO_OUTPUT != true)
  pinMode(AUDIO_CHANNEL_1_PIN, PWM);
#  if (AUDIO_MODE == HIFI)
  pinMode(AUDIO_CHANNEL_1_PIN_HIGH, PWM);
#  elif (AUDIO_CHANNELS > 1)
  pinMode(AUDIO_CHANNEL_2_PIN, PWM);
#  endif

#  define MAX_CARRIER_FREQ (F_CPU / (1 << AUDIO_BITS_PER_CHANNEL))
#  if MAX_CARRIER_FREQ < AUDIO_RATE
#    error Configured audio resolution is definitely too high at the configured audio rate (and the given CPU speed)
#  elif MAX_CARRIER_FREQ < (AUDIO_RATE * 3)
#    warning Configured audio resolution may be higher than optimal at the configured audio rate (and the given CPU speed)
#  endif

#  if MAX_CARRIER_FREQ < (AUDIO_RATE * 5)
  // Generate as fast a carrier as possible
  audio_pwm_timer.setPrescaleFactor(1);
#  else
  // No point in generating arbitrarily high carrier frequencies. In fact, if
  // there _is_ any headroom, give the PWM pin more time to swing from HIGH to
  // LOW and BACK, cleanly
  audio_pwm_timer.setPrescaleFactor((int)MAX_CARRIER_FREQ / (AUDIO_RATE * 5));
#  endif
  audio_pwm_timer.setOverflow(
      1 << AUDIO_BITS_PER_CHANNEL); // Allocate enough room to write all
                                    // intended bits
#endif
}

void stopMozzi() {
  audio_update_timer.pause();
}

//// END AUDIO OUTPUT code ///////
