/*
 * MozziGuts_impl_STM32.hpp
 *
 * This file is part of Mozzi.
 *
 * Copyright 2020-2024 Thomas Friedrichsmeier and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

#include "HardwareTimer.h"

namespace MozziPrivate {

////// BEGIN analog input code ////////
#if MOZZI_IS(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_STANDARD)

} // namespace MozziPrivate
//#include <STM32ADC.h>  // Disabled, here. See hardware_defines.h
namespace MozziPrivate {

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

void setupMozziADC(int8_t speed) {
  adc.attachInterrupt(stm32_adc_eoc_handler);
}


inline uint8_t STM32PinMap(uint8_t pin)
{  
  if (pin > 15) return pin-8;
  else return pin;
}

void setupFastAnalogRead(int8_t speed) {
  // NOTE: These picks are pretty arbitrary. Further available options are 7_5, 28_5, 55_5, 71_5 and 239_5 (i.e. 7.5 ADC cylces, etc.)
  if (speed == FASTEST_ADC) adc.setSampleRate(ADC_SMPR_1_5);
  else if (speed == FASTER_ADC) adc.setSampleRate(ADC_SMPR_13_5);
  else (adc.setSampleRate(ADC_SMPR_41_5));
}
#endif

////// END analog input code ////////



//// BEGIN AUDIO OUTPUT code ///////
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED)
HardwareTimer audio_update_timer(MOZZI_AUDIO_UPDATE_TIMER);
#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_2PIN_PWM)
HardwareTimer audio_update_timer(MOZZI_AUDIO_UPDATE_TIMER);
HardwareTimer audio_pwm_timer(MOZZI_AUDIO_PWM_TIMER);

inline void audioOutput(const AudioOutput f) {
#  if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_2PIN_PWM)
  pwmWrite(MOZZI_AUDIO_PIN_1, (f.l()+MOZZI_AUDIO_BIAS) >> MOZZI_AUDIO_BITS_PER_CHANNEL);
  pwmWrite(MOZZI_AUDIO_PIN_1_LOW, (f.l()+MOZZI_AUDIO_BIAS) & ((1 << MOZZI_AUDIO_BITS_PER_CHANNEL) - 1));
#  else
  pwmWrite(MOZZI_AUDIO_PIN_1, f.l()+MOZZI_AUDIO_BIAS);
#    if (MOZZI_AUDIO_CHANNELS > 1)
  pwmWrite(MOZZI_AUDIO_PIN_2, f.r()+MOZZI_AUDIO_BIAS);
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
  audio_update_timer.setMode(TIMER_CH1, TIMER_OUTPUT_COMPARE);
  audio_update_timer.setCompare(TIMER_CH1,
                                1); // Interrupt 1 count after each update
  audio_update_timer.attachInterrupt(TIMER_CH1, defaultAudioOutput);
  audio_update_timer.refresh();
  audio_update_timer.resume();
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_2PIN_PWM)
  pinMode(MOZZI_AUDIO_PIN_1, PWM);
#  if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_2PIN_PWM)
  pinMode(MOZZI_AUDIO_PIN_1_LOW, PWM);
#  elif (MOZZI_AUDIO_CHANNELS > 1)
  pinMode(MOZZI_AUDIO_PIN_2, PWM);
#  endif

#  define MAX_CARRIER_FREQ (F_CPU / (1 << MOZZI_AUDIO_BITS_PER_CHANNEL))
#  if MAX_CARRIER_FREQ < MOZZI_AUDIO_RATE
#    error Configured audio resolution is definitely too high at the configured audio rate (and the given CPU speed)
#  elif MAX_CARRIER_FREQ < (MOZZI_AUDIO_RATE * 3)
#    warning Configured audio resolution may be higher than optimal at the configured audio rate (and the given CPU speed)
#  endif

#  if MAX_CARRIER_FREQ < (MOZZI_AUDIO_RATE * 5)
  // Generate as fast a carrier as possible
  audio_pwm_timer.setPrescaleFactor(1);
#  else
  // No point in generating arbitrarily high carrier frequencies. In fact, if
  // there _is_ any headroom, give the PWM pin more time to swing from HIGH to
  // LOW and BACK, cleanly
  audio_pwm_timer.setPrescaleFactor((int)MAX_CARRIER_FREQ / (MOZZI_AUDIO_RATE * 5));
#  endif
  audio_pwm_timer.setOverflow(
      1 << MOZZI_AUDIO_BITS_PER_CHANNEL); // Allocate enough room to write all
                                    // intended bits
#  undef MAX_CARRIER_FREQ // no longer needed
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
  // Unfortunately the internal temp sensor on STM32s does _not_ appear to create a lot of noise.
  // Ironically, the calls to calibrate help induce some random noise. You're still fairly likely to produce two equal
  // random seeds in two subsequent runs, however.
  adc.enableInternalReading();
  union {
    float cf;
    uint32_t ci;
  } conv;
  conv.cf = adc.readTemp();
  x=x^conv.ci;
  adc.calibrate();
  conv.cf = adc.readTemp();
  y=y^conv.ci;
  adc.calibrate();
  conv.cf = adc.readTemp();
  z=z^conv.ci;
}
//// END Random seeding ////////

} // namespace MozziPrivate
