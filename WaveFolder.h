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


#include "IntegerType.h"
#include "AudioOutput.h"


/* 
   A simple wavefolder which folds the waves once it reachs the up or
   low limits. The wave can be folded several times. It constrains the wave
   to be constrain between the LowLimit and the HighLimit.
   By default, this class is working on data which not overflow the  
   AudioOutputStorage_t type, which is int by default.
   Feeding samples which, before folding, are overflowing this container
   will lead to unpredicted behavior.
   It is possible to use a bigger type with the template formulation
   if needed.
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
    Ri = NUMERATOR / (hl-ll); // calculated here to speed up next()
  }


  /** Set the low limit where the wave will start to be folded back the other way.
      @param lowLimit the low limit used by the wavefolder.
  */
  void setLowLimit(T lowLimit)
  {
    ll = lowLimit;
    R = hl-ll;
    Ri = NUMERATOR / (hl-ll); // calculated here to speed up next()
  }


  /** Set the low and the high limits at the same time.
      @param lowLimit the low limit used by the wavefolder
      @param highLimit the high limit used by the wavefolder
      @note highLimit MUST be higher than lowLimit
  */
  void setLimits(T lowLimit, T highLimit)
  {
    hl = highLimit;
    ll = lowLimit;
    R = hl-ll;
    Ri = NUMERATOR / (hl-ll); // calculated here to speed up next()
  }


  /** Return the next folded sample
      @param in is the signal input.
      @return the folded output.
  */
  T next(T in)
  {
     if (in > hl)
      {
	typename IntegerType<sizeof(T)>::unsigned_type sub = in-hl;
	/* Instead of using a division, we multiply by the inverse.
	   As the inverse is necessary smaller than 1, in order to fit in an integer
	   we multiply it by NUMERATOR before computing the inverse.
	   The shift is equivalent to divide by the NUMERATOR:
	   q = sub / R = (sub * (NUMERATOR/R))/NUMERATOR with NUMERATOR/R = Ri
	*/
        typename IntegerType<sizeof(T)>::unsigned_type q =  ((typename IntegerType<sizeof(T)+sizeof(T)>::unsigned_type) sub*Ri) >> SHIFT; 
	typename IntegerType<sizeof(T)>::unsigned_type r = sub - q*R; // remainer
	if (q&0b1) return ll+r;  //odd
	else return hl-r;  // even
	
      }
    else if (in < ll)
      {
	typename IntegerType<sizeof(T)>::unsigned_type sub = ll-in;
	typename IntegerType<sizeof(T)>::unsigned_type q = ((typename IntegerType<sizeof(T)+sizeof(T)>::unsigned_type) sub*Ri) >> SHIFT;
	typename IntegerType<sizeof(T)>::unsigned_type r = sub - q*R; // remainer
	if (q&0b1) return hl-r;
	else return ll+r;
      }
    else return in;
  }

private:
  T hl;
  T ll;
  typename IntegerType<sizeof(T)>::unsigned_type R;
  typename IntegerType<sizeof(T)>::unsigned_type Ri;
  static const uint8_t SHIFT = (sizeof(T) << 3);
  static const typename IntegerType<sizeof(T)>::unsigned_type NUMERATOR = ((typename IntegerType<sizeof(T)+sizeof(T)>::unsigned_type) 1<<(SHIFT))-1;

  // Slower (way slower, around 2.5 times) but more precise, kept in case we want to switch one day.
  /*   typename IntegerType<sizeof(T)+sizeof(T)>::unsigned_type Ri;
  static const uint8_t SHIFT = 1+(sizeof(T) << 3);
  static const typename IntegerType<sizeof(T)+sizeof(T)>::unsigned_type NUMERATOR = ((typename IntegerType<sizeof(T)+sizeof(T)>::unsigned_type) 1<<(SHIFT))-1;*/
    
    
};


#endif



