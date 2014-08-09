/*
 * mozzi_fixmath.h
 *
 * Copyright 2012 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */

#ifndef FIXEDMATH_H_
#define FIXEDMATH_H_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

/**@ingroup fixmath
@{
*/
// types
typedef int8_t Q0n7; 				/**< signed fractional number using 7 fractional bits, represents -0.5 to 0.496*/
typedef int8_t Q7n0;				/**< ordinary old signed Q7n0 int8_t with 0 fractional bits, represents -128 to 127*/
typedef uint8_t Q0n8;  		/**< unsigned fractional number using 8 fractional bits, represents 0.0 to 0.996*/
typedef uint8_t Q8n0;  		/**< normal uint8_t with 0 fractional bits, represents 0.0 to 255.0*/
typedef uint16_t Q0n16;  		/**< unsigned fractional number using 16 fractional bits, represents 0.0 to 0.999*/
typedef uint32_t Q0n31;				/**< signed number using 0 integer bits and 31 fractional bits, represents -0.2147483648 to 0.2147483647*/
typedef int16_t Q7n8; 				/**< signed fractional number using 7 integer bits and 8 fractional bits, represents -127.996 to 127.996*/
typedef uint16_t Q3n13; /**< unsigned fractional number using 3 integer bits and 13 fractional bits, represents 0 to 7.999*/
typedef int16_t Q1n14;				/**< signed fractional number using 1 integer bit and 14 fractional bits, represents -1.999 to 1.999*/
typedef int16_t Q15n0;				/**< signed number using 15 integer bits and 0 fractional bits, represents -2147483648 to 2147483647*/
typedef uint16_t Q8n8;		/**< unsigned fractional number using 8 integer bits and 8 fractional bits, represents 0 to 255.996*/
typedef int16_t	 Q0n15;				/**< signed fractional number using 0 integer bits and 15 fractional bits, represents -0.32768 to 0.32767*/
typedef uint16_t	 Q1n15;	/**< unsigned fractional number using 1 integer bit and 15 fractional bits, represents 0 to 1.999*/
typedef uint16_t	 Q16n0;	/**< unsigned number using 16 integer bits and 0 fractional bits, represents 0 to 65536.0*/
typedef int32_t Q23n8;				/**< signed fractional number using 23 integer bits and 8 fractional bits, represents -8388607.996 to 8388607.996*/
typedef int32_t Q15n16;			/**< signed fractional number using 15 integer bits and 16 fractional bits, represents -32767.999 to 32767.999*/
typedef int32_t Q31n0;				/**< signed (normal int32_t int16_t) number using 31 integer bits and 0 fractional bits, represents -2147483648 to 2147483647*/
typedef uint32_t Q32n0;		/**< unsigned (normal uint32_t int16_t) number using 32 integer bits and 0 fractional bits, represents 0 to 4294967295*/
typedef uint32_t	Q0n32;		/**< unsigned fractional number using 0 integer bits and 32 fractional bits, represents 0 to 0.999999999767169*/
typedef uint32_t	Q8n24;		/**< signed fractional number using 8 integer bits and 24 fractional bits, represents 0 to 255.999*/
typedef uint32_t Q24n8;		/**< unsigned fractional number using 24 integer bits and 8 fractional bits, represents 0 to  16777215*/
typedef uint32_t Q16n16;	/**< unsigned fractional number using 16 integer bits and 16 fractional bits, represents 0 to 65535.999*/
/** @}*/

/** @ingroup fixmath
@{
*/
// macros to save runtime calculations for representations of 1
#define Q0n7_FIX1 ((Q0n7) 127)			/**< 0.992 in Q0n7 format*/
#define Q7n8_FIX1 ((Q7n8) 256)			/**< 1 in Q7n8 format*/
#define Q8n8_FIX1 ((Q8n8) 256)			/**< 1 in Q8n8 format*/
#define Q23n8_FIX1 ((Q23n8) 256)			/**< 1 in Q23n8 format*/
#define Q1n14_FIX1 ((Q1n14) 16384)		/**< 1 in Q1n14 format*/
#define Q1n15_FIX1 ((Q1n15) 32768)		/**< 1 in Q1n15 format*/
#define Q16n16_FIX1 ((Q16n16) 65536)		/**< 1 in Q16n16 format*/
#define Q0n15_FIX1 ((Q0n15) 32767)		/**< 0.999 in Q0n15 format*/
#define Q0n16_FIX1 ((Q0n16) 65535)		/**< 0.999 in Q0n16 format*/
#define Q15n0_FIX1  ((Q15n0) 16384)		/**< 1 in Q15n0 format*/
#define Q15n16_FIX1 ((Q15n16) 65536)		/**< 1 in Q15n16 format*/
#define Q8n24_FIX1 ((Q8n24) 16777216)	/**< 1 in Q8n24 format*/
#define Q0n32_FIX1 ((Q0n32) 4294967295)	/**< 0.999999999767169 in Q0n32 format*/

#define Q16n16_PI ((Q16n16) 205887)		/**< PI in Q16n16 format*/
#define Q3n13_2PI ((Q3n13) 411775)		/**< 2*PI in Q3n13 format*/
#define Q16n16_2PI ((Q16n16) 411775)		/**< 2*PI in Q16n16 format*/

#define low15bits ((Q1n15) 32767) /**< Useful for keeping the lower 15 bits of a Q1n15 number, using &*/
/** @}*/


/*
Type conversions: Float to Q
 
To convert a number from floating point to Qm.n format:
 
  Multiply the floating point number by 2^n
  Round to the nearest integer
 
Q to float
 
To convert a number from Qm.n format to floating point:
 
  Convert the number to floating point as if it were an integer
  Multiply by 2^-n
*/
/** @ingroup fixmath
@{
*/
inline
Q0n7 float_to_Q0n7(float a) { return static_cast<Q0n7>(a*256); } 		/**<Convert float to Q0n7 fix. @param a is a float*/

inline
Q0n8 float_to_Q0n8(float a) { return static_cast<Q0n8>(a*256); }		/**<Convert float to Q0n8 fix. @param a is a float*/

inline
Q7n8 float_to_Q7n8(float a) { return static_cast<Q7n8>(a*256); }		/**<Convert float to Q7n8 fix. @param a is a float*/

inline
Q8n8 float_to_Q8n8(float a) { return static_cast<Q8n8>(a*256); }		/**<Convert float to Q8n8 fix. @param a is a float*/

inline
Q1n14 float_to_Q1n14(float a) { return static_cast<Q1n14>(a*16384); }	/**<Convert float to Q1n14 fix. @param a is a float*/

inline
Q1n15 float_to_Q1n15(float a) { return static_cast<Q1n15>(a*32768); }	/**<Convert float to Q1n15 fix. @param a is a float*/

inline
Q8n24 float_to_Q8n24(float a) { return static_cast<Q8n24>(a*16777216); }	/**<Convert float to Q8n24 fix. @param a is a float*/

inline
Q23n8 float_to_Q23n8(float a) { return static_cast<Q23n8>(a*256); }		/**<Convert float to Q23n8 fix. @param a is a float*/

inline
Q24n8 float_to_Q24n8(float a) { return static_cast<Q24n8>(a*256); }		/**<Convert float to Q24n8 fix. @param a is a float*/

inline
Q16n16 float_to_Q16n16(float a) { return static_cast<Q16n16>(a*65536); }	/**<Convert float to Q16n16 fix. @param a is a float*/

inline
Q0n16 float_to_Q0n16(float a) { return static_cast<Q0n16>(a*65536); }	/**<Convert float to Q0n16 fix. @param a is a float*/

inline
Q15n16 float_to_Q15n16(float a) { return static_cast<Q15n16>(a*65536); }	/**<Convert float to Q15n16 fix. @param a is a float*/

inline
Q1n14 Q0n7_to_Q1n14(Q0n7 a) { return (static_cast<Q1n14>(a))<<7; }		/**<Convert Q0n7 int8_t to Q1n14 fix. @param a is a Q0n7 int8_t */

inline
Q15n16 Q0n7_to_Q15n16(Q0n7 a) { return (static_cast<Q15n16>(a))<<8; }		/**<Convert Q0n7 signed int8_t to Q15n16 fix. @param a is a Q0n7 signed int8_t */

inline
float Q0n7_to_float(Q0n7 a) { return (static_cast<float>(a))/256; }		/**<Convert Q0n7 fix to float. @param a is a Q0n7 int8_t*/

inline
Q1n15 Q0n8_to_Q1n15(Q0n8 a) { return (static_cast<Q1n15>(a))<<7; }			/**<Convert Q0n8 uint8_t to Q1n15 fix. @param a is a Q0n8 uint8_t */

inline
Q8n8 Q0n8_to_Q8n8(Q0n8 a) { return (static_cast<Q8n8>(a))<<8; }			/**<Convert Q0n8 uint8_t to Q8n8 fix. @param a is a Q0n8 uint8_t */

inline
Q8n24 Q0n8_to_Q8n24(Q0n8 a) { return (static_cast<Q8n24>(a))<<16; }		/**<Convert Q0n8 uint8_t to Q8n24 fix. @param a is a Q0n8 uint8_t */

inline
Q24n8 Q0n8_to_Q24n8(Q0n8 a) { return (static_cast<Q24n8>(a))<<8; }				/**<Convert Q0n8 uint8_t to Q24n8 fix. @param a is a Q0n8 uint8_t */

inline
Q15n16 Q0n8_to_Q15n16(Q0n8 a) { return (static_cast<Q15n16>(a))<<8; }	/**<Convert Q0n8 uint8_t to Q15n16 fix. @param a is a Q0n8 uint8_t */

inline
Q16n16 Q0n8_to_Q16n16(Q0n8 a) { return (static_cast<Q16n16>(a))<<8; }		/**<Convert Q0n8 uint8_t to Q16n16 fix. @param a is a Q0n8 uint8_t */

inline
float Q0n8_to_float(Q0n8 a) { return (static_cast<float>(a))/256; }	/**<Convert Q0n8 fix to float. @param a is a Q0n8 uint8_t*/

inline
Q7n8 Q7n0_to_Q7n8(Q7n0 a) { return (static_cast<Q7n8>(a))<<8; }			/**<Convert Q7n0 int8_t to Q7n8 fix. @param a is a int8_t*/

inline
Q15n16 Q7n0_to_Q15n16(Q7n0 a) { return (static_cast<Q15n16>(a))<<16; }			/**<Convert Q7n0 int8_t to Q15n16 fix. @param a is a int8_t*/

inline
Q7n8 Q8n0_to_Q7n8(Q8n0 a) { return (static_cast<Q7n8>(a))<<8; }		/**<Convert Q8n0 uint8_t to Q7n8 fix. @param a is a Q8n0 uint8_t*.  Beware of overflow. */

inline
Q8n8 Q8n0_to_Q8n8(Q8n0 a) { return (static_cast<Q8n8>(a))<<8; }			/**<Convert uint8_t to Q8n8 fix. @param a is a Q8n0 uint8_t*/

inline
Q15n16 Q8n0_to_Q15n16(Q8n0 a) { return (static_cast<Q15n16>(a))<<16; }			/**<Convert Q8n0 uint8_t to Q15n16 fix. @param a is a Q8n0 uint8_t */

inline
Q16n16 Q8n0_to_Q16n16(Q8n0 a) { return (static_cast<Q16n16>(a))<<16; }			/**<Convert Q8n0 uint8_t to Q16n16 fix. @param a is a Q8n0 uint8_t */

inline
Q7n0 Q7n8_to_Q7n0(Q7n8 a) { return static_cast<Q7n0>((Q7n8)a>>8); }			/**<Convert Q7n8 fix to Q7n0. @param a is a Q7n8 int16_t*/

inline
Q15n16 Q7n8_to_Q15n16(Q7n8 a) { return (static_cast<Q15n16>(a))<<8; }			/**<Convert Q7n8 fix to Q15n16. @param a is a Q7n8 int16_t*/

inline
float Q7n8_to_float(Q7n8 a) { return (static_cast<float>(a))/256; }			/**<Convert Q7n8 fix to float. @param a is a Q7n8 int16_t*/

inline
Q8n0 Q8n8_to_Q8n0(Q8n8 a) { return static_cast<Q8n0>((Q8n8)a>>8); }			/**<Convert Q8n8 fix to Q8n0 uint8_t. @param a is a Q8n8 uint16_t*/

inline
Q16n16 Q8n8_to_Q16n16(Q8n8 a) { return (static_cast<Q16n16>(a))<<8; }			/**<Convert Q8n8 fix to Q16n16 uint32_t. @param a is a Q8n8 uint16_t*/

inline
float Q8n8_to_float(Q8n8 a) { return (static_cast<float>(a))/256; }				/**<Convert Q8n8 fix to float. @param a is a Q8n8 uint16_t*/

inline
Q0n7 Q1n14_to_Q0n7(Q1n14 a) { return static_cast<Q0n7>((Q1n14)a>>7); }			/**<Convert Q1n14 fixed to Q0n7 int8_t. @param a is a Q1n14 int16_t*/

inline
float Q1n14_to_float(Q1n14 a) { return (static_cast<float>(a))/16384; }			/**<Convert fix to float. @param a is an int16_t*/

inline
Q0n8 Q1n15_to_Q0n8(Q1n15 a) { return static_cast<Q0n8>((Q1n15)a>>7); }			/**<Convert Q1n15 fixed to Q0n8 uint8_t. Only for  positive values! @param a is a Q1n15 uint16_t*/

inline
float Q1n15_to_float(Q1n15 a) { return (static_cast<float>(a))/32768; }			/**<Convert fix to float. @param a is a Q1n15 uint16_t*/

inline
float Q0n16_to_float(Q0n16 a) { return (static_cast<float>(a))/65536; }			/**<Convert fix to float. @param a is a Q0n16 uint16_t*/

inline
Q15n16 Q15n0_to_Q15n16(Q15n0 a) { return (static_cast<Q15n16>(a))<<16; }		/**<Convert Q15n0 int16_t to Q15n16 fix. @param a is a Q15n0 int16_t */

inline
Q15n16 Q16n0_to_Q15n16(Q16n0 a) { return (static_cast<Q15n16>(a))<<16; }			/**<Convert Q16n0 uint16_t to Q15n16 fix. @param a is a Q16n0 uint16_t */

inline
Q23n8 Q16n0_to_Q23n8(Q16n0 a) { return (static_cast<Q23n8>(a))<<8; }			/**<Convert Q16n0 uint16_t to Q23n8 fixed point signed int32_t. @param a is a Q16n0 uint16_t*/

inline
Q24n8 Q16n0_to_Q24n8(Q16n0 a) { return (static_cast<Q24n8>(a))<<8; }		/**<Convert Q16n0 uint16_t to Q24n8 fixed point uint32_t. @param a is a Q16n0 uint16_t*/

inline
Q16n16 Q16n0_to_Q16n16(Q16n0 a) { return (static_cast<Q16n16>(a))<<16; }			/**<Convert Q16n0 uint16_t to Q16n16 fixed point uint32_t. @param a is a Q16n0 uint16_t*/

inline
float Q16n0_to_float(Q16n0 a) { return (static_cast<float>(a)); }					/**<Convert Q16n0 uint16_t to float. @param a is a Q16n0 uint16_t*/

inline
Q0n8 Q8n24_to_Q0n8(Q8n24 a) { return static_cast<Q0n8>((Q8n24)a>>16); }		/**<Convert Q8n24 fixed to Q0n8 uint8_t. @param a is a Q8n24 uint32_t*/

inline
float Q8n24_to_float(Q8n24 a) { return (static_cast<float>(a))/16777216; }		/**<Convert fix to float. @param a is a Q8n24 uint32_t*/


inline
Q31n0 Q23n8_to_Q31n0(Q23n8 a) { return static_cast<Q31n0>((Q23n8)a>>8); }		/**<Convert Q23n8 fixed to Q31n0 int32_t. @param a is a Q23n8 int32_t*/

inline
Q16n0 Q23n8_to_Q16n0(Q23n8 a) { return static_cast<Q16n0>((Q23n8)a>>8); }		/**<Convert Q23n8 fixed to Q16n0 uint16_t. Positive values only. @param a is a Q23n8 int32_t*/

inline
Q15n0 Q23n8_to_Q15n0(Q23n8 a) { return static_cast<Q15n0>((Q23n8)a>>8); }		/**<Convert Q23n8 fixed to Q15n0 signed int16_t. @param a is a Q23n8 int32_t*/

inline
Q7n8 Q23n8_to_Q7n8(Q23n8 a) { return static_cast<Q7n8>(a); }				/**<Convert Q23n8 fixed to Q7n8 signed int16_t, losing most significant bits. @param a is a Q23n8 signed int32_t.*/


inline
float Q23n8_to_float(Q23n8 a) { return (static_cast<float>(a))/256; }			/**<Convert fix to float. @param a is a Q23n8 signed int32_t*/

inline
Q0n8 Q24n8_to_Q0n8(Q24n8 a) { return static_cast<Q0n8>(a); }			/**<Convert Q24n8 fixed to Q0n8 uint8_t. @param a is a Q24n8 uint32_t*/

inline
Q16n16 Q24n8_to_Q16n0(Q24n8 a) { return (static_cast<Q16n0>((Q24n8)a))>>8; }			/**<Convert Q24n8 fixed to Q16n0 uint16_t. @param a is a Q24n8 uint32_t*/

inline
Q32n0 Q24n8_to_Q32n0(Q24n8 a) { return static_cast<Q32n0>((Q24n8)a>>8); }		/**<Convert Q24n8 fixed to Q32n0 uint32_t. @param a is a Q24n8 uint32_t*/

inline
Q16n16 Q24n8_to_Q16n16(Q24n8 a) { return (static_cast<Q16n16>(a))<<8; }			/**<Convert Q24n8 fixed to Q16n16 uint32_t. @param a is a Q24n8 uint32_t*/

inline
float Q24n8_to_float(Q24n8 a) { return (static_cast<float>(a))/256; }				/**<Convert fix to float. @param a is a Q24n8 uint32_t*/

inline
Q0n8 Q15n16_to_Q0n8(Q15n16 a) { return static_cast<Q0n8>((Q15n16)a>>8); }			/**<Convert Q15n16 fixed to Q0n8 uint8_t.  Only for  positive values!  @param a is a Q15n16 signed int32_t*/

inline
Q8n0 Q15n16_to_Q8n0(Q15n16 a) { return static_cast<Q8n0>((Q15n16)a>>16); }			/**<Convert Q15n16 fixed to Q8n0 uint8_t.  Only for positive values!  @param a is a Q15n16 signed int32_t*/

inline
Q15n0 Q15n16_to_Q15n0(Q15n16 a) { return static_cast<Q15n0>((Q15n16)a>>16); }		/**<Convert Q15n16 fixed to Q15n0 signed int16_t. @param a is a Q15n16 signed int32_t*/

inline
Q7n8 Q15n16_to_Q7n8(Q15n16 a) { return static_cast<Q7n8>((Q15n16)a>>8); }			/**<Convert Q15n16 fixed to Q7n8 signed int16_t, keeping middle bits only. @param a is a Q15n16 signed int32_t.*/

inline
Q23n8 Q15n16_to_Q23n8(Q15n16 a) { return static_cast<Q23n8>((Q15n16)a>>8); }			/**<Convert Q15n16 fixed to Q23n8 signed int32_t. @param a is a Q15n16 signed int32_t.*/

inline
float Q15n16_to_float(Q15n16 a) { return (static_cast<float>(a))/65536; }			/**<Convert fix to float. @param a is a Q15n16 signed int32_t*/

inline
Q0n8 Q16n16_to_Q0n8(Q16n16 a) { return static_cast<Q0n8>((Q16n16)a>>8); }			/**<Convert Q16n16 fixed to Q0n8 uint8_t. @param a is a Q16n16 uint32_t*/

inline
Q16n0 Q16n16_to_Q16n0(Q16n16 a) { return static_cast<Q16n0>((Q16n16)a>>16); }		/**<Convert Q16n16 fixed to Q16n0 uint16_t. @param a is a Q16n16 uint32_t*/

inline
Q24n8 Q16n16_to_Q24n8(Q16n16 a) { return static_cast<Q24n8>((Q16n16)a>>8); }		/**<Convert Q16n16 fixed to Q24n8 uint32_t. @param a is a Q16n16 uint32_t*/

inline
float Q16n16_to_float(Q16n16 a) { return (static_cast<float>(a))/65536; }		/**<Convert fix to float. @param a is a Q16n16 uint32_t*/
/** @}*/

/* @ingroup fixmath
Fast (?) fixed point multiply for Q7n8 fractional numbers.
The c  version below is 3 times faster, and not subject to the same overflow limitations (+-3.99, or +-2048)
@param a Q7n8 format multiplicand
@param b Q7n8 format multiplier
@return a Q7n8 format product
*/
/*
#define Q7n8_mult(a,b)    	  \
({            \
int16_t prod, val1=a, val2=b ;    \
__asm__ __volatile__ (    \
	"muls %B1, %B2	\n\t"   \
	"mov %B0, r0 \n\t"	   \
	"mul %A1, %A2\n\t"	   \
	"mov %A0, r1 \n\t"   \
	"mulsu %B1, %A2	\n\t"   \
	"add %A0, r0  \n\t"     \
	"adc %B0, r1 \n\t"     \
	"mulsu %B2, %A1	\n\t"   \
	"add %A0, r0 \n\t"     \
	"adc %B0, r1  \n\t"    \
	"clr r1  \n\t" 		   \
	: "=&d" (prod)     \
	: "a" (val1), "a" (val2)  \
	  );        	\
  prod;        	\
})
*/

/** @ingroup fixmath
Fast fixed point multiply for Q7n8 fractional numbers.
@param a Q7n8 format multiplicand
@param b Q7n8 format multiplier
@return a Q7n8 format product
*/
inline
Q7n8 Q7n8_mult(Q7n8 a, Q7n8 b) {
  return ((int16_t)((((int32_t)(a))*(b))>>8));
}


/*
#define FMULS8(v1, v2)      \
({            \
  uint8_t res;        \
  uint8_t val1 = v1;      \
  uint8_t val2 = v2;      \
  __asm__ __volatile__      \
  (           \
    "fmuls $1, $2"   "\n\t" \
    "mov $0, r1"     "\n\t" \
    "clr r1"     "\n\t" \
    : "=&d" (res)       \
    : "a" (val1), "a" (val2)  \
  );            \
  res;          \
}) */


/*
// from octosynth, Joe Marshall 2011:
 
  // multiply 2 16 bit numbers together and shift 8 without precision loss
  // requires assembler really
  volatile uint8_t zeroReg=0;
  volatile uint16_t multipliedCounter=oscillators[c].phaseStep;
  asm volatile
  (
  // high uint8_ts mult together = high  uint8_t
  "ldi %A[outVal],0" "\n\t"
  "mul %B[phaseStep],%B[pitchB}]" "\n\t"
  "mov %B[outVal],r0" "\n\t"
  // ignore overflow into r1 (should never overflow)
  // low uint8_t * high uint8_t -> both uint8_ts
  "mul %A[phaseStep],%B[pitchB}]" "\n\t"
  "add %A[outVal],r0" "\n\t"
  // carry into high uint8_t
  "adc %B[outVal],r1" "\n\t"
  // high uint8_t* low uint8_t -> both uint8_ts
  "mul %B[phaseStep],%A[pitchB}]" "\n\t"
  "add %A[outVal],r0" "\n\t"
  // carry into high uint8_t
  "adc %B[outVal],r1" "\n\t"
  // low uint8_t * low uint8_t -> round
  "mul %A[phaseStep],%A[pitchB}]" "\n\t"
  // the adc below is to round up based on high bit of low*low:
  "adc %A[outVal],r1" "\n\t"
  "adc %B[outVal],%[ZERO]" "\n\t"
  "clr r1" "\n\t"
  :[outVal] "=&d" (multipliedCounter)
  :[phaseStep] "d" (oscillators[c].phaseStep),[pitchB}] "d"( pitchB}Multiplier),[ZERO] "d" (zeroReg)
  :"r1","r0"
  );
  oscillators[c].phaseStep=multipliedCounter;
 
  */



int16_t ipow(int16_t base, int16_t exp); /**< dangerous overflow-prone  int16_t power function */

Q16n16 Q16n16_pow2(Q8n8 exponent);

uint8_t uint8_tMod(uint8_t n, uint8_t d);
uint8_t uint8_tDiv(uint8_t n, uint8_t d);
uint8_t uint8_tRnd(uint8_t min, uint8_t max);
uint16_t isqrt16(uint16_t n);
uint32_t isqrt32(uint32_t n);

#endif /* FIXEDMATH_H_ */
