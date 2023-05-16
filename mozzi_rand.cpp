#include "mozzi_rand.h"

#include "hardware_defines.h"

#if IS_STM32()
//#include <STM32ADC.h>
extern STM32ADC adc;
#elif IS_ESP8266()
#include <esp8266_peri.h>
#endif

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


#if defined (__AVR_ATmega644P__)

// a less fancy version for gizduino (__AVR_ATmega644P__) which doesn't know INTERNAL
static long longRandom()
{
	return ((long)analogRead(0)+63)*(analogRead(1)+97); // added offsets in case analogRead is 0
}

#elif defined (__AVR_ATmega2560__)
/*
longRandom(), used as a seed generator, comes from:
http://arduino.cc/forum/index.php/topic,38091.0.html
//  AUTHOR: Rob Tillaart
// PURPOSE: Simple Random functions based upon unreliable internal temp sensor
// VERSION: 0.1
//       DATE: 2011-05-01
//
// Released to the public domain, use at own risk
//
*/
static long longRandom()
{
	//analogReference(INTERNAL2V56);
	unsigned long rv = 0;
	for (uint8_t i=0; i< 32; i++) rv |= ((analogRead(8)+2294) & 1L) << i; // added 2294 in case analogRead is 0
	return rv;
}

#elif IS_AVR()

static long longRandom()
{
	//analogReference(INTERNAL);
	unsigned long rv = 0;
	for (uint8_t i=0; i< 32; i++) rv |= ((analogRead(8)+1171) & 1L) << i; // added 1171 in case analogRead is 0
	return rv;
}


#endif


/** @ingroup random
Initialises Mozzi's (pseudo)random number generator xorshift96(), which is used
in Mozzi's rand() function. This can be useful if you want random sequences to
be different on each run of a sketch, by seeding with a fairly random input.
randSeed() called without a parameter uses noise from reading the Arduino's
internal temperature as the seed, a technique discussed at
http://arduino.cc/forum/index.php/topic,38091.0.html, borrowing code put there
by Rob Tillaart.
@note It's not perfect, as discussed in the forum thread.
It might only work with some processors: (from the thread)
"...ATmega328P in DIP, possibly others but the duemilanove and uno will do it at least."
So far, gizduino's __AVR_ATmega644P__ chip doesn't like it, so we use (long)analogRead(0)*analogRead(1) for that instead.
It works to some degree on STM32 chips, but the produced seed is not very random at all. Again, using an appropriate
analogRead() (preferably on one or two floating input pins) is much more effective.
@todo add Teensy 3 code
*/
void randSeed() {
#if IS_AVR()
	ADCSRA &= ~ (1 << ADIE); // adc Disable Interrupt, re-enable at end
	// this attempt at remembering analog_reference stops it working 
	// maybe needs a delay after changing analog reference in longRandom (Arduino reference suggests this)
	// because the analog reads return 0
	//uint8_t analog_reference_orig = ADMUX&192; // analog_reference is high 2 bits of ADMUX, store this because longRandom sets it to internal
	x=longRandom();
	y=longRandom();
	z=longRandom();
	//analogReference(analog_reference_orig); // change back to original
	ADCSRA |= (1 << ADIE); // adc re-Enable Interrupt
#elif IS_STM32()
	// Unfortunately the internal temp sensor on STM32s does _not_ appear to create a lot of noise.
	// Ironically, the calls to calibrate help induce some random noise. You're still fairly likely to produce two equal
	// random seeds in two subsequent runs, however.
	adc.enableInternalReading();
	float dummy = adc.readTemp();
        int* dummy_int = (int*) &dummy;
	x=*dummy_int;
	adc.calibrate();
	dummy = adc.readTemp();
	y=*dummy_int;
	adc.calibrate();
	dummy = adc.readTemp();
	z=*dummy_int;
#elif IS_ESP8266()
	x = RANDOM_REG32;
	y = random (0xFFFFFFFF) ^ RANDOM_REG32;
	z = random (0xFFFFFFFF) ^ RANDOM_REG32;
#else
#warning Automatic random seeding not implemented on this platform
#endif
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
@param minval the minimum signed uint8_t value of the range to be chosen from. Minval will be the minimum value possibly returned by the function. 
@param maxval the maximum signed uint8_t value of the range to be chosen from. Maxval-1 will be the largest value possibly returned by the function. 
@return a random int8_t between minval and maxval-1 inclusive.
*/
int8_t rand(int8_t minval, int8_t maxval)
{
	return (int8_t) ((((int) (lowByte(xorshift96()))) * (maxval-minval))>>8) + minval;
}


/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for generating at audio rate with Mozzi.
@param minval the minimum unsigned uint8_t value of the range to be chosen from.  Minval will be the minimum value possibly returned by the function.
@param maxval the maximum unsigned uint8_t value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random uint8_t between minval and maxval-1 inclusive.
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
@param maxval the maximum signed uint8_t value of the range to be chosen from.  Maxval-1 will be the largest value  possibly returned by the function.
@return a random int8_t between 0 and maxval-1 inclusive.
*/
int8_t rand(int8_t maxval)
{
	return (int8_t) ((((int) (lowByte(xorshift96()))) * maxval)>>8);
}


/** @ingroup random
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for generating at audio rate with Mozzi.
@param maxval the maximum unsigned uint8_t value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random uint8_t between 0 and maxval-1 inclusive.
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
