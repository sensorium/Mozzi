/*
 * FixMath2.h
 *
 * Copyright 2012 Tim Barrass
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons
 * Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */


#ifndef FIXMATH2_H_
#define FIXMATH2_H_

#include "IntegerType.h"

#define SHIFTR(x,bits) (bits > 0 ? (x) >> (bits) : (x) << (-bits)) // shift right for positive shift numbers, and left for negative ones.

template<byte NI, byte NF> // NI and NF being the number of bits for the integral and the fractionnal parts respectively.
class UFixMath2
{
  typedef typename IntegerType< ((NI+NF)>>3) >::unsigned_type internal_type ;
 public:
  /** Constructor
   */
  UFixMath2() {;}
  UFixMath2(float fl)  {internal_value = static_cast<internal_type> (fl * (internal_type(1) << NF));}
  UFixMath2(typename IntegerType<((NI)>>3)>::unsigned_type integral_part, typename IntegerType<((NF)>>3)>::unsigned_type fractionnal_part)
  {
    internal_value = (integral_part << NI) + fractionnal_part;
  } // probably a more confusing than anything constructor!

  float asFloat() { return (static_cast<float>(internal_value)) / (internal_type(1)<<NF); }
  internal_type getInt() { return internal_value; }

 private:
  internal_type internal_value;
  
};

#endif
