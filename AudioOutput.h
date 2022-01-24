/** @file AudioOutput
 *
 * Platform independent audio output and adding support for new platforms or output methods.
 *
 * Mozzi provides support for audio ouput on a range of different boards and CPUs. This page is about the following related topics:
 *
 *  - adding a custom output method (importantly using external DACs) to your sketch
 *  - writing sketches that will work on different platforms / with different output methods
 *  - extending Mozzi for a new architecture
 *
 * For all of these topics, it is helpful to have a basic understanding of the basic output steps in Mozzi:
 *
 * 1. Inside the loop() function in your sketch you call audioHook(). This function is responsible for calling updateAudio(), whenever there is room in the output buffer,
 *    adding the generated sample to the output buffer, calling updateControl() at an appropriate rate, and a few other details that are not important for this discussion.
 *
 * 2. A platform-specific timer is triggered at audio rate (usually), takes a sample from the output buffer and sends it to audioOutput().
 *
 * 3. The audioOutput() function - usually predefined inside Mozzi - takes care of sending the sample to the hardware.
 *
 * This basic output pipeline can be customized in several ways. First, defining EXTERNAL_AUDIO_OUTPUT to true in mozzi_config.h will allow you to define your own audioOutput()
 * fuction. The library ships with some sample sketches for output to external DACs using this mechanism.
 *
 * In some cases, you will additionally want to bypass Mozzis output buffer, for example, if your board, or your external DAC already comes with an efficient built-in buffer.
 * In this case, define BYPASS_MOZZI_OUTPUT_BUFFER to true. You will then have to provide a custom definition of canBufferAudioOutput(), returning true whenever your hardware
 * is ready toaccept a new sample of output. This is called from inside audioHook(), and whenever there is room for a new sample, it is generated and sent to audioOutput(),
 * immediately. In this case, should you need a timer running at AUDIO_RATE, you will have to set up one, yourself, if needed.
 *
 * In custom code, setting BYPASS_MOZZI_OUTPUT_BUFFER does not make much sense without EXTERNAL_AUDIO_OUTPUT also set to true. However, some platform ports (e.g. ESP32) actually
 * use this combination, internally.
 *
 * Different output methods often support a different resolution of output samples. To provide best performance on slow boards, Mozzi expects your updateAudio() function to
 * return samples in exactly the width that is needed at the output stage. Thus, defining this naively, an updateAudio() function designed for 8 bit output will produce very
 * low volume output on a 16 bit DAC, while the other way around overflows will result in way too loud and heavily distored output. Fortunately, all that is needed to write
 * portable sketches is to specify how many bits your updateAudio() function provides. The (inline) functions in the AudioOutput namespace do just that. Using them makes sure
 * your audio output is shifted if, and as much as needed on all platforms.
 */

#ifndef AUDIOOUTPUT
#define AUDIOOUTPUT

#include "MozziGuts.h"

/** The type used to store a single channel of a single frame, internally. For compatibility with earlier versions of Mozzi this is defined as int.
 *  If you do not care about keeping old sketches working, you may be able to save some RAM by using int16_t, instead (on boards where int is larger
 *  than 16 bits). */
#define AudioOutputStorage_t int

#if IS_AVR() && ((AUDIO_MODE == STANDARD_PLUS) || (AUDIO_MODE == STANDARD))
#define SCALE_AUDIO(x,bits) (bits > 8 ? (x) >> (bits - 8) : (x) << (8 - bits))
#define SCALE_AUDIO_NEAR(x,bits) (bits > 9 ? (x) >> (bits - 9) : (x) << (9 - bits))
#define CLIP_AUDIO(x) constrain((x), -244,243)
#else
#define SCALE_AUDIO(x,bits) (bits > AUDIO_BITS ? (x) >> (bits - AUDIO_BITS) : (x) << (AUDIO_BITS - bits))
#define SCALE_AUDIO_NEAR(x,bits) SCALE_AUDIO(x,bits)
#define CLIP_AUDIO(x) constrain((x), (-(AudioOutputStorage_t) AUDIO_BIAS), (AudioOutputStorage_t) AUDIO_BIAS-1)
#endif

#if (AUDIO_CHANNELS == STEREO)
#define AudioOutput StereoOutput
#if (STEREO_HACK == true)
#define AudioOutput_t void
#else
#define AudioOutput_t StereoOutput
#endif
#else
/** Representation of an single audio output sample/frame. For mono output, this is really just a single zero-centered int,
 *  but for stereo it's a struct containing two ints.
 *
 *  While it may be tempting (and is possible) to use an int, directly, using AudioOutput_t and the functions AudioOutput::from8Bit(),
 *  AudioOutput::fromNBit(), or AudioOutput::fromAlmostNBit() will allow you to write code that will work across different platforms, even
 *  when those use a different output resolution.
 *
 *  @note The only place where you should be using AudioOutput_t, directly, is in your updateAudio() function signature. It's really just a
 *        dummy used to make the "same" function signature work across different configurations (importantly mono/stereo). It's true value
 *        might be subject to change, and it may even be void. Use either MonoOutput or StereoOutput to represent a piece of audio output.
 */
#define AudioOutput_t AudioOutputStorage_t
/** Representation of an single audio output sample/frame. This #define maps to either MonoOutput or StereoOutput, depending on what is configured
 *  in mozzi_config.h. Since the two are source compatible to a large degree, it often isn't even necessary to test, which it is, in your code. E.g.
 *  both have functions l() and r(), to return "two" audio channels (which will be the same in case of mono).
 *
 *  You will not usually use or encounter this definition, unless using EXTERNAL_AUDIO_OUTPUT.
 */
#define AudioOutput MonoOutput
#endif

/** This struct encapsulates one frame of mono audio output. Internally, it really just boils down to two int values, but the struct provides
 *  useful API an top of that. For more detail @see MonoOutput . */
struct StereoOutput {
  /** Construct an audio frame from raw values (zero-centered) */
  StereoOutput(AudioOutputStorage_t l, AudioOutputStorage_t r) : _l(l), _r(r) {};
  /** Default contstructor */
  StereoOutput() : _l(0), _r(0) {};
#if (AUDIO_CHANNELS != STEREO)
  /** Conversion to int operator: If used in a mono config, returns only the left channel (and gives a compile time warning). 
      This _could_ be turned into an operator for implicit conversion in this case. For now we chose to apply conversion on demand, only, as most of the time
      using StereoOutput in a mono config, is not intended. */
  inline AudioOutput_t portable() const __attribute__((deprecated("Sketch generates stereo output, but Mozzi is configured for mono. Check mozzi_config.h."))) { return _l; };
#endif
  AudioOutputStorage_t l() const { return _l; };
  AudioOutputStorage_t r() const { return _r; };
  /** @see MonoOutput::clip(). Clips both channels. */
  StereoOutput& clip() { _l = CLIP_AUDIO(_l); _r = CLIP_AUDIO(_r); return *this; };

  /** @see MonoOutput::fromNBit(), stereo variant */
  static inline StereoOutput fromNBit(uint8_t bits, int16_t l, int16_t r) { return StereoOutput(SCALE_AUDIO(l, bits), SCALE_AUDIO(r, bits)); }
  /** @see MonoOutput::fromNBit(), stereo variant, 32 bit overload */
  static inline StereoOutput fromNBit(uint8_t bits, int32_t l, int32_t r) { return StereoOutput(SCALE_AUDIO(l, bits), SCALE_AUDIO(r, bits)); }
  /** @see MonoOutput::from8Bit(), stereo variant */
  static inline StereoOutput from8Bit(int16_t l, int16_t r) { return fromNBit(8, l, r); }
  /** @see MonoOutput::from16Bit(), stereo variant */
  static inline StereoOutput from16Bit(int16_t l, int16_t r) { return fromNBit(16, l, r); }
  /** @see MonoOutput::fromAlmostNBit(), stereo variant */
  static inline StereoOutput fromAlmostNBit(uint8_t bits, int16_t l, int16_t r) { return StereoOutput(SCALE_AUDIO_NEAR(l, bits), SCALE_AUDIO_NEAR(r, bits)); }
  /** @see MonoOutput::fromAlmostNBit(), stereo variant, 32 bit overload */
  static inline StereoOutput fromAlmostNBit(uint8_t bits, int32_t l, int32_t r) { return StereoOutput(SCALE_AUDIO_NEAR(l, bits), SCALE_AUDIO_NEAR(r, bits)); }
private:
  AudioOutputStorage_t _l;
  AudioOutputStorage_t _r;
};

/** This struct encapsulates one frame of mono audio output. Internally, it really just boils down to a single int value, but the struct provides
 *  useful API an top of that, for the following:
 *
 * a) To construct an output frame, you should use one of the from8Bit(), fromNBit(), etc. functions. Given a raw input value, at a known resolution (number of bits), 
 *    this scales the output efficiently to whatever is needed on the target platform. Using this, your updateAudio() function will be portable across different CPU and
 *    different output methods, including external DACs.
 * b) The struct provides some convenience API on top of this. Right now, this is the function clip(), replacing the more verbose, and non-portable constrain(x, -244, 243)
 *    found in some old sketches.
 * c) The struct provides accessors l() and r() that are source-compatible with StereoOutput, making it easy to e.g. implement support for an external DAC in both mono
 *    and stereo.
 * d) Finally, an automatic conversion operator to int aka AudioOutput_t provides backward compatibility with old Mozzi sketches. Internally, the compiler will actually
 *    do away with this wholw struct, leaving just the same basic fast integer operations as in older Mozzi sketches. However, now, you don't have to rewrite those for
 *    different configurations.
 */
struct MonoOutput {
  /** Construct an audio frame from raw values (zero-centered) */
  MonoOutput(AudioOutputStorage_t l=0) : _l(l) {};
#if (AUDIO_CHANNELS > 1)
  /** Conversion to stereo operator: If used in a stereo config, returns identical channels (and gives a compile time warning).
      This _could_ be turned into an operator for implicit conversion in this case. For now we chose to apply conversion on demand, only, as most of the time
      using StereoOutput in a mono config, is not intended. */
  inline StereoOutput portable() const __attribute__((deprecated("Sketch generates mono output, but Mozzi is configured for stereo. Check mozzi_config.h."))) { return StereoOutput(_l, _l); };
#else
  /** Conversion to int operator. */
  inline operator AudioOutput_t() const { return _l; };
#endif
  AudioOutputStorage_t l() const { return _l; };
  AudioOutputStorage_t r() const { return _l; };
  /** Clip frame to supported range. This is useful when at times, but only rarely, the signal may exceed the usual range. Using this function does not avoid
   *  artifacts, entirely, but gives much better results than an overflow. */
  MonoOutput& clip() { _l = CLIP_AUDIO(_l); return *this; };

  /** Construct an audio frame a zero-centered value known to be in the N bit range. Appropriate left- or right-shifting will be performed, based on the number of output
   *  bits available. While this function takes care of the shifting, beware of potential overflow issues, if your intermediary results exceed the 16 bit range. Use proper
   *  casts to int32_t or larger in that case (and the compiler will automatically pick the 32 bit overload in this case) */
  static inline MonoOutput fromNBit(uint8_t bits, int16_t l) { return MonoOutput(SCALE_AUDIO(l, bits)); }
  /** 32bit overload. See above. */
  static inline MonoOutput fromNBit(uint8_t bits, int32_t l) { return MonoOutput(SCALE_AUDIO(l, bits)); }
  /** Construct an audio frame from a zero-centered value known to be in the 8 bit range. On AVR, STANDADR or STANDARD_PLUS mode, this is effectively the same as calling the
   * constructor, directly (no scaling gets applied). On platforms/configs using more bits, an appropriate left-shift will be performed. */
  static inline MonoOutput from8Bit(int16_t l) { return fromNBit(8, l); }
  /** Construct an audio frame a zero-centered value known to be in the 16 bit range. This is jsut a shortcut for fromNBit(16, ...) provided for convenience. */
  static inline MonoOutput from16Bit(int16_t l) { return fromNBit(16, l); }
  /** Construct an audio frame a zero-centered value known to be above at almost but not quite the N bit range, e.g. at N=8 bits and a litte. On most platforms, this is
   *  exactly the same as fromNBit(), shifting up or down to the platforms' available resolution.
   *
   *  However, on AVR, STANDARD(_PLUS) (where about 8.5 bits are usable), the value will be shifted to the (almost) 9 bit range, instead of to the 8 bit range. allowing to
   *  make use of that extra half bit of resolution. In many cases it is useful to follow up this call with clip().
   *
   *  @example fromAlmostNBit(10, oscilA.next() + oscilB.next() + oscilC.next());
   */
  static inline MonoOutput fromAlmostNBit(uint8_t bits, int16_t l) { return MonoOutput(SCALE_AUDIO_NEAR(l, bits)); }
  /** 32bit overload. See above. */
  static inline MonoOutput fromAlmostNBit(uint8_t bits, int32_t l) { return MonoOutput(SCALE_AUDIO_NEAR(l, bits)); }
private:
  AudioOutputStorage_t _l;
};


/** When setting EXTERNAL_AUDIO_OUTPUT to true, implement this function to take care of writing samples to the hardware. */
void audioOutput(const AudioOutput f);
#if BYPASS_MOZZI_OUTPUT_BUFFER
/** When setting BYPASS_MOZZI_OUTPUT_BUFFER to true, implement this function to return true, if and only if your hardware (or custom buffer) is ready to accept the next sample. */
inline bool canBufferAudioOutput();
#endif

/** Perform one step of (fast) pdm encoding, returning 8 "bits" (i.e. 8 ones and zeros).
 *  You will usually call this at least four or eight times for a single input sample.
 *
 *  The return type is defined as uint32_t to avoid conversion steps. Actually, only the 8 lowest
 *  bits of the return value are set. */
inline uint32_t pdmCode8(uint16_t sample) {
  // lookup table for fast pdm coding on 8 output bits at a time
  static const byte fast_pdm_table[]{0,  0b00010000, 0b01000100,
                             0b10010010, 0b10101010, 0b10110101,
                             0b11011101, 0b11110111, 0b11111111};

  static uint32_t lastwritten = 0;
  static uint32_t nexttarget = 0;
  // in each iteration, code the highest 3-and-a-little bits.
  // Note that sample only has 16 bits, while the
  // highest bit we consider for writing is bit 17.
  // Thus, if the highest bit is set, the next
  // three bits cannot be.
  nexttarget += sample;
  nexttarget -= lastwritten;
  lastwritten = nexttarget & 0b11110000000000000;
  return fast_pdm_table[lastwritten >> 13];
}

/** Convenience function to perform four iterations of pdmCode8() */
inline uint32_t pdmCode32(uint16_t sample) {
  uint32_t outbits = 0;
  for (uint8_t i = 0; i < 4; ++i) {
    outbits = outbits << 8;
    outbits |= pdmCode8(sample);
  }
  return outbits;
}

#if (EXTERNAL_AUDIO_OUTPUT != true)

///////////////////// SAMD21
#if IS_SAMD21()
#include "AudioConfigSAMD21.h"
inline void audioOutput(const AudioOutput f)
{
  analogWrite(AUDIO_CHANNEL_1_PIN, f.l()+AUDIO_BIAS);
}
#endif


///////////////////// TEENSY3
#if IS_TEENSY3()
#include "AudioConfigTeensy3_12bit.h"
inline void audioOutput(const AudioOutput f)
{
  analogWrite(AUDIO_CHANNEL_1_PIN, f.l()+AUDIO_BIAS);
}
#endif


///////////////////// STM32
#if IS_STM32()
#include "AudioConfigSTM32.h"
inline void audioOutput(const AudioOutput f)
{
#if (AUDIO_MODE == HIFI)
  pwmWrite(AUDIO_CHANNEL_1_PIN, (f.l()+AUDIO_BIAS) & ((1 << AUDIO_BITS_PER_CHANNEL) - 1));
  pwmWrite(AUDIO_CHANNEL_1_PIN_HIGH, (f.l()+AUDIO_BIAS) >> AUDIO_BITS_PER_CHANNEL);
#else
  pwmWrite(AUDIO_CHANNEL_1_PIN, f.l()+AUDIO_BIAS);
#if (AUDIO_CHANNELS > 1)
  pwmWrite(AUDIO_CHANNEL_2_PIN, f.r()+AUDIO_BIAS);
#endif
#endif
}
#endif


///////////////////// ESP8266
#if IS_ESP8266()
#include "AudioConfigESP.h"
#if (ESP_AUDIO_OUT_MODE == PDM_VIA_I2S)
#include <i2s.h>
inline bool canBufferAudioOutput() {
  return (i2s_available() >= PDM_RESOLUTION);
}
inline void audioOutput(const AudioOutput f) {
  for (uint8_t words = 0; words < PDM_RESOLUTION; ++words) {
    i2s_write_sample(pdmCode32(f.l()));
  }
}
#elif (ESP_AUDIO_OUT_MODE == EXTERNAL_DAC_VIA_I2S)
#include <i2s.h>
inline bool canBufferAudioOutput() {
  return (i2s_available() >= PDM_RESOLUTION);
}
inline void audioOutput(const AudioOutput f) {
  i2s_write_lr(f.l(), f.r());  // Note: i2s_write expects zero-centered output
}
#else
inline void audioOutput(const AudioOutput f) {
  // optimized version of: Serial1.write(...);
  for (uint8_t i = 0; i < PDM_RESOLUTION*4; ++i) {
    U1F = pdmCode8(f);
  }
}
#endif
#endif



///////////////////// ESP32
#if IS_ESP32()
#include "AudioConfigESP32.h"
// On ESP32 we cannot test wether the DMA buffer has room. Instead, we have to use a one-sample mini buffer. In each iteration we
// _try_ to write that sample to the DMA buffer, and if successful, we can buffer the next sample. Somewhat cumbersome, but works.
// TODO: Should ESP32 gain an implemenation of i2s_available(), we should switch to using that, instead.
static bool _esp32_can_buffer_next = true;
#if (ESP32_AUDIO_OUT_MODE == INTERNAL_DAC)
static uint16_t _esp32_prev_sample[2];
#define ESP_SAMPLE_SIZE (2*sizeof(uint16_t))
#elif (ESP32_AUDIO_OUT_MODE == PT8211_DAC)
static int16_t _esp32_prev_sample[2];
#define ESP_SAMPLE_SIZE (2*sizeof(int16_t))
#elif (ESP32_AUDIO_OUT_MODE == PDM_VIA_I2S)
static uint32_t _esp32_prev_sample[PDM_RESOLUTION];
#define ESP_SAMPLE_SIZE (PDM_RESOLUTION*sizeof(uint32_t))
#endif

inline bool esp32_tryWriteSample() {
  size_t bytes_written;
  i2s_write(i2s_num, &_esp32_prev_sample, ESP_SAMPLE_SIZE, &bytes_written, 0);
  return (bytes_written != 0);
}

inline bool canBufferAudioOutput() {
  if (_esp32_can_buffer_next) return true;
  _esp32_can_buffer_next = esp32_tryWriteSample();
  return _esp32_can_buffer_next;
}

inline void audioOutput(const AudioOutput f) {
#if (ESP32_AUDIO_OUT_MODE == INTERNAL_DAC)
  _esp32_prev_sample[0] = (f.l() + AUDIO_BIAS) << 8;
#if (AUDIO_CHANNELS > 1)
  _esp32_prev_sample[1] = (f.r() + AUDIO_BIAS) << 8;
#else
  // For simplicity of code, even in mono, we're writing stereo samples
  _esp32_prev_sample[1] = _esp32_prev_sample[0];
#endif
#elif (ESP32_AUDIO_OUT_MODE == PDM_VIA_I2S)
  for (uint8_t i=0; i<PDM_RESOLUTION; ++i) {
    _esp32_prev_sample[i] = pdmCode32(f.l() + AUDIO_BIAS);
  }
#else
  // PT8211 takes signed samples
  _esp32_prev_sample[0] = f.l();
  _esp32_prev_sample[1] = f.r();
#endif
  _esp32_can_buffer_next = esp32_tryWriteSample();
}

#endif



///////////////////// AVR STANDARD
#if IS_AVR() && (AUDIO_MODE == STANDARD_PLUS)
#include "AudioConfigStandardPlus.h"
inline void audioOutput(const AudioOutput f)
{
  AUDIO_CHANNEL_1_OUTPUT_REGISTER = f.l()+AUDIO_BIAS;
#if (AUDIO_CHANNELS > 1)
  AUDIO_CHANNEL_2_OUTPUT_REGISTER = f.r()+AUDIO_BIAS;
#endif
}



///////////////////// AVR HIFI
#elif IS_AVR() && (AUDIO_MODE == HIFI)
#include "AudioConfigHiSpeed14bitPwm.h"
inline void audioOutput(const AudioOutput f)
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
  AUDIO_CHANNEL_1_highByte_REGISTER = (f.l()+AUDIO_BIAS) >> AUDIO_BITS_PER_REGISTER;
  AUDIO_CHANNEL_1_lowByte_REGISTER = (f.l()+AUDIO_BIAS) & ((1 << AUDIO_BITS_PER_REGISTER) - 1);
}
#endif


#else
#warning "Mozzi is configured to use an external void 'audioOutput(const AudioOutput f)' function. Please define one in your sketch"
#endif

#endif
