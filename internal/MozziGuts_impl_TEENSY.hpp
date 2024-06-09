/*
 * MozziGuts_impl_STM32duino.hpp
 *
 * This file is part of Mozzi.
 *
 * Copyright 2013-2024 Tim Barrass and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

#if !(IS_TEENSY3() || IS_TEENSY4())
#  error "Wrong implementation included for this platform"
#endif

#if (IS_TEENSY3() && F_CPU != 48000000)
#warning                                                                       \
    "Mozzi has been tested with a cpu clock speed of 16MHz on Arduino and 48MHz on Teensy 3!  Results may vary with other speeds."
#endif

namespace MozziPrivate {
////// BEGIN analog input code ////////
#if MOZZI_IS(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_STANDARD)
// required from http://github.com/pedvide/ADC for Teensy 3.*
} //namespace MozziPrivate
#include <ADC.h>
namespace MozziPrivate {

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

} // namespace MozziPrivate
void adc0_isr(void)
{
  MozziPrivate::advanceADCStep();
}
namespace MozziPrivate {

void setupMozziADC(int8_t speed) {
  adc = new ADC();
  adc->adc0->enableInterrupts(adc0_isr);  // TODO: is this even correct? Does the function take a parameter? And is adc0_isr a predefined name, or are we free to move this to MozziPrivate?
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
#endif  // MOZZI_ANALOG_READ

////// END  analog input code ////////



//// BEGIN AUDIO OUTPUT code ///////
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_INTERNAL_DAC, MOZZI_OUTPUT_EXTERNAL_TIMED)
} // namespace MozziPrivate
#include "IntervalTimer.h"
namespace MozziPrivate {
IntervalTimer timer1;
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_INTERNAL_DAC)
inline void audioOutput(const AudioOutput f) {
  analogWrite(MOZZI_AUDIO_PIN_1, f.l()+MOZZI_AUDIO_BIAS);
#  if (MOZZI_AUDIO_CHANNELS > 1)
  analogWrite(MOZZI_AUDIO_PIN_2, f.r()+MOZZI_AUDIO_BIAS);
#  endif
}
#endif

static void startAudio() {
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_INTERNAL_DAC)
#  if IS_TEENSY3()
  analogWriteResolution(12);
#  elif IS_TEENSY4()
  analogWriteResolution(10);
  analogWriteFrequency(MOZZI_AUDIO_PIN_1, 146484.38f);
#    if (MOZZI_AUDIO_CHANNELS > 1)
  analogWriteFrequency(MOZZI_AUDIO_PIN_2, 146484.38f);
#    endif  // end #if (MOZZI_AUDIO_CHANNELS > 1)
#  endif // TEENSY3/4
#endif
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_INTERNAL_DAC, MOZZI_OUTPUT_EXTERNAL_TIMED)
  timer1.begin(defaultAudioOutput, 1000000. / MOZZI_AUDIO_RATE);
#endif
}

void stopMozzi() {
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_INTERNAL_DAC, MOZZI_OUTPUT_EXTERNAL_TIMED)
  timer1.end();
#endif
  interrupts();
}
//// END AUDIO OUTPUT code ///////

//// BEGIN Random seeding ////////
void MozziRandPrivate::autoSeed() {
#warning Automatic random seeding is not implemented on this platform
}
//// END Random seeding ////////

} // namespace MozziPrivate
