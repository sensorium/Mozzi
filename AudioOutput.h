#ifndef AUDIOOUTPUT
#define AUDIOOUTPUT

#include "hardware_defines.h"
#include "mozzi_config.h"
//#include "MozziGuts.h"

#if IS_AVR() && (AUDIO_MODE == STANDARD_PLUS)
#define SCALE_AUDIO8(x) (x)
#define SCALE_AUDIO9(x) constrain(x,-244,243)
#define SCALE_AUDIO_ANY(x,bits) (bits > 8 ? x >> (bits - 8) : x << (8 - bits))
#else
#define SCALE_AUDIO8(x) (x << (AUDIO_BITS-8))
#define SCALE_AUDIO9(x) (x << (AUDIO_BITS-9))
#define SCALE_AUDIO_ANY(x,bits) (bits > AUDIO_BITS ? x >> (bits - AUDIO_BITS) : x << (AUDIO_BITS - bits))
#endif

#if (STEREO_HACK == true)
struct AudioOutput_t {
  /** Construct an audio frame from raw values (zero-centered) */
  AudioOutput_t(int16_t l, int16_t r) : l(l), r(r) {};
  int16_t l;
  int16_t r;
}
#else
/** Representation of an single audio output sample/frame. For mono output, this is really just a single zero-centered int,
 *  but for stereo it's a struct containing two ints.
 *
 *  While it may be tempting (and is possible) to use an int, directly, using AudioOutput_t and the functions AudioOutput::from8Bit(),
 *  AudioOutput::from9Bit(), or AudioOutput::fromNBits() will allow you to write code that will work across different platforms, even
 *  when those use a different output resolution.
 */
typedef AudioOutput_t int16_t;
#endif

namespace AudioOutput {
#if (STEREO_HACK == true)
  /** Construct an audio frame a zero-centered value known to be in the 8 bit range. On AVR, STANDARD_PLUS mode, this is effectively the same as calling the constructor,
   *  directly (no scaling gets applied). On platforms/configs using more bits, an appropriate left-shift will be performed. */
   AudioOutput_t from8Bit(int16_t l, int16_t r) { return AudioOutput_t(SCALE_AUDIO8(l), SCALE_AUDIO8(r)); }
  /** Construct an audio frame a zero-centered value known to be in the 9 bit range. On AVR, STANDARD_PLUS (where about 8.5 bits are usable), the value will not be shifted,
   *  but will be constrained() into the representable range. On platforms/configs using more bits, an appropriate left-shift (but not constrain) will be performed. */
  AudioOutput_t from9Bit(int16_t l, int16_t r) { return AudioOutput_t(SCALE_AUDIO9(l), SCALE_AUDIO9(r)); }
  /** Construct an audio frame a zero-centered value known to be in the N bit range. Appropriate left- or right-shifting will be performed, based on the number of output
   *  bits available. While this function takes care of the shifting, beware of potential overflow issues, if you intermediary results exceed the 16 bit range. Use proper
   *  casts to int32_t or larger in that case (and the compiler will automaticall pick the 32 bit overload in this case) */
  AudioOutput_t fromNBit(uint8_t bits, int16_t l, int16_t r) { return AudioOutput_t(SCALE_AUDIO_ANY(l, bits), SCALE_AUDIO_ANY(r, bits)); }
  /** 32bit overload. See above. */
  AudioOutput_t fromNBit(uint8_t bits, int32_t l, int32_t r) { return AudioOutput_t(SCALE_AUDIO_ANY(l, bits), SCALE_AUDIO_ANY(r, bits)); }
#else
  AudioOutput_t from8Bit(int16_t l) { return AudioOutput_t(SCALE_AUDIO8(l)); }
  AudioOutput_t from9Bit(int16_t l) { return AudioOutput_t(SCALE_AUDIO9(l)); }
  AudioOutput_t fromNBit(uint8_t bits, int16_t l) { return AudioOutput_t(SCALE_AUDIO_ANY(l, bits)); }
  AudioOutput_t fromNBit(uint8_t bits, int32_t l) { return AudioOutput_t(SCALE_AUDIO_ANY(l, bits)); }
#endif
};

inline void audioOutput(const AudioOutput_t f);
inline void bufferAudioOutput(const AudioOutput_t f);
inline bool canBufferAudioOutput();

#ifndef EXTERNAL_AUDIO_OUTPUT

// SAMD21
#if IS_SAMD21()
#include "AudioConfigSAMD21.h"
inline void audioOutput(const AudioOutput_t f)
{
  analogWrite(AUDIO_CHANNEL_1_PIN, f.l+AUDIO_BIAS);
}
#endif


// TEENSY3
#if IS_TEENSY3()
#include "AudioConfigTeensy3_12bit.h"
inline void audioOutput(const AudioOutput_t f)
{
  analogWrite(AUDIO_CHANNEL_1_PIN, f.l+AUDIO_BIAS);
}
#endif


// STM32
#if IS_STM32()
#include "AudioConfigSTM32.h"
inline void audioOutput(const AudioOutput_t f)
{
#if (AUDIO_MODE == HIFI)
  pwmWrite(AUDIO_CHANNEL_1_PIN, (f.l+AUDIO_BIAS) & ((1 << AUDIO_BITS_PER_CHANNEL) - 1));
  pwmWrite(AUDIO_CHANNEL_1_PIN_HIGH, (f.l+AUDIO_BIAS) >> AUDIO_BITS_PER_CHANNEL);
#else
  pwmWrite(AUDIO_CHANNEL_1_PIN, f.l+AUDIO_BIAS);
#if (STEREO_HACK == true)
  pwmWrite(AUDIO_CHANNEL_2_PIN, f.r+AUDIO_BIAS);
#endif
#endif
}
#endif




//AVR STANDARD
#if IS_AVR() && (AUDIO_MODE == STANDARD_PLUS)
#include "AudioConfigStandardPlus.h"
inline void audioOutput(const AudioOutput_t f)
{
  AUDIO_CHANNEL_1_OUTPUT_REGISTER = f.l+AUDIO_BIAS;
#if (STEREO_HACK == true)
  AUDIO_CHANNEL_2_OUTPUT_REGISTER = f.r+AUDIO_BIAS;
#endif
}



//AVR HIFI
#elif IS_AVR() && (AUDIO_MODE == HIFI)
#include "AudioConfigHiSpeed14bitPwm.h"
inline void audioOutput(const AudioOutput_t f)
{
  // read about dual pwm at
  // http://www.openmusiclabs.com/learning/digital/pwm-dac/dual-pwm-circuits/
  // sketches at http://wiki.openmusiclabs.com/wiki/PWMDAC,
  // http://wiki.openmusiclabs.com/wiki/MiniArDSP
  // if (!output_buffer.isEmpty()){
  //unsigned int out = output_buffer.read();
  // 14 bit, 7 bits on each pin
  // AUDIO_CHANNEL_1_highByte_REGISTER = out >> 7; // B00111111 10000000 becomes
  // B1111111
  // try to avoid looping over 7 shifts - need to check timing or disassemble to
  // see what really happens unsigned int out_high = out<<1; // B00111111
  // 10000000 becomes B01111111 00000000
  // AUDIO_CHANNEL_1_highByte_REGISTER = out_high >> 8; // B01111111 00000000
  // produces B01111111 AUDIO_CHANNEL_1_lowByte_REGISTER = out & 127;
  /* Atmega manual, p123
  The high byte (OCR1xH) has to be written first.
  When the high byte I/O location is written by the CPU,
  the TEMP Register will be updated by the value written.
  Then when the low byte (OCR1xL) is written to the lower eight bits,
  the high byte will be copied into the upper 8-bits of
  either the OCR1x buffer or OCR1x Compare Register in
  the same system clock cycle.
  */
  AUDIO_CHANNEL_1_highByte_REGISTER = (f.l+AUDIO_BIAS) >> AUDIO_BITS_PER_REGISTER;
  AUDIO_CHANNEL_1_lowByte_REGISTER = (f.l+AUDIO_BIAS) & ((1 << AUDIO_BITS_PER_REGISTER) - 1);
}
#endif


#else
#warning "Mozzi is configured to use an external void 'audioOutput(const AudioOutput_t f)' function. Please define one in your sketch"
#endif

#endif
