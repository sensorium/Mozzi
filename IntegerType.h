/*
 * IntegerType.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2021-2024 Thomas Friedrichsmeier and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
 */

#ifndef INTTYPE_H_
#define INTTYPE_H_

#include <Arduino.h>

/** @ingroup util
Provides appropriate integer types that can bit the given number of bytes on this platform (at most 64).
*/
template<uint8_t BYTES> struct IntegerType {
    // at an odd value, such as 3 bytes? Add one more byte (up to at most 8 bytes)..
    typedef typename IntegerType<(BYTES < 8) ? (BYTES+1) : 8>::unsigned_type unsigned_type;
    typedef typename IntegerType<(BYTES < 8) ? (BYTES+1) : 8>::signed_type signed_type;
};

// These are the specializations for the types that we actually assume to exist:
template<> struct IntegerType<1> {
    typedef uint8_t unsigned_type;
    typedef int8_t signed_type;
};

template<> struct IntegerType<2> {
    typedef uint16_t unsigned_type;
    typedef int16_t signed_type;
};

template<> struct IntegerType<4> {
    typedef uint32_t unsigned_type;
    typedef int32_t signed_type;
};

template<> struct IntegerType<8> {
    typedef uint64_t unsigned_type;
    typedef int64_t signed_type;
};

#endif /* INTTYPE_H_ */
