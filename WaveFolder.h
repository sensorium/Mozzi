/*
 * Wavefolder.h
 *
 * Copyright 2022 Thomas Combriat
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons
 * Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */

#ifndef WAVEFOLDER_H
#define WAVEFOLDER_H

#include "AudioOutput.h"


/* 
   A simple wavefolder which folds the waves once it reachs the up or
   low limits. The wave can be folded several times. It constrains the wave
   to be constrain between the LowLimit and the HighLimit.
*/





/** A simple wavefolder
 */

template<typename T=AudioOutputStorage_t>
  class WaveFolder
  {  
  public:
  /** Constructor
   */
  WaveFolder(){;}


  /** Set the high limit where the wave will start to be folded back the other way.
      @param highLimit the high limit used by the wavefolder.
  */
  void setHighLimit(T highLimit)
  {
    hl = highLimit;
    R = hl-ll;
  }


  /** Set the low limit where the wave will start to be folded back the other way.
      @param lowLimit the low limit used by the wavefolder.
  */
  void setLowLimit(T lowLimit)
  {
    ll = lowLimit;
    R = hl-ll;
  }


  /** Set the low and the high limits at the same time.
      @param lowLimit the low limit used by the wavefolder
      @param highLimit the high limit used by the wavefolder
  */
  void setLimits(T lowLimit, T highLimit)
  {
    hl = highLimit;
    ll = lowLimit;
    R = hl-ll;
  }


  /** Return the next folded sample
      @param in is the signal input.
      @return the folded output.
  */
  T next(T in)
  {
    if (in > hl)
      {
	T sub = in-hl;
	T q = sub/R; // ratio
	T r = sub - q*R; // remainer
	if (q%2 == 0) return hl-r;
	else return ll+r;
      }
    else if (in < ll)
      {
	T sub = ll-in;
	T q = sub/R; // ratio
	T r = sub - q*R; // remainer
	if (q%2 == 0) return ll+r;
	else return hl-r;
      }
    else return in;
  }

  private:
  T hl;
  T ll;
  T R;
  };


#endif



