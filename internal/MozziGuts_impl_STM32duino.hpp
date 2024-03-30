/*
 * MozziGuts_impl_STM32duino.hpp
 *
 * This file is part of Mozzi.
 *
 * Copyright 2023-2024 Thomas Friedrichsmeier and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

#include "HardwareTimer.h"

namespace MozziPrivate {
////// BEGIN analog input code ////////

#if MOZZI_IS(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_STANDARD)
// Notes on ADC implementation: So in hours I could not get IRQ-driven ADC to work, much less in a way that should work across the whole STM32 family.
// Instead, this code resorts to polling, but contrary to the regular implementation, it sets does so non-blocking. Polling is done from inside audioHook().
// Not terribly efficient, but seems to work ok.
//
// All this still involves coping much, much more low level detail, than I would like. Most of that is moved to MozziGuts_impl_STM32duino_analog.hpp .
// If this core ever gets a more advanced ADC API, we should definitely switch to using that.

#define getADCReading() HAL_ADC_GetValue(&AdcHandle)

// NOTE: a single uint8_t for ADC channel is no good, here, as we may be dealing with serval distinct ADCs servicing the pins.
//       However, there is a real danger of overflowing an int8_t storage (as is used as an intermediate), so subtract min pin number.
#define channelNumToIndex(channel) channel
uint8_t adcPinToChannelNum(uint8_t pin) {
  return pin - PNUM_ANALOG_BASE;
}

uint32_t adc_pins_initialized = 0;
int16_t previously_sampled_pin = -1;
bool conversion_running = false;
ADC_HandleTypeDef AdcHandle = {};

} // namespace MozziPrivate
#include "MozziGuts_impl_STM32duino_analog.hpp"
namespace MozziPrivate {

void adcStartConversion(int8_t pin) {
  if (pin != previously_sampled_pin) {
    if (conversion_running) {
      HAL_ADC_Stop(&AdcHandle);
      HAL_ADC_DeInit(&AdcHandle);
    }
    previously_sampled_pin = pin;
    uint32_t mask = 1 << pin;
    if (!(adc_pins_initialized & mask)) {
      analogRead(pin+PNUM_ANALOG_BASE);  // I have no idea, what black magic analogRead() performs, but it seems to be needed - once - on STM32F411
      adc_pins_initialized += mask;
    }
  }
  adc_setup_read(analogInputToPinName(pin+PNUM_ANALOG_BASE), 16); // resolution will be limited to max available, anyway, so let's request 16 bits
  conversion_running = true;
}

void startSecondADCReadOnCurrentChannel() {
  HAL_ADC_Start(&AdcHandle);
  conversion_running = true;
}

void setupMozziADC(int8_t /*speed*/) {
}

#define AUDIO_HOOK_HOOK checkADCConversionComplete();

void checkADCConversionComplete() {
  if (!conversion_running) return;
  if(HAL_ADC_PollForConversion(&AdcHandle, 0) == HAL_OK) {
    conversion_running = false;
    advanceADCStep();
  }
}

#endif

void setupFastAnalogRead(int8_t /*speed*/) {
}

////// END analog input code ////////
////// END analog input code ////////



//// BEGIN AUDIO OUTPUT code ///////
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED)
HardwareTimer audio_update_timer(MOZZI_AUDIO_UPDATE_TIMER);
#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_2PIN_PWM)
HardwareTimer audio_update_timer(MOZZI_AUDIO_UPDATE_TIMER);
HardwareTimer *pwm_timer_ht;
PinName output_pin_1 = digitalPinToPinName(MOZZI_AUDIO_PIN_1);
uint32_t pwm_timer_channel_1 = STM_PIN_CHANNEL(pinmap_function(output_pin_1, PinMap_TIM));

#  if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_2PIN_PWM)
PinName output_pin_1_low = digitalPinToPinName(MOZZI_AUDIO_PIN_1_LOW);
uint32_t pwm_timer_channel_1_low = STM_PIN_CHANNEL(pinmap_function(output_pin_1_low, PinMap_TIM));
#  elif (MOZZI_AUDIO_CHANNELS > 1)
PinName output_pin_2 = digitalPinToPinName(MOZZI_AUDIO_PIN_2);
uint32_t pwm_timer_channel_2 = STM_PIN_CHANNEL(pinmap_function(output_pin_2, PinMap_TIM));
#  endif

inline void audioOutput(const AudioOutput f) {
#  if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_2PIN_PWM)
  pwm_timer_ht->setCaptureCompare(pwm_timer_channel_1, (f.l()+MOZZI_AUDIO_BIAS) >> MOZZI_AUDIO_BITS_PER_CHANNEL);
  pwm_timer_ht->setCaptureCompare(pwm_timer_channel_1_low, (f.l()+MOZZI_AUDIO_BIAS) & ((1 << MOZZI_AUDIO_BITS_PER_CHANNEL) - 1));
#  else
  pwm_timer_ht->setCaptureCompare(pwm_timer_channel_1, f.l()+MOZZI_AUDIO_BIAS);
#    if (MOZZI_AUDIO_CHANNELS > 1)
  pwm_timer_ht->setCaptureCompare(pwm_timer_channel_2, f.r()+MOZZI_AUDIO_BIAS);
#    endif
#endif
}
#endif

static void startAudio() {
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_2PIN_PWM, MOZZI_OUTPUT_EXTERNAL_TIMED)
  audio_update_timer.pause();
  //audio_update_timer.setPeriod(1000000UL / MOZZI_AUDIO_RATE);
  // Manually calculate prescaler and overflow instead of using setPeriod, to avoid rounding errors
  uint32_t period_cyc = F_CPU / MOZZI_AUDIO_RATE;
  uint16_t prescaler = (uint16_t)(period_cyc / 65535 + 1);
  uint16_t overflow = (uint16_t)((period_cyc + (prescaler / 2)) / prescaler);
  audio_update_timer.setPrescaleFactor(prescaler);
  audio_update_timer.setOverflow(overflow);
  audio_update_timer.setMode(/* channel */ 1, TIMER_OUTPUT_COMPARE);
  audio_update_timer.setCaptureCompare(/* channel */ 1, 1); // Interrupt 1 count after each update
  audio_update_timer.attachInterrupt(/* channel */ 1, defaultAudioOutput);
  audio_update_timer.refresh();
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_2PIN_PWM)
  // Configure PWM output
  pinMode(MOZZI_AUDIO_PIN_1, OUTPUT);
#  if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_2PIN_PWM)
  pinMode(MOZZI_AUDIO_PIN_1_LOW, OUTPUT);
#  elif (MOZZI_AUDIO_CHANNELS > 1)
  pinMode(MOZZI_AUDIO_PIN_2, OUTPUT);
#  endif

#  define MAX_CARRIER_FREQ (F_CPU / (1 << MOZZI_AUDIO_BITS_PER_CHANNEL))
  // static_assert(MAX_CARRIER_FREQ >= MOZZI_AUDIO_RATE); // Unfortunately, we cannot test this at compile time. F_CPU expands to a runtime variable
  TIM_TypeDef *pwm_timer_tim = (TIM_TypeDef *) pinmap_peripheral(output_pin_1, PinMap_TIM);
  pwm_timer_ht = new HardwareTimer(pwm_timer_tim);
  pwm_timer_ht->setMode(pwm_timer_channel_1, TIMER_OUTPUT_COMPARE_PWM1, output_pin_1);
#  if MAX_CARRIER_FREQ < (MOZZI_AUDIO_RATE * 5)
  // Generate as fast a carrier as possible
  pwm_timer_ht->setPrescaleFactor(1);
#  else
  // No point in generating arbitrarily high carrier frequencies. In fact, if
  // there _is_ any headroom, give the PWM pin more time to swing from HIGH to
  // LOW and BACK, cleanly
  pwm_timer_ht->setPrescaleFactor((int)MAX_CARRIER_FREQ / (MOZZI_AUDIO_RATE * 5)); // as fast as possible
#  endif
// Allocate enough room to write all intended bits
  pwm_timer_ht->setOverflow(1 << MOZZI_AUDIO_BITS_PER_CHANNEL);
  pwm_timer_ht->setCaptureCompare(pwm_timer_channel_1, MOZZI_AUDIO_BIAS /*, resolution */);

  pwm_timer_ht->resume();
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_2PIN_PWM, MOZZI_OUTPUT_EXTERNAL_TIMED)
  audio_update_timer.resume();
#endif
}

void stopMozzi() {
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_2PIN_PWM, MOZZI_OUTPUT_EXTERNAL_TIMED)
  audio_update_timer.pause();
#endif
}

//// END AUDIO OUTPUT code ///////

//// BEGIN Random seeding ////////
void MozziRandPrivate::autoSeed() {
#warning Automatic random seeding is not implemented on this platform
}
//// END Random seeding ////////

} // namespace MozziPrivate
