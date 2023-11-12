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

template<byte IP, byte FP>
class FixMath2
{
 public:

 private:
  // use of the IntegerType for that?
};







#endif
