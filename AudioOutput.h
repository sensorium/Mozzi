/*
 * AudioOutput.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2021-2024 Thomas Friedrichsmeier and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
 */

/** @defgroup audio_output Audio Output and Buffering
 *
 * @details Documentation on basic Mozzi architecture and output modes */

/** @ingroup audio_output
 * @page mozzi_audio_output_architecture Basic architecture of audio generation, buffering, and output in Mozzi
 *
 * Mozzi provides support for audio ouput on a range of different boards and CPUs. This page is about the following related topics:
 *
 *  - adding a custom output method (importantly using external DACs) to your sketch
 *  - writing sketches that will work on different platforms / with different output methods
 *  - extending Mozzi for a new architecture
 *
 * For all of these topics, it is helpful to have a basic understanding of the basic output steps in Mozzi:
 *
 * 1. Inside the loop() function in your sketch you call audioHook().
 * 1a. If the audio output buffer is currently filled, this does nothing.
 * 1b. Otherwise, this calls updateAudio(). The generated sample is then added to the audio output buffer. (Also, updateControl() will be called at an appropriate rate,
 *     and a few other details that are not important for this discussion.)
 *
 * 2. A platform-specific timer is triggered at audio rate (usually), takes a sample from the output buffer and sends it to audioOutput().
 *
 * 3. The audioOutput() function - usually predefined inside Mozzi - takes care of sending the sample to the hardware.
 *
 * These output steps are not always followed, however. Firstly, when using @ref external_audio output, the audioOutput() funtion is supplied by the user sketch,
 * instead of Mozzi. @ref external_audio output.
 *
 * Some ports will also want to bypass the Mozzi audio output buffer. For instance, an internal DAC may be addressable via an efficient DMA-connected
 * buffer, already, and also have a built-in rate control. In this case, ports will internally set the define @ref BYPASS_MOZZI_OUTPUT_BUFFER to true. Such a port will
 * have to provide a custom definition of canBufferAudioOutput(), returning true, whenever a new sample of output can be accepted. No timer at audio-rate is set up in this
 * case.
 *
 * Finally, the @ref external_audio output mode (@ref MOZZI_AUDIO_MODE MOZZI_OUTPUT_EXTERNAL_CUSTOM) is essentially a combination of the two. Here, the user sketch needs to provide
 * both audioOutput() and canBufferAudioOutput(). The latter is again called from audioHook(), and whenever it returns true, a new sample is generated and passed to
 * audioOutput().
 *
 * @section audio_shifting Platform specific audio resolution
 * Different output methods often support a different resolution of output samples. To provide best performance on slow boards, Mozzi expects your updateAudio() function to
 * return samples in exactly the width that is needed at the output stage. Thus, defining this naively, an updateAudio() function designed for 8 bit output will produce very
 * low volume output on a 16 bit DAC, while the other way around overflows will result in way too loud and heavily distored output. Fortunately, all that is needed to write
 * portable sketches is to specify how many bits your updateAudio() function provides. The (inline) functions in the AudioOutput namespace do just that. Using them makes sure
 * your audio output is shifted if, and as much as needed on all platforms.
 *
 * @see MonoOutput::fromNBit(), StereoOutput::fromNBit()
 */

#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H
#include <FixMath.h>

/** The type used to store a single channel of a single frame, internally. For compatibility with earlier versions of Mozzi this is defined as int.
 *  If you do not care about keeping old sketches working, you may be able to save some RAM by using int16_t, instead (on boards where int is larger
 *  than 16 bits). */
#define AudioOutputStorage_t int

template<typename T> constexpr AudioOutputStorage_t SCALE_AUDIO(T x, byte bits) { return (bits > MOZZI_AUDIO_BITS ? (x) >> (bits - MOZZI_AUDIO_BITS) : (x) << (MOZZI_AUDIO_BITS - bits)); }
template<typename T> constexpr AudioOutputStorage_t SCALE_AUDIO_NEAR(T x, byte bits) { return (bits > MOZZI_AUDIO_BITS_OPTIMISTIC ? (x) >> (bits - MOZZI_AUDIO_BITS_OPTIMISTIC) : (x) << (MOZZI_AUDIO_BITS_OPTIMISTIC - bits)); }
template<typename T> constexpr AudioOutputStorage_t CLIP_AUDIO(T x) { return (constrain((x), (-(AudioOutputStorage_t) MOZZI_AUDIO_BIAS), (AudioOutputStorage_t) (MOZZI_AUDIO_BIAS-1))); }

struct MonoOutput;
struct StereoOutput;

#if MOZZI_IS(MOZZI_AUDIO_CHANNELS, MOZZI_STEREO)
typedef StereoOutput AudioOutput;
#else
/** Representation of an single audio output sample/frame. This typedef maps to either MonoOutput or StereoOutput, depending on what is configured
 *  in MOZZI_AUDIO_CHANNELS. Since the two are source compatible to a large degree, it often isn't even necessary to test, which it is, in your code. E.g.
 *  both have functions l() and r(), to return "two" audio channels (which will be the same in case of mono).
 *
 *  You will not usually use or encounter this definition, unless using @ref external_audio output mode.
 */
typedef MonoOutput AudioOutput;
#endif

#if MOZZI_COMPATIBILITY_LEVEL < MOZZI_COMPATIBILITY_LATEST
#if (MOZZI_COMPATIBILITY_LEVEL <= MOZZI_COMPATIBILITY_1_1) && MOZZI_IS(MOZZI_AUDIO_CHANNELS, MOZZI_MONO)
typedef int AudioOutput_t;  // Note: Needed for pre 1.1 backwards compatibility
#else
/** Transitory alias to AudioOutput. The only point of this typedef is to keep old code working. In new code,
 *  use AudioOutput, directly, instead.
*/
MOZZI_DEPRECATED("2.0", "Replace AudioOutput_t with simple AudioOutput") typedef AudioOutput AudioOutput_t;
#endif
#endif

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
 *    do away with this whole struct, leaving just the same basic fast integer operations as in older Mozzi sketches. However, now, you don't have to rewrite those for
 *    different configurations.
 */
struct MonoOutput {
  /** Default constructor. Does not initialize the sample! */
  MonoOutput() {};
  /** Construct an audio frame from raw values (zero-centered) */
  MonoOutput(AudioOutputStorage_t l) : _l(l) {};
#if (MOZZI_AUDIO_CHANNELS > 1)
  /** Conversion to stereo operator: If used in a stereo config, returns identical channels (and gives a compile time warning).
      This _could_ be turned into an operator for implicit conversion in this case. For now we chose to apply conversion on demand, only, as most of the time
      using StereoOutput in a mono config, is not intended. */
  StereoOutput portable() const __attribute__((deprecated("Sketch generates mono output, but Mozzi is configured for stereo. Check MOZZI_AUDIO_CHANNELS setting.")));  // Note: defintion below
#endif
  /** Conversion to int operator. */
  operator AudioOutputStorage_t() const { return _l; };

  AudioOutputStorage_t l() const { return _l; };
  AudioOutputStorage_t r() const { return _l; };
  /** Clip frame to supported range. This is useful when at times, but only rarely, the signal may exceed the usual range. Using this function does not avoid
   *  artifacts, entirely, but gives much better results than an overflow. */
  MonoOutput& clip() { _l = CLIP_AUDIO(_l); return *this; };

  /** Construct an audio frame a zero-centered value known to be in the N bit range. Appropriate left- or right-shifting will be performed, based on the number of output
   *  bits available. While this function takes care of the shifting, beware of potential overflow issues, if your intermediary results exceed the 16 bit range. Use proper
   *  casts to int32_t or larger in that case (and the compiler will automatically pick the 32 bit overload in this case) */
  template<typename T> static inline MonoOutput fromNBit(uint8_t bits, T l) { return MonoOutput(SCALE_AUDIO(l, bits)); }
  /** Construct an audio frame from a zero-centered value known to be in the 8 bit range. On AVR, if MOZZI_OUTPUT_PWM mode, this is effectively the same as calling the
   * constructor, directly (no scaling gets applied). On platforms/configs using more bits, an appropriate left-shift will be performed. */
  static inline MonoOutput from8Bit(int16_t l) { return fromNBit(8, l); }
  /** Construct an audio frame from a zero-centered value known to be in the 16 bit range. This is jsut a shortcut for fromNBit(16, ...) provided for convenience. */
  static inline MonoOutput from16Bit(int16_t l) { return fromNBit(16, l); }
  /** Construct an audio frame from a SFix type from FixMath. Mozzi will figure out how many bits are in there and performs appropriate shifting to match the output range. */
  template<int8_t NI, int8_t NF, uint64_t RANGE>
  static inline MonoOutput fromSFix(SFix<NI,NF,RANGE> l) { return MonoOutput(SCALE_AUDIO(l.asRaw(), (NI+NF+1))) ;}
  /** Construct an audio frame a zero-centered value known to be above at almost but not quite the N bit range, e.g. at N=8 bits and a litte. On most platforms, this is
   *  exactly the same as fromNBit(), shifting up or down to the platforms' available resolution.
   *
   *  However, on AVR, MOZZI_OUTPUT_PWM mode (where about 8.5 bits are usable), the value will be shifted to the (almost) 9 bit range, instead of to the 8 bit range. allowing to
   *  make use of that extra half bit of resolution. In many cases it is useful to follow up this call with clip(). E.g.:
   *
   *  @code
   *  return MonoOutput::fromAlmostNBit(10, oscilA.next() + oscilB.next() + oscilC.next()).clip();
   *  @endcode
   */
  template<typename A, typename B> static inline MonoOutput fromAlmostNBit(A bits, B l) { return MonoOutput(SCALE_AUDIO_NEAR(l, bits)); }

private:
  AudioOutputStorage_t _l;
};

/** This struct encapsulates one frame of mono audio output. Internally, it really just boils down to two int values, but the struct provides
 *  useful API an top of that. For more detail see @ref MonoOutput . */
struct StereoOutput {
  /** Construct an audio frame from raw values (zero-centered) */
  StereoOutput(AudioOutputStorage_t l, AudioOutputStorage_t r) : _l(l), _r(r) {};
  /** Default constructor. Does not initialize the sample! */
  StereoOutput() {};
#if !MOZZI_IS(MOZZI_AUDIO_CHANNELS, MOZZI_STEREO)
  /** Conversion to int operator: If used in a mono config, returns only the left channel (and gives a compile time warning). 
      This _could_ be turned into an operator for implicit conversion in this case. For now we chose to apply conversion on demand, only, as most of the time
      using StereoOutput in a mono config, is not intended. */
  inline AudioOutput portable() const __attribute__((deprecated("Sketch generates stereo output, but Mozzi is configured for mono. Check MOZZI_AUDIO_CHANNELS setting."))) { return _l; };
#  if GITHUB_RUNNER_ACCEPT_STEREO_IN_MONO
  inline operator AudioOutput() const __attribute__((deprecated("Stereo converted to mono on github runner"))) { return _l; };
#  endif
#endif
  AudioOutputStorage_t l() const { return _l; };
  AudioOutputStorage_t r() const { return _r; };
  /** See @ref MonoOutput::clip(). Clips both channels. */
  StereoOutput& clip() { _l = CLIP_AUDIO(_l); _r = CLIP_AUDIO(_r); return *this; };

  /** See @ref MonoOutput::fromNBit(), stereo variant */
template<typename T> static inline StereoOutput fromNBit(uint8_t bits, T l, T r) { return StereoOutput(SCALE_AUDIO(l, bits), SCALE_AUDIO(r, bits)); }
  /** See @ref MonoOutput::from8Bit(), stereo variant */
  static inline StereoOutput from8Bit(int16_t l, int16_t r) { return fromNBit(8, l, r); }
  /** See @ref MonoOutput::from16Bit(), stereo variant */
  static inline StereoOutput from16Bit(int16_t l, int16_t r) { return fromNBit(16, l, r); }
/** See @ref MonoOutput::fromSFix(), stereo variant. Note that the two channels do not need to have the same number of bits. */
  template<int8_t NI, int8_t NF, uint64_t RANGE, int8_t _NI, int8_t _NF, uint64_t _RANGE>
  static inline StereoOutput fromSFix(SFix<NI,NF,RANGE> l, SFix<_NI,_NF,_RANGE> r) { return StereoOutput(SCALE_AUDIO(l.asRaw(), (NI+NF+1)), SCALE_AUDIO(r.asRaw(), (_NI+_NF+1))); }
  /** See @ref MonoOutput::fromAlmostNBit(), stereo variant */
  template<typename A, typename B> static inline StereoOutput fromAlmostNBit(A bits, B l, B r) { return StereoOutput(SCALE_AUDIO_NEAR(l, bits), SCALE_AUDIO_NEAR(r, bits)); }
private:
  AudioOutputStorage_t _l;
  AudioOutputStorage_t _r;
};

#if MOZZI_AUDIO_CHANNELS > 1
StereoOutput MonoOutput::portable() const { return StereoOutput(_l, _l); };
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_EXTERNAL_CUSTOM)
/** When setting using one of the external output modes (@ref MOZZI_OUTPUT_EXTERNAL_TIMED or @ref MOZZI_OUTPUT_EXTERNAL_CUSTOM) implement this function to take care of writing samples to the hardware.
 *  In all otther cases, it will be provided by the platform implementation. You should never call this function, directly, in your sketch. */
void audioOutput(const AudioOutput f);
#endif
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_CUSTOM)
/** For @ref MOZZI_OUTPUT_EXTERNAL_CUSTOM implement this function to return true, if and only if your hardware (or custom buffer) is ready to accept the next sample. */
inline bool canBufferAudioOutput();
#endif

/** Perform one step of (fast) pdm encoding, returning 8 "bits" (i.e. 8 ones and zeros).
 *  You will usually call this at least four or eight times, and possibly much more often
 *  for a single input sample.
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
  // three bits cannot be. (highest possible values:
  // nexttarget-lastwritten == 0b00001111111111111,
  // sample                 == 0b01111111111111111)
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

#endif
