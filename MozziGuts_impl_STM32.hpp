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
//#include <STM32ADC.h>  // Disabled, here. See AudioConfigSTM32.h
STM32ADC adc(ADC1);
uint8_t stm32_current_adc_pin;

////// BEGIN AUDIO INPUT code ////////
#if (USE_AUDIO_INPUT == true)
static void startFirstAudioADC() {
  uint8_t dummy = AUDIO_INPUT_PIN;
  adc.setPins(&dummy, 1);
  adc.startConversion();
}

static void startSecondAudioADC() {
  uint8_t dummy = AUDIO_INPUT_PIN;
  adc.setPins(&dummy, 1);
  adc.startConversion();
}

static void receiveSecondAudioADC() {
  if (!input_buffer.isFull())
    input_buffer.write(adc.getData());
}

void stm32_adc_eoc_handler()
{
  advancdeADCStep();
}
#endif
////// END AUDIO INPUT code ////////


#if (EXTERNAL_AUDIO_OUTPUT == true)
HardwareTimer audio_update_timer(2);
#else
HardwareTimer audio_update_timer(AUDIO_UPDATE_TIMER);
HardwareTimer audio_pwm_timer(AUDIO_PWM_TIMER);
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
#if (AUDIO_MODE == HIFI)
  pinMode(AUDIO_CHANNEL_1_PIN_HIGH, PWM);
#elif (AUDIO_CHANNELS > 1)
  pinMode(AUDIO_CHANNEL_2_PIN, PWM);
#endif

#define MAX_CARRIER_FREQ (F_CPU / (1 << AUDIO_BITS_PER_CHANNEL))
#if MAX_CARRIER_FREQ < AUDIO_RATE
#error Configured audio resolution is definitely too high at the configured audio rate (and the given CPU speed)
#elif MAX_CARRIER_FREQ < (AUDIO_RATE * 3)
#warning Configured audio resolution may be higher than optimal at the configured audio rate (and the given CPU speed)
#endif

#if MAX_CARRIER_FREQ < (AUDIO_RATE * 5)
  // Generate as fast a carrier as possible
  audio_pwm_timer.setPrescaleFactor(1);
#else
  // No point in generating arbitrarily high carrier frequencies. In fact, if
  // there _is_ any headroom, give the PWM pin more time to swing from HIGH to
  // LOW and BACK, cleanly
  audio_pwm_timer.setPrescaleFactor((int)MAX_CARRIER_FREQ / (AUDIO_RATE * 5));
#endif
  audio_pwm_timer.setOverflow(
      1 << AUDIO_BITS_PER_CHANNEL); // Allocate enough room to write all
                                    // intended bits
}

void stopMozzi() {
  audio_update_timer.pause();
}

unsigned long mozziMicros() { return audioTicks() * MICROS_PER_AUDIO_TICK; }
