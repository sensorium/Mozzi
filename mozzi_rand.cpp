#include "mozzi_rand.h"

// moved these out of xorshift96() so xorshift96() can be reseeded manually
static unsigned long x=132456789, y=362436069, z=521288629;
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
	// static unsigned long x=123456789, y=362436069, z=521288629;
	unsigned long t;

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
Initialises Mozzi's (pseudo)random number generator xorshift96(), which is used in Mozzi's rand() function. This can be useful if you want random sequences to be different on each run of a sketch, by seeding with fairly random input, such as analogRead() on an unconnected pin (as explained in the Arduino documentation for randomSeed(). randSeed is the same as xorshift96Seed(), but easier to remember.
@param seed an int to use as a seed.
*/
void randSeed(unsigned long seed)
{
	x=seed;
}


/** @ingroup random
Initialises Mozzi's (pseudo)random number generator xorshift96(). This can be
useful if you want random sequences to be different on each run of a sketch, by
seeding with fairly random input, such as analogRead() on an unconnected pin (as
explained in the Arduino documentation for randomSeed().
@param seed an int to use as a seed.
*/
void xorshiftSeed(unsigned long seed)
{
	x=seed;
}



/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for Mozzi. 
@param minval the minimum signed byte value of the range to be chosen from. Minval will be the minimum value possibly returned by the function. 
@param maxval the maximum signed byte value of the range to be chosen from. Maxval-1 will be the largest value possibly returned by the function. 
@return a random char between minval and maxval-1 inclusive.
*/
char rand(char minval, char maxval)
{
	return (char) ((((int) (lowByte(xorshift96()))) * (maxval-minval))/256) + minval;
}


/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for Mozzi.
@param minval the minimum unsigned byte value of the range to be chosen from.  Minval will be the minimum value possibly returned by the function.
@param maxval the maximum unsigned byte value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random unsigned char between minval and maxval-1 inclusive.
*/
unsigned char rand(unsigned char minval, unsigned char maxval)
{
	return (unsigned char) ((((unsigned int) (lowByte(xorshift96()))) * (maxval-minval))/256) + minval;
}


/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for Mozzi.
@param minval the minimum signed int value of the range to be chosen from.  Minval will be the minimum value possibly returned by the function.
@param maxval the maximum signed int value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random int between minval and maxval-1 inclusive.
*/
int rand( int minval,  int maxval)
{
	return (int) ((((xorshift96()>>16) * (maxval-minval))>>16) + minval);
}


/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for Mozzi.
@param minval the minimum unsigned int value of the range to be chosen from.  Minval will be the minimum value possibly returned by the function.
@param maxval the maximum unsigned int value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random unsigned int between minval and maxval-1 inclusive.
*/
unsigned int rand(unsigned int minval, unsigned int maxval)
{
	return (unsigned int) ((((xorshift96()>>16) * (maxval-minval))>>16) + minval);
}


/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for Mozzi.
@param maxval the maximum signed byte value of the range to be chosen from.  Maxval-1 will be the largest value  possibly returned by the function.
@return a random char between 0 and maxval-1 inclusive.
*/
char rand(char maxval)
{
	return (char) ((((int) (lowByte(xorshift96()))) * maxval)/256);
}


/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for Mozzi.
@param maxval the maximum unsigned byte value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random unsigned char between 0 and maxval-1 inclusive.
*/
unsigned char rand(unsigned char maxval)
{
	return (unsigned char) ((((unsigned int) (lowByte(xorshift96()))) * maxval)/256);
}


/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for Mozzi.
@param maxval the maximum signed int value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random int between 0 and maxval-1 inclusive.
*/
int rand(int maxval)
{
	return (int) (((xorshift96()>>16) * maxval)>>16);
}


/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for Mozzi.
@param maxval the maximum unsigned int value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random unsigned int between 0 and maxval-1 inclusive.
*/
unsigned int rand(unsigned int maxval)
{
	return (unsigned int) (((xorshift96()>>16) * maxval)>>16);
}


/** @ingroup random
Generates a random number in the range for midi notes.
@return a random value between 0 and 127 inclusive
*/
unsigned char randMidiNote()
{
	return lowByte(xorshift96())>>1;
}


