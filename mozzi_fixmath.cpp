#include "mozzi_fixmath.h"

/** @ingroup fixmath
@{ 
*/

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
//Here is some ready to use fast integer 1 uint8_t wide math functions (from ht1632c library).

/** 
fast uint8_t modulus
@param n numerator
@param  d denominator
@return modulus
*/
uint8_t uint8_tMod(uint8_t n, uint8_t d)
{
	while(n >= d)
		n -= d;
	return n;
}

/** Fast uint8_t division
@param n numerator
@param  d denominator
@return quotient
*/
uint8_t uint8_tDiv(uint8_t n, uint8_t d)
{
	uint8_t q = 0;
	while(n >= d)
	{
		n -= d;
		q++;
	}
	return q;
}

/* fast integer (1 uint8_t) PRNG */
uint8_t uint8_tRnd(uint8_t min, uint8_t max)
{
	static uint8_t seed;
	seed = (21 * seed + 21);
	return min + uint8_tMod(seed, --max);
}
//WARNING: don't use this uint8_tRnd() function for cryptography!

//} of snip from http://code.google.com/p/ht1632c/wiki/Optimizations



// from http://stackoverflow.com/questions/101439/the-most-efficient-way-to-implement-an-integer-based-power-function-powint-int
/* Exponentiation by squaring.
*/
int ipow(int base, int exp)
{
	int result = 1;
	while (exp)
	{
		if (exp & 1)
			result *= base;
		exp >>= 1;
		base *= base;
	}

	return result;
}


/*
from: http://objectmix.com/vhdl/189970-2-powerof-x-where-x-fixed-point-value-2.html
 
to do 2^(x.y) first find
2^x and 2^(x+1) through bit shifting 1 to the left by x and (x + 1) places
 
now you do linear interpolation by drawing a line through these two points 2^x,
2^(x+1), then use f = m*x+b. the slope, m = rise over run
= (2^(x+1) - 2^x)/((x+1) - (x))
= 2^(x) * (2 - 1) / 1
= 2^(x)
b = 2^x, so to linearly interpolate do....(edited out typo)..
f = 2^(x) * (y) + 2^x
= 2^x * (y + 1)
where x is integer part, y is fractional part
*/


/** 
fast replacement for pow(2,x), where x is a Q8n8 fractional
fixed-point exponent. It's less accurate than pow(2,x), but useful where a
tradeoff between accuracy and speed is required to keep audio from glitching.
@param exponent in Q8n8 format.
@return pow(2,x) in Q16n16 format.
@todo Q16n16_pow2() accuracy needs more attention.
*/
Q16n16 Q16n16_pow2(Q8n8 exponent)
{
	// to do 2^(x.y) first find
	//2^x and 2^(x+1) through bit shifting 1 to the left by x and (x + 1) places
	uint8_t Q = (uint8_t)((Q8n8)exponent>>8); // integer part
	uint8_t n = (uint8_t) exponent; // fractional part
	// f = 2^x * (y + 1)
	return (((Q16n16)Q8n8_FIX1 << Q) * (Q8n8_FIX1 + n));
}




//http://www.codecodex.com/wiki/Calculate_an_integer_square_root
//see Integer Square Roots by Jack W. Crenshaw, figure 2, http://www.embedded.com/electronics-blogs/programmer-s-toolbox/4219659/Integer-Square-Roots

uint32_t  // OR uint16 OR uint8_t
isqrt32 (uint32_t n) // OR isqrt16 ( uint16_t n ) OR  isqrt8 ( uint8_t n ) - respectively [ OR overloaded as isqrt (uint16_t?? n) in C++ ]
{
	register uint32_t // OR register uint16_t OR register uint8_t - respectively
	root, remainder, place;

	root = 0;
	remainder = n;
	place = 0x40000000; // OR place = 0x4000; OR place = 0x40; - respectively

	while (place > remainder)
		place = place >> 2;
	while (place)
	{
		if (remainder >= root + place)
		{
			remainder = remainder - root - place;
			root = root + (place << 1);
		}
		root = root >> 1;
		place = place >> 2;
	}
	return root;
}


//http://www.codecodex.com/wiki/Calculate_an_integer_square_root
uint16_t  // OR uint16_t OR uint8_t
isqrt16 (uint16_t n) // OR isqrt16 ( uint16_t n ) OR  isqrt8 ( uint8_t n ) - respectively [ OR overloaded as isqrt (uint16_t?? n) in C++ ]
{
	register uint16_t // OR register uint16_t OR register uint8_t - respectively
	root, remainder, place;

	root = 0;
	remainder = n;
	place = 0x4000; // OR place = 0x4000; OR place = 0x40; - respectively

	while (place > remainder)
		place = place >> 2;
	while (place)
	{
		if (remainder >= root + place)
		{
			remainder = remainder - root - place;
			root = root + (place << 1);
		}
		root = root >> 1;
		place = place >> 2;
	}
	return root;
}


/** @} */
