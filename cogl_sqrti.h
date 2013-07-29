/**
 * cogl_sqrti:
 * @x: integer value
 *
 * Very fast fixed point implementation of square root for integers.
 *
 * This function is at least 6x faster than clib sqrt() on x86, and (this is
 * not a typo!) about 500x faster on ARM without FPU. It's error is < 5%
 * for arguments < %COGL_SQRTI_ARG_5_PERCENT and < 10% for arguments <
 * %COGL_SQRTI_ARG_10_PERCENT. The maximum argument that can be passed to
 * this function is COGL_SQRTI_ARG_MAX.
 *
 * Return value: integer square root.
 *
 *
 * Since: 0.2
 */
#include "mozzi_fixmath.h"


//http://www.codecodex.com/wiki/Calculate_an_integer_square_root
//see Integer Square Roots by Jack W. Crenshaw, figure 2, http://www.embedded.com/electronics-blogs/programmer-s-toolbox/4219659/Integer-Square-Roots
    typedef unsigned char       	uint8_t;  
    typedef unsigned short int  uint16;  
    typedef unsigned long int   uint32;  
      

      
    uint32  // OR uint16 OR uint8_t  
    isqrt32 (uint32 n) // OR isqrt16 ( uint16 n ) OR  isqrt8 ( uint8_t n ) - respectively [ OR overloaded as isqrt (uint?? n) in C++ ]  
    {  
        register uint32 // OR register uint16 OR register uint8_t - respectively  
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
     uint16  // OR uint16 OR uint8_t  
    isqrt16 (uint16 n) // OR isqrt16 ( uint16 n ) OR  isqrt8 ( uint8_t n ) - respectively [ OR overloaded as isqrt (uint?? n) in C++ ]  
    {  
        register uint16 // OR register uint16 OR register uint8_t - respectively  
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
    
    
    
    
    
/*-- isqrt -----------------------------------------------------------------*/

unsigned long isqrt(unsigned long n) {
  unsigned long s, t;

#define sqrtBit(k) \
  t = s+(1UL<<(k-1)); t <<= k+1; if (n >= t) { n -= t; s |= 1UL<<k; }

  s = 0UL;
#ifdef __alpha
  if (n >= 1UL<<62) { n -= 1UL<<62; s = 1UL<<31; }
  sqrtBit(30); sqrtBit(29); sqrtBit(28); sqrtBit(27); sqrtBit(26);
  sqrtBit(25); sqrtBit(24); sqrtBit(23); sqrtBit(22); sqrtBit(21);
  sqrtBit(20); sqrtBit(19); sqrtBit(18); sqrtBit(17); sqrtBit(16);
  sqrtBit(15);
#else
  if (n >= 1UL<<30) { n -= 1UL<<30; s = 1UL<<15; }
#endif
  sqrtBit(14); sqrtBit(13); sqrtBit(12); sqrtBit(11); sqrtBit(10);
  sqrtBit(9); sqrtBit(8); sqrtBit(7); sqrtBit(6); sqrtBit(5);
  sqrtBit(4); sqrtBit(3); sqrtBit(2); sqrtBit(1);
  if (n > s<<1) s |= 1UL;

#undef sqrtBit

  return s;
} /* end isqrt */


/**
http://stackoverflow.com/questions/1100090/looking-for-an-efficient-integer-square-root-algorithm-for-arm-thumb2
It's essentially from the Wikipedia article on square-root computing methods.
But it has been changed to use stdint.h types uint32_t etc. Strictly speaking
the return type could be changed to uint16_t.

* \brief    Fast Square root algorithm
 *
 * Fractional parts of the answer are discarded. That is:
 *      - SquareRoot(3) --> 1
 *      - SquareRoot(4) --> 2
 *      - SquareRoot(5) --> 2
 *      - SquareRoot(8) --> 2
 *      - SquareRoot(9) --> 3
 *
 * \param[in] a_nInput - unsigned integer for which to find the square root
 *
 * \return Integer square root of the input value.
 */
uint16_t SquareRoot(uint32_t a_nInput)
{
    uint32_t op  = a_nInput;
    uint32_t res = 0;
    uint32_t one = 1uL << 30; // The second-to-top bit is set: use 1u << 14 for uint16_t type; use 1uL<<30 for uint32_t type


    // "one" starts at the highest power of four <= than the argument.
    while (one > op)
    {
        one >>= 2;
    }

    while (one != 0)
    {
        if (op >= res + one)
        {
            op = op - (res + one);
            res = res +  2 * one;
        }
        res >>= 1;
        one >>= 2;
    }
    return res;
}




int cogl_sqrti (int number)
{

	/* This is a fixed point implementation of the Quake III sqrt algorithm,
	 * described, for example, at
	 *   http://www.codemaestro.com/reviews/review00000105.html
	 *
	 * While the original QIII is extremely fast, the use of floating division
	 * and multiplication makes it perform very on arm processors without FPU.
	 *
	 * The key to successfully replacing the floating point operations with
	 * fixed point is in the choice of the fixed point format. The QIII
	 * algorithm does not calculate the square root, but its reciprocal ('y'
	 * below), which is only at the end turned to the inverse value. In order
	 * for the algorithm to produce satisfactory results, the reciprocal value
	 * must be represented with sufficient precission; the 16.16 we use
	 * elsewhere in clutter is not good enough, and 10.22 is used instead.
	 */
	//CoglFixed x;
	long x;
	uint32_t y_1;        /* 10.22 fixed point */
	uint32_t f = 0x600000; /* '1.5' as 10.22 fixed */

	union
	{
		float f;
		uint32_t i;
	} flt, flt2;

	flt.f = number;

	x = Q15n0_to_Q15n16(number) / 2;

	/* The QIII initial estimate */
	flt.i = 0x5f3759df - ( flt.i >> 1 );

	/* Now, we convert the float to 10.22 fixed. We exploit the mechanism
	 * described at http://www.d6.com/users/checker/pdfs/gdmfp.pdf.
	 *
	 * We want 22 bit fraction; a single precission float uses 23 bit
	 * mantisa, so we only need to add 2^(23-22) (no need for the 1.5
	 * multiplier as we are only dealing with positive numbers).
	 *
	 * Note: we have to use two separate variables here -- for some reason,
	 * if we try to use just the flt variable, gcc on ARM optimises the whole
	 * addition out, and it all goes pear shape, since without it, the bits
	 * in the float will not be correctly aligned.
	 */
	flt2.f = flt.f + 2.0;
	flt2.i &= 0x7FFFFF;

	/* Now we correct the estimate */
	y_1 = (flt2.i >> 11) * (flt2.i >> 11);
	y_1 = (y_1 >> 8) * (x >> 8);

	y_1 = f - y_1;
	flt2.i = (flt2.i >> 11) * (y_1 >> 11);

	/* If the original argument is less than 342, we do another
	 * iteration to improve precission (for arguments >= 342, the single
	 * iteration produces generally better results).
	 */
	if (x < 171)
	{
		y_1 = (flt2.i >> 11) * (flt2.i >> 11);
		y_1 = (y_1 >> 8) * (x >> 8);

		y_1 = f - y_1;
		flt2.i = (flt2.i >> 11) * (y_1 >> 11);
	}

	/* Invert, round and convert from 10.22 to an integer
	 * 0x1e3c68 is a magical rounding constant that produces slightly
	 * better results than 0x200000.
	 */
	return (number * flt2.i + 0x1e3c68) >> 22;

}
