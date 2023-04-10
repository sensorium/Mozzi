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

/** NOTE: This section deals with implementing (fast) asynchronous analog reads, which form the backbone of mozziAnalogRead(), but also of USE_AUDIO_INPUT (if enabled).
 *  This template provides empty/dummy implementations to allow you to skip over this section, initially. Once you have an implementation, be sure to enable the
 *  #define, below: */
//#define MOZZI_FAST_ANALOG_IMPLEMENTED

// Insert here code to read the result of the latest asynchronous conversion, when it is finished.
// You can also provide this as a function returning unsigned int, should it be more complex on your platform
#define getADCReading() 0

/** NOTE: On "pins" vs. "channels" vs. "indices"
 *  "Pin" is the pin number as would usually be specified by the user in mozziAnalogRead().
 *  "Channel" is an internal ADC channel number corresponding to that pin. On many platforms this is simply the same as the pin number, on others it differs.
 *      In other words, this is an internal representation of "pin".
 *  "Index" is the index of the reading for a certain pin/channel in the array of analog_readings, ranging from 0 to NUM_ANALOG_PINS. This, again may be the
 *      same as "channel" (e.g. on AVR), however, on platforms where ADC-capable "channels" are not numbered sequentially starting from 0, the channel needs
 *      to be converted to a suitable index.
 *
 *  In summary, the semantics are roughly
 *      mozziAnalogRead(pin) -> _ADCimplementation_(channel) -> analog_readings[index]
 *  Implement adcPinToChannelNum() and channelNumToIndex() to perform the appropriate mapping.
 */
// NOTE: Theoretically, adcPinToChannelNum is public API for historical reasons, thus cannot be replaced by a define
#define channelNumToIndex(channel) channel
uint8_t adcPinToChannelNum(uint8_t pin) {
  return pin;
}

/** NOTE: Code needed to trigger a conversion on a new channel */
void adcStartConversion(uint8_t channel) {
#warning Fast analog read not implemented on this platform
}

/** NOTE: Code needed to trigger a subsequent conversion on the latest channel. If your platform has no special code for it, you should store the channel from
 *  adcStartConversion(), and simply call adcStartConversion(previous_channel), here. */
void startSecondADCReadOnCurrentChannel() {
#warning Fast analog read not implemented on this platform
}

/** NOTE: Code needed to set up faster than usual analog reads, e.g. specifying the number of CPU cycles that the ADC waits for the result to stabilize.
 *  This particular function is not super important, so may be ok to leave empty, at least, if the ADC is fast enough by default. */
void setupFastAnalogRead(int8_t speed) {
#warning Fast analog read not implemented on this platform
}

/** NOTE: Code needed to initialize the ADC for asynchronous reads. Typically involves setting up an interrupt handler for when conversion is done, and
 *  possibly calibration. */
void setupMozziADC(int8_t speed) {
#warning Fast analog read not implemented on this platform
}

/* NOTE: Most platforms call a specific function/ISR when conversion is complete. Provide this function, here.
 * From inside its body, simply call advanceADCStep(). E.g.:
void stm32_adc_eoc_handler() {
  advanceADCStep();
}
*/
////// END analog input code ////////
////// END analog input code ////////



//// BEGIN AUDIO OUTPUT code ///////
HardwareTimer audio_update_timer(AUDIO_UPDATE_TIMER);

#if (EXTERNAL_AUDIO_OUTPUT != true)
HardwareTimer *pwm_timer_ht;
PinName output_pin_1 = digitalPinToPinName(AUDIO_CHANNEL_1_PIN);
uint32_t pwm_timer_channel_1 = STM_PIN_CHANNEL(pinmap_function(output_pin_1, PinMap_TIM));

#  if (AUDIO_MODE == HIFI)
PinName output_pin_1_high = digitalPinToPinName(AUDIO_CHANNEL_1_PIN_HIGH);
uint32_t pwm_timer_channel_1_high = STM_PIN_CHANNEL(pinmap_function(output_pin_1_high, PinMap_TIM));
#  elif (AUDIO_CHANNELS > 1)
PinName output_pin_2 = digitalPinToPinName(AUDIO_CHANNEL_2);
uint32_t pwm_timer_channel_2 = STM_PIN_CHANNEL(pinmap_function(output_pin_2, PinMap_TIM));
#  endif

#include "AudioConfigSTM32.h"
inline void audioOutput(const AudioOutput f) {
#  if (AUDIO_MODE == HIFI)
  pwm_timer_ht->setCaptureCompare(pwm_timer_channel_1, (f.l()+AUDIO_BIAS) & ((1 << AUDIO_BITS_PER_CHANNEL) - 1));
  pwm_timer_ht->setCaptureCompare(pwm_timer_channel_1_high, (f.l()+AUDIO_BIAS) >> AUDIO_BITS_PER_CHANNEL);
#  else
  pwm_timer_ht->setCaptureCompare(pwm_timer_channel_1, f.l()+AUDIO_BIAS);
#    if (AUDIO_CHANNELS > 1)
  pwm_timer_ht->setCaptureCompare(pwm_timer_channel_2, f.r()+AUDIO_BIAS);
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
  audio_update_timer.setMode(/* channel */ 1, TIMER_OUTPUT_COMPARE);
  audio_update_timer.setCaptureCompare(/* channel */ 1, 1); // Interrupt 1 count after each update
  audio_update_timer.attachInterrupt(/* channel */ 1, defaultAudioOutput);
  audio_update_timer.refresh();

#if (EXTERNAL_AUDIO_OUTPUT != true)
  // Configure PWM output
  pinMode(AUDIO_CHANNEL_1_PIN, OUTPUT);
#  if (AUDIO_MODE == HIFI)
  pinMode(AUDIO_CHANNEL_1_PIN_HIGH, OUTPUT);
#  elif (AUDIO_CHANNELS > 1)
  pinMode(AUDIO_CHANNEL_2_PIN, OUTPUT);
#  endif

#  define MAX_CARRIER_FREQ (F_CPU / (1 << AUDIO_BITS_PER_CHANNEL))
  // static_assert(MAX_CARRIER_FREQ >= AUDIO_RATE); // Unfortunately, we cannot test this at compile time. F_CPU expands to a runtime variable
  TIM_TypeDef *pwm_timer_tim = (TIM_TypeDef *) pinmap_peripheral(output_pin_1, PinMap_TIM);
  pwm_timer_ht = new HardwareTimer(pwm_timer_tim);
  pwm_timer_ht->setMode(pwm_timer_channel_1, TIMER_OUTPUT_COMPARE_PWM1, output_pin_1);
#  if MAX_CARRIER_FREQ < (AUDIO_RATE * 5)
  // Generate as fast a carrier as possible
  pwm_timer_ht->setPrescaleFactor(1);
#  else
  // No point in generating arbitrarily high carrier frequencies. In fact, if
  // there _is_ any headroom, give the PWM pin more time to swing from HIGH to
  // LOW and BACK, cleanly
  pwm_timer_ht->setPrescaleFactor((int)MAX_CARRIER_FREQ / (AUDIO_RATE * 5)); // as fast as possible
#  endif
// Allocate enough room to write all intended bits
  pwm_timer_ht->setOverflow(1 << AUDIO_BITS_PER_CHANNEL);
  pwm_timer_ht->setCaptureCompare(pwm_timer_channel_1, AUDIO_BIAS /*, resolution */);

  pwm_timer_ht->resume();
#endif

  audio_update_timer.resume();
}

void stopMozzi() {
  audio_update_timer.pause();
}

//// END AUDIO OUTPUT code ///////
