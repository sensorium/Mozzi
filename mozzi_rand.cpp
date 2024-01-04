#include "mozzi_rand.h"

// moved these out of xorshift96() so xorshift96() can be reseeded manually
static uint32_t x=132456789, y=362436069, z=521288629;
// static unsigned long x= analogRead(A0)+123456789;
// static unsigned long y= analogRead(A1)+362436069;
// static unsigned long z= analogRead(A2)+521288629;

/** @ingroup random
Random number generator. A faster replacement for Arduino's random function,
which is too slow to use with Mozzi.  
Based on Marsaglia, George. (2003). Xorshift RNGs. http://www.jstatsoft.org/v08/i14/xorshift.pdf
@return a random 32 bit integer.
@todo check timing of xorshift96(), rand() and other PRNG candidates.
 */

unsigned long xorshift96()
{ //period 2^96-1
	uint32_t t;

	x ^= x << 16;
	x ^= x >> 5;
	x ^= x << 1;

	t = x;
	x = y;
	y = z;
	z = t ^ x ^ y;

	return z;
}


/** @ingroup random
Initialises Mozzi's (pseudo)random number generator xorshift96(), which is used
in Mozzi's rand() function. This can be useful if you want random sequences to
be different on each run of a sketch, by seeding with fairly random input, such
as analogRead() on an unconnected pin (as explained in the Arduino documentation
for randomSeed(). randSeed is the same as xorshift96Seed(), but easier to
remember. 
@param seed a number to use as a seed.
*/
void randSeed(unsigned long seed)
{
	x=seed;
}

// To be defined in hardware implementations
void autoRandomSeeds(uint32_t *x, uint32_t *y, uint32_t *z);

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
void randSeed() {
	autoRandomSeeds(&x, &y, &z);
}

/** @ingroup random
Initialises Mozzi's (pseudo)random number generator xorshift96() with a chosen seed number.
@param seed a number to use as a seed.
*/
void xorshiftSeed(unsigned long seed)
{
	x=seed;
}



/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for generating at audio rate with Mozzi. 
@param minval the minimum signed byte value of the range to be chosen from. Minval will be the minimum value possibly returned by the function. 
@param maxval the maximum signed byte value of the range to be chosen from. Maxval-1 will be the largest value possibly returned by the function. 
@return a random signed byte between minval and maxval-1 inclusive.
*/
int8_t rand(int8_t minval, int8_t maxval)
{
	return (int8_t) ((((int) (lowByte(xorshift96()))) * (maxval-minval))>>8) + minval;
}


/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for generating at audio rate with Mozzi.
@param minval the minimum unsigned byte value of the range to be chosen from.  Minval will be the minimum value possibly returned by the function.
@param maxval the maximum unsigned byte value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random unsigned byte between minval and maxval-1 inclusive.
*/
uint8_t rand(uint8_t minval, uint8_t maxval)
{
	return (uint8_t) ((((unsigned int) (lowByte(xorshift96()))) * (maxval-minval))>>8) + minval;
}


/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for generating at audio rate with Mozzi.
@param minval the minimum signed int value of the range to be chosen from.  Minval will be the minimum value possibly returned by the function.
@param maxval the maximum signed int value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random int between minval and maxval-1 inclusive.
*/
int rand( int minval,  int maxval)
{
	return (int) ((((xorshift96() & 0xFFFF) * (maxval-minval))>>16) + minval);
}


/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for generating at audio rate with Mozzi.
@param minval the minimum unsigned int value of the range to be chosen from.  Minval will be the minimum value possibly returned by the function.
@param maxval the maximum unsigned int value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random unsigned int between minval and maxval-1 inclusive.
*/
unsigned int rand(unsigned int minval, unsigned int maxval)
{
	return (unsigned int) ((((xorshift96() & 0xFFFF) * (maxval-minval))>>16) + minval);
}


/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for generating at audio rate with Mozzi.
@param maxval the maximum signed byte value of the range to be chosen from.  Maxval-1 will be the largest value  possibly returned by the function.
@return a random signed byte between 0 and maxval-1 inclusive.
*/
int8_t rand(int8_t maxval)
{
	return (int8_t) ((((int) (lowByte(xorshift96()))) * maxval)>>8);
}


/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for generating at audio rate with Mozzi.
@param maxval the maximum unsigned byte value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random unsigned byte between 0 and maxval-1 inclusive.
*/
uint8_t rand(uint8_t maxval)
{
	return (uint8_t) ((((unsigned int) (lowByte(xorshift96()))) * maxval)>>8);
}


/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for generating at audio rate with Mozzi.
@param maxval the maximum signed int value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random int between 0 and maxval-1 inclusive.
*/
int rand(int maxval)
{
	return (int) (((xorshift96() & 0xFFFF) * maxval)>>16);
}


/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for generating at audio rate with Mozzi.
@param maxval the maximum unsigned int value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random unsigned int between 0 and maxval-1 inclusive.
*/
unsigned int rand(unsigned int maxval)
{
	return (unsigned int) (((xorshift96() & 0xFFFF) * maxval)>>16);
}


/** @ingroup random
Generates a random number in the range for midi notes.
@return a random value between 0 and 127 inclusive
*/
uint8_t randMidiNote()
{
	return lowByte(xorshift96())>>1;
}
