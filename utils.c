#include "utils.h"

/** Converts midi note number to frequency.  Code borrowed rom AF_precision_synthesis sketch.  
 *  Like mtof object in Pd, midi values can have fractions.
 */
float mtof(float x) {
  return (float)(8.1757989156 * pow(2.0, x/12.0));
}


/** Fast random function which returns a 32 bit integer.
 *  This code comes from 
 *  Based on Marsaglia, George. (2003). Xorshift RNGs. http://www.jstatsoft.org/v08/i14/xorshift.pdf
 */
 
unsigned long xorshift96() {          //period 2^96-1
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


void setupFastADC() {
    /*
    * Make analogRead() faster than the standard arduino version, see:
    * http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1208715493/11
    * FASTADC set in guts.h
    */
    sbi(ADCSRA,ADPS2);
    cbi(ADCSRA,ADPS1);
    cbi(ADCSRA,ADPS0);
}
