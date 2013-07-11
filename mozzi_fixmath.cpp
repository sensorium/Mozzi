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
//Here is some ready to use fast integer 1 byte wide math functions (from ht1632c library).

/** Fast byte modulus
@param n numerator
@param  d denominator
@return modulus
*/
byte byteMod(byte n, byte d)
{
	while(n >= d)
		n -= d;
	return n;
}

/** Fast byte division
@param n numerator
@param  d denominator
@return quotient
*/
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

/** This is a fast replacement for pow(2,x), where x is a fractional number in Q8n8
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
	unsigned char Q = (unsigned char)((Q8n8)exponent>>8); // integer part
	unsigned char n = (unsigned char) exponent; // fractional part
	// f = 2^x * (y + 1)
	return (((Q16n16)Q8n8_FIX1 << Q) * (Q8n8_FIX1 + n));
}



// from divfix https://instruct1.cit.cornell.edu/courses/ee476/Math/GCC644/fixedPt/FixedPtOps.c
// replaced  begin and end with { and }
// replaced int with Q7n8 (signed)

/** Fast Q7n8 fixed point division.
@param nn numerator
@param  dd denominator
@return quotient
*/
Q7n8 Q7n8_div(Q7n8 nn, Q7n8 dd)
{
    Q7n8 x, d ;
    signed char count, neg ;
    count = 0;
    neg = 0 ; 
    d = dd ;
    
    // only works with + numbers
    if (d & 0x8000)
    {
        neg = 1;
        d = -d ;
    }
    
    // range reduction
    while (d>0x0100)
    {
        --count ;
        d >>= 1 ;
    }
    
    while (d<0x0080)
    {
        ++count ;
        d <<= 1 ;
    }
     
    // Newton interation
    x = 0x02ea - (d<<1) ;
    //x = multfix(x, 0x0200-multfix(d,x));
    //x = multfix(x, 0x0200-multfix(d,x)); 
    // TB2013 should test this vs above for speed
    int tmp;
    tmp = (int)0x0200- (int)(((long)d*x)>>16);
	x = ((long)x*tmp)>>16;
	tmp = (int)0x0200- (int)(((long)d*x)>>16);
	x = ((long)x*tmp)>>16;

    
    // range expansion
    if (count>0)  x = x<<count ;
    else if (count<0) x = x>>(-count) ;  
    
    // fix sign
    if (neg==1) x=-x;
    
    //form ratio
    //x = multfix(x,nn) ;
	// TB2013 should test this vs above for speed
	x = ((long)x*nn)>>16;
     
    return x ;  
}


// from sqrtfix https://instruct1.cit.cornell.edu/courses/ee476/Math/GCC644/fixedPt/FixedPtOps.c
// replaced begin and end with { and }
// replaced int with 8n8 (unsigned)

/** Fast Q8n8 fixed point square root.
@param aa number to find the root of
@return square root
*/
Q8n8 Q8n8_sqrt(Q8n8 aa) {

	Q8n8 a;
	char nextbit, ahigh;
	Q8n8 root, p ;
	a = aa;
	ahigh = a>>8 ;
	//
	// range sort to get integer part and to
	// check for weird bits near the top of the range
	if (ahigh >= 0x40)   //bigger than 64?
	{
		if (a > 0x7e8f)  //>=126.562 = 11.25^2
		{
			root = 0x0b40;  // 11
			nextbit = 0x10 ;
		}
		else if (ahigh >= 0x79)  //>=121
		{
			root = 0x0b00;  // 11
			nextbit = 0x40 ;
		}
		else if (ahigh >= 0x64)  //>=100
		{
			root = 0x0a00;  // 10
			nextbit = 0x80 ;
		}
		else if (ahigh >= 0x51)  //>=81
		{
			root = 0x0900;  // 9
			nextbit = 0x80 ;
		}
		else //64
		{
			root = 0x0800;  //8
			nextbit = 0x80 ;
		}
	}
	else if  (ahigh >= 0x10)  //16  //smaller than 64 and bigger then 16
	{
		if (ahigh >= 0x31)  //49
		{
			root = 0x0700;  //7
			nextbit = 0x80 ;
		}
		else if (ahigh >= 0x24)  //36
		{
			root = 0x0600;  //6
			nextbit = 0x80 ;
		}
		else if (ahigh >= 0x19)  //25
		{
			root = 0x0500;  //5
			nextbit = 0x80 ;
		}
		else //16
		{
			root = 0x0400;  //4
			nextbit = 0x80 ;
		}
	}
	else   //smaller than 16
	{
		if (ahigh >= 0x09)  //9
		{
			root = 0x0300;  //3
			nextbit = 0x80 ;
		}
		else if (ahigh >= 0x04)  //4
		{
			root = 0x0200;  //2
			nextbit = 0x80 ;
		}
		else if (ahigh >= 0x01)  //1
		{
			root = 0x0100;  //1
			nextbit = 0x80 ;
		}
		else   //less than one
		{
			root = 0;
			nextbit = 0x80 ;
		}
	}
	// now get the low order bits
	while (nextbit)
	{
		root = nextbit + root;
		//p =  multfix(root,root);
		// TB2013 should test this vs above for speed
		p = ((unsigned long)root*root)>>16;
		//
		if (p >= a) root = root - nextbit ;
		nextbit = nextbit>>1 ;
	}
	return root ;
}

/** @} */
