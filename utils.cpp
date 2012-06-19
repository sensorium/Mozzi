#include "utils.h"

/** @ingroup util
Converts midi note number to frequency.
@param midival a midi note number.  Like the mtof object in Pd, midi values can have fractions.
@return the frequency represented by the input midi note number..
 */
// code from AF_precision_synthesis sketch, copyright 2009, Adrian Freed.
float mtof(float midival)
{
	return (float)(8.1757989156 * pow(2.0, midival/12.0));
}

/** @ingroup util
Generates a random number in the range for midi notes.
@return a random value between 0 and 127 inclusive
*/
unsigned char randomMidi(){
  lowByte(xorshift96())>>1;
}


/** @ingroup util
Random number generator.
A faster replacement for Arduino's random function,
which is too slow to use with Mozzi.
@return a random 32 bit integer.
 */
// Based on Marsaglia, George. (2003). Xorshift RNGs. http://www.jstatsoft.org/v08/i14/xorshift.pdf
unsigned long xorshift96()
{          //period 2^96-1
	static unsigned long x=123456789, y=362436069, z=521288629;
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

//Snipped from http://code.google.com/p/ht1632c/wiki/Optimizations
//TB2012 changed names to not interfere with arduino compilation
//Fast integer math
//
//If you need to include arithmetic operations in you code but you don't need
//floating point operations, you could use boolean operations instead of arithmetic
//operations, or use smaller data types and custom functions instead of stdlib functions
//or C operators (expecially / and %).
//Look at IntegerCodeSnippets, http://code.google.com/p/ht1632c/wiki/IntegerCodeSnippets
//
//Here is some ready to use fast integer 1 byte wide math functions (from ht1632c library).

/* fast integer (1 byte) modulus */
byte byteMod(byte n, byte d)
{
	while(n >= d)
		n -= d;
	return n;
}

/* fast integer (1 byte) division */
byte byteDiv(byte n, byte d)
{
	byte q = 0;
	while(n >= d)
	{
		n -= d;
		q++;
	}
	return q;
}

/* fast integer (1 byte) PRNG */
byte byteRnd(byte min, byte max)
{
	static byte seed;
	seed = (21 * seed + 21);
	return min + byteMod(seed, --max);
}
//WARNING: don't use this byteRnd() function for cryptography!

//end of snip from http://code.google.com/p/ht1632c/wiki/Optimizations


/** @ingroup util
Make analogRead() faster than the standard Arduino version. Put this in setup()
if you intend to use analogRead() with Mozzi, to avoid glitches. See:
http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1208715493/11
*/
void setupFastADC()
{
	sbi(ADCSRA,ADPS2);
	cbi(ADCSRA,ADPS1);
	cbi(ADCSRA,ADPS0);
}

