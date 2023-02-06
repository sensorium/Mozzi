/*
 * ResonantFilter.h
 *
 * Copyright 2012 Tim Barrass
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons
 * Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */

#ifndef RESONANTFILTER_H_
#define RESONANTFILTER_H_

#include "IntegerType.h"
#include "AudioOutput.h"
#include "meta.h"



/*
simple resonant filter posted to musicdsp.org by Paul Kellett
http://www.musicdsp.org/archive.php?classid=3#259, applying the 
modification from Peter Schoffhauzer to make it able to output
all filter types (LowPass, HighPass, Notch and BandPass).

The generic filter is ResonantFilter<unsigned_t type, FILTER_TYPE>. 
 - type specifies the type expected for the cutoff and resonance. Only uint8_t and uint16_t have been tested. These are denoted 8bits and 16bits versions of the filter in the following.
 - FILTER_TYPE specifies the filter type. LOWPASS, BANDPASS, HIGHPASS and NOTCH are available types.

Two versions are available: the 8bits and 16bits versions (see above).
The 8bits version is an optimized version that uses 8bits values to set
the resonance and the cutoff_freq. It can works on 8bits samples only
on 8bits platforms.
The 16bits version consumes more CPU ressources but uses 16bits values
for resonance and cutoff_freq and can work on samples up to 16bits on
8bits platforms and up to 32 on 32bits platforms.

The filter can be instanciated using the template version ResonantFilter<unsigned_t type, FILTER_TYPE>. For ease of use, the following types are also accepted:

8bits versions: LowPassFilter, HighPassFilter, BandPassFilter, NotchFilter
16bits versions: LowPassFilter16, HighPassFilter16, BandPassFilter16, NotchFilter16



//// ALGORITHM ////
// set feedback amount given f and q between 0 and 1
fb = q + q/(1.0 - f);
In order to avoid a slow division we use the use a Taylor expansion to approximate 1/(1.0 - f):
Close to f=0: 1/(1.0-f) approx 1.0+f.
Hence: fb = q + q * (1.0 + f)
This approximation is less and less valid with an increasing cutoff, leading to a reduction of the resonance of the filter at high cutoff frequencies.

// for each sample...
buf0 = buf0 + f * (in - buf0 + fb * (buf0 - buf1));
buf1 = buf1 + f * (buf0 - buf1);
out = buf1; // LowPass
out = in - buf0; // HighPass
out = buf0 - buf1; // BandPass
out = in - buf0 + buf1; // Notch

fixed point version of the filter
"dave's blog of art and programming" http://www.pawfal.org/dave/blog/2011/09/
*/





enum filter_types { LOWPASS, BANDPASS, HIGHPASS, NOTCH };

/** A generic resonant filter for audio signals.
 */
template<int8_t FILTER_TYPE, typename su=uint8_t>
class ResonantFilter
{

public:
  /** Constructor.
   */
  ResonantFilter() { ; }


  /** deprecated.  Use setCutoffFreqAndResonance(su cutoff, su
  resonance).

  Set the cut off frequency,
  @param cutoff use the range 0-255 to represent 0-8191 Hz (AUDIO_RATE/2) for ResonantFilter, cutoff use the range 0-65535 to represent 0-AUDIO_RATE/2.
  Be careful of distortion at the lower end, especially with high resonance.
  */
  void setCutoffFreq(su cutoff)
      {
    f = cutoff;
    fb = q + ucfxmul(q, (typename IntegerType<sizeof(su)+sizeof(su)>::unsigned_type) SHIFTED_1 + cutoff);
  }

  /** deprecated.  Use setCutoffFreqAndResonance(su cutoff, su
  resonance).

  Set the resonance. If you hear unwanted distortion, back off the resonance.
  After setting resonance, you need to call setCuttoffFreq() to hear the change!
  @param resonance in the range 0-255 for ResonantFilter, 0-65535 for ResonantFilter<FILTER_TYPE, uint16_t>, with 255/65535 being most resonant
  @note	Remember to call setCuttoffFreq() after resonance is changed!
  */
  void setResonance(su resonance) { q = resonance; }

  /**
  Set the cut off frequency and resonance.  Replaces setCutoffFreq() and
  setResonance().  (Because the internal calculations need to be done whenever either parameter changes.)
  @param cutoff range 0-255 represents 0-8191 Hz (AUDIO_RATE/2) for ResonantFilter, range 0-65535 for ResonantFilter16
  Be careful of distortion at the lower end, especially with high resonance.
  @param resonance range 0-255 for ResonantFilter, 0-65535 for ResonantFilter<FILTER_TYPE, uint16_t>, 255/65535 is most resonant.
  */
  void setCutoffFreqAndResonance(su cutoff, su resonance)
	{
    f = cutoff;
    q = resonance; // hopefully optimised away when compiled, just here for
                   // backwards compatibility
    fb = q + ucfxmul(q,(typename IntegerType<sizeof(su)+sizeof(su)>::unsigned_type) SHIFTED_1 + cutoff);
  }

  /** Calculate the next sample, given an input signal.
  @param in the signal input. Should not be more than 8bits on 8bits platforms (Arduino) if using the 8bits version and not 16bits version.
  @return the signal output.
  @note Timing: about 11us.
  */
  //	10.5 to 12.5 us, mostly 10.5 us (was 14us)
  inline AudioOutputStorage_t next(AudioOutputStorage_t in)
  {
    advanceBuffers(in);
    return current(in, Int2Type<FILTER_TYPE>());
  }

protected:
  su q;
  su f;
  typename IntegerType<sizeof(su)+sizeof(su)>::unsigned_type fb;
  AudioOutputStorage_t buf0, buf1;
  const uint8_t FX_SHIFT = sizeof(su) << 3;
  const uint8_t FX_SHIFT_M_1 = FX_SHIFT-1;
  const su SHIFTED_1 = (1<<FX_SHIFT)-1;

  // // multiply two fixed point numbers (returns fixed point)
  // inline
  // long fxmul(long a, long b)
  // {
  // 	return (a*b)>>FX_SHIFT;
  // }

  inline void advanceBuffers(AudioOutputStorage_t in)
  {
    buf0 += fxmul(((in - buf0) + fxmul(fb, buf0 - buf1)), f);
    buf1 += ifxmul(buf0 - buf1, f); // could overflow if input changes fast
  }
  
  inline AudioOutputStorage_t current(AudioOutputStorage_t in, Int2Type<LOWPASS>) {return buf1;}

  inline AudioOutputStorage_t current(AudioOutputStorage_t in, Int2Type<HIGHPASS>) {return in - buf0;}

  inline AudioOutputStorage_t current(AudioOutputStorage_t in, Int2Type<BANDPASS>) {return buf0-buf1;}

  inline AudioOutputStorage_t current(AudioOutputStorage_t in, Int2Type<NOTCH>) {return in - buf0 + buf1;}
  
  // multiply two fixed point numbers (returns fixed point)
  inline typename IntegerType<sizeof(su)+sizeof(su)>::unsigned_type ucfxmul(su a, typename IntegerType<sizeof(su)+sizeof(su)>::unsigned_type b)
	{
	  return (((typename IntegerType<sizeof(su)+sizeof(su)>::unsigned_type)a * (b >> 1)) >> (FX_SHIFT_M_1));
  }

  // multiply two fixed point numbers (returns fixed point)
  inline typename IntegerType<sizeof(AudioOutputStorage_t)+sizeof(su)-1>::signed_type ifxmul(typename IntegerType<sizeof(AudioOutputStorage_t )+sizeof(su)-1>::signed_type a, su b) { return ((a * b) >> FX_SHIFT); } 

  // multiply two fixed point numbers (returns fixed point)
  inline typename IntegerType<sizeof(AudioOutputStorage_t)+sizeof(AudioOutputStorage_t)>::signed_type fxmul(typename IntegerType<sizeof(AudioOutputStorage_t)+sizeof(AudioOutputStorage_t)>::signed_type a, typename IntegerType<sizeof(AudioOutputStorage_t)+sizeof(su)-1>::signed_type b) { return ((a * b) >> FX_SHIFT); }
};

/** A generic filter for audio signals that can produce lowpass, highpass, bandpass and notch outputs at runtime.
Behaves like ResonantFilter for setting the resonance and cutoff frequency.
Like ResonantFilter, it can be used on different sample sizes: MultiResonantFilter<uint8_t> and MultiResonantFilter<uint16_t> have been tested.
For the former, both cutoff and resonance are uint8_t, hence between 0-255.
For the later,  both cutoff and resonance are uint16_t, hence between 0-65535.
 */
template<typename su=uint8_t>
class MultiResonantFilter: public ResonantFilter<LOWPASS,su>
{
public:
  /** Compute the filters, given an input signal.
      @param in the signal input. Should not be more than 8bits on 8bits platforms (Arduino) if using the 8bits version and not 16bits version.
  */
inline void next (AudioOutputStorage_t in)
  {
    last_in = in;
    ResonantFilter<LOWPASS,su>::advanceBuffers(in);
  }
  /** Return the input filtered with a lowpass filter
      @return the filtered signal output.
   */
  inline AudioOutputStorage_t low() {return ResonantFilter<LOWPASS,su>::current(last_in,Int2Type<LOWPASS>());}
  /** Return the input filtered with a highpass filter
      @return the filtered signal output.
   */
  inline AudioOutputStorage_t high() {return ResonantFilter<LOWPASS,su>::current(last_in,Int2Type<HIGHPASS>());}
  /** Return the input filtered with a bandpass filter
      @return the filtered signal output.
   */
  inline AudioOutputStorage_t band() {return ResonantFilter<LOWPASS,su>::current(last_in,Int2Type<BANDPASS>());}
  /** Return the input filtered with a notch filter
      @return the filtered signal output.
   */
  inline AudioOutputStorage_t notch() {return ResonantFilter<LOWPASS,su>::current(last_in,Int2Type<NOTCH>());}
  
private:
  AudioOutputStorage_t last_in;
};


typedef ResonantFilter<LOWPASS> LowPassFilter;
typedef ResonantFilter<LOWPASS, uint16_t> LowPassFilter16;
/*
typedef ResonantFilter<uint8_t, HIGHPASS> HighPassFilter;
typedef ResonantFilter<uint16_t, HIGHPASS> HighPassFilter16;
typedef ResonantFilter<uint8_t, BANDPASS> BandPassFilter;
typedef ResonantFilter<uint16_t, BANDPASS> BandPassFilter16;
typedef ResonantFilter<uint8_t, NOTCH> NotchFilter;
typedef ResonantFilter<uint16_t, NOTCH> NotchFilter16;
*/


/**
@example 10.Audio_Filters/ResonantFilter/ResonantFilter.ino
This example demonstrates the ResonantFilter specification of this class.

@example 10.Audio_Filters/ResonantFilter16/ResonantFilter16.ino
This example demonstrates the ResonantFilter16 specification of this class.

@example 10.Audio_Filters/MultiResonantFilter/MultiResonantFilter.ino
This example demonstrates the MultiResonantFilter specification of this class.
*/

#endif /* RESONANTFILTER_H_ */
