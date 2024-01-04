#ifndef MOZZI_RAND_H_
#define MOZZI_RAND_H_

#include <Arduino.h>
#include "internal/mozzi_rand_p.h"

/** @ingroup random
Random number generator. A faster replacement for Arduino's random function,
which is too slow to use with Mozzi.
Based on Marsaglia, George. (2003). Xorshift RNGs. http://www.jstatsoft.org/v08/i14/xorshift.pdf
@return a random 32 bit integer.
@todo check timing of xorshift96(), rand() and other PRNG candidates.
 */
inline uint32_t xorshift96() { return MozziRandPrivate::xorshift96(); };

/** @ingroup random
Initialises Mozzi's (pseudo)random number generator xorshift96(), which is used
in Mozzi's rand() function. This can be useful if you want random sequences to
be different on each run of a sketch, by seeding with fairly random input, such
as analogRead() on an unconnected pin (as explained in the Arduino documentation
for randomSeed(). randSeed is the same as xorshift96Seed(), but easier to
remember. 
@param seed a number to use as a seed.
*/
inline void randSeed(uint32_t seed) {
  MozziRandPrivate::x=seed;
}

/** @ingroup random
Initialises Mozzi's (pseudo)random number generator xorshift96(), which is used
in Mozzi's rand() function. This can be useful if you want random sequences to
be different on each run of a sketch, by seeding with a fairly random input.
randSeed() called without a parameter uses noise from reading the Arduino's
internal temperature as the seed, a technique discussed at
http://arduino.cc/forum/index.php/topic,38091.0.html, borrowing code put there
by Rob Tillaart.

@note Intialization of the random seed is done differently on different MCUs,
      but is nowhere near perfect for most (and for some it is not even implemented at all).
      Many implementations (e.g. on AVR, STM32) simply rely on reading a (hopefully noisy)
      internal temperature sensor.
      You will often get better results by calling analogRead() - @em not mozziAnalogRead(0), in this case! -
      on one or two floating (non-connected) analog pins.
*/
inline void randSeed() { MozziRandPrivate::autoSeed(); };

/** @ingroup random
Initialises Mozzi's (pseudo)random number generator xorshift96() with a chosen seed number.
@param seed a number to use as a seed.
// TODO: duplicate deprecate / remove
*/
inline void xorshiftSeed(uint32_t seed) { randSeed(seed); };

/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for generating at audio rate with Mozzi. 
@param minval the minimum signed byte value of the range to be chosen from. Minval will be the minimum value possibly returned by the function. 
@param maxval the maximum signed byte value of the range to be chosen from. Maxval-1 will be the largest value possibly returned by the function. 
@return a random signed byte between minval and maxval-1 inclusive.
*/
inline int8_t rand(int8_t minval, int8_t maxval)
{
  return (int8_t) ((((int) (lowByte(xorshift96()))) * (maxval-minval))>>8) + minval;
}

/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for generating at audio rate with Mozzi.
@param maxval the maximum signed byte value of the range to be chosen from.  Maxval-1 will be the largest value  possibly returned by the function.
@return a random signed byte between 0 and maxval-1 inclusive.
*/
inline int8_t rand(int8_t maxval)
{
  return (int8_t) ((((int) (lowByte(xorshift96()))) * maxval)>>8);
}

/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for generating at audio rate with Mozzi.
@param minval the minimum unsigned byte value of the range to be chosen from.  Minval will be the minimum value possibly returned by the function.
@param maxval the maximum unsigned byte value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random unsigned byte between minval and maxval-1 inclusive.
*/
inline uint8_t rand(uint8_t minval, uint8_t maxval)
{
  return (uint8_t) ((((unsigned int) (lowByte(xorshift96()))) * (maxval-minval))>>8) + minval;
}

/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for generating at audio rate with Mozzi.
@param maxval the maximum unsigned byte value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random unsigned byte between 0 and maxval-1 inclusive.
*/
inline uint8_t rand(uint8_t maxval)
{
  return (uint8_t) ((((unsigned int) (lowByte(xorshift96()))) * maxval)>>8);
}

/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for generating at audio rate with Mozzi.
@param minval the minimum signed int value of the range to be chosen from.  Minval will be the minimum value possibly returned by the function.
@param maxval the maximum signed int value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random int between minval and maxval-1 inclusive.
*/
inline int16_t rand(int16_t minval, int16_t maxval)
{
  return (int16_t) ((((xorshift96() & 0xFFFF) * (maxval-minval))>>16) + minval);
}


/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for generating at audio rate with Mozzi.
@param minval the minimum unsigned int value of the range to be chosen from.  Minval will be the minimum value possibly returned by the function.
@param maxval the maximum unsigned int value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random unsigned int between minval and maxval-1 inclusive.
*/
inline uint16_t rand(uint16_t minval, uint16_t maxval)
{
  return (uint16_t) ((((xorshift96() & 0xFFFF) * (maxval-minval))>>16) + minval);
}


/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for generating at audio rate with Mozzi.
@param maxval the maximum signed int value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random int between 0 and maxval-1 inclusive.
*/
inline int16_t rand(int16_t maxval)
{
  return (int16_t) (((xorshift96() & 0xFFFF) * maxval)>>16);
}


/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for generating at audio rate with Mozzi.
@param maxval the maximum unsigned int value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random unsigned int between 0 and maxval-1 inclusive.
*/
inline uint16_t rand(uint16_t maxval)
{
  return (uint16_t) (((xorshift96() & 0xFFFF) * maxval)>>16);
}


/** @ingroup random
Generates a random number in the range for midi notes.
@return a random value between 0 and 127 inclusive
*/
inline uint8_t randMidiNote()
{
  return lowByte(xorshift96())>>1;
}

#endif /* MOZZI_RAND_H_ */
