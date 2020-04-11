#include"AudioOutput.h"
#include "mozzi_config.h"

#ifndef EXTERNAL_AUDIO_OUTPUT

// SAMD21
#if IS_SAMD21()
#include "AudioConfigSAMD21.h"
void audioOutput(int l, int r)
{
  analogWrite(AUDIO_CHANNEL_1_PIN, l);
}
#endif


// TEENSY3
#if IS_TEENSY3()
#include "AudioConfigTeensy3_12bit.h"
void audioOutput(int l, int r)
{
  analogWrite(AUDIO_CHANNEL_1_PIN, l);
}
#endif


// STM32
#if IS_STM32()
#warning "in stm"
#if (AUDIO_MODE == HIFI)
#warning "HIFI"
#endif
#include "AudioConfigSTM32.h"
void audioOutput(int l, int r)
{

#if (AUDIO_MODE == HIFI)
  pwmWrite(AUDIO_CHANNEL_1_PIN, l & ((1 << AUDIO_BITS_PER_CHANNEL) - 1));
  pwmWrite(AUDIO_CHANNEL_1_PIN_HIGH, l >> AUDIO_BITS_PER_CHANNEL);
#else
  pwmWrite(AUDIO_CHANNEL_1_PIN, l);
#if (STEREO_HACK == true)
  pwmWrite(AUDIO_CHANNEL_2_PIN, r);
#endif
#endif
}
#endif




//AVR STANDARD
#if IS_AVR() && (AUDIO_MODE == STANDARD_PLUS)
#include "AudioConfigStandardPlus.h"
void audioOutput(int l, int r)
{
  AUDIO_CHANNEL_1_OUTPUT_REGISTER = l;
#if (STEREO_HACK == true)
  AUDIO_CHANNEL_2_OUTPUT_REGISTER = r;
#endif
}



//AVR HIFI
#elif IS_AVR() && (AUDIO_MODE == HIFI)
#include "AudioConfigHiSpeed14bitPwm.h"
void audioOutput(int l, int r)
{
  AUDIO_CHANNEL_1_highByte_REGISTER = l >> AUDIO_BITS_PER_REGISTER;
  AUDIO_CHANNEL_1_lowByte_REGISTER = l & ((1 << AUDIO_BITS_PER_REGISTER) - 1);
}
#endif


#else
#warning "Mozzi is configured to use an external void 'audioOutput(int l, int r)' function. Please define one in your sketch"
#endif
