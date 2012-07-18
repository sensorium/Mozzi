#ifndef FIXEDMATH_H_
#define FIXEDMATH_H_

#include "Arduino.h" // for "byte" - but is this depreciated?

/** @defgroup fixedMath Mozzi fixed point maths functions
Take care when converting that the important bits of your numbers will fit in the types you choose!
@note Timing: converting between fixed and float 10-12us, converting between fixed types about 1us.
*/

/** @ingroup fixedMath
@{
*/
// types
#define Q0n7 char 				/**< signed fractional number using 7 fractional bits, represents -0.5 to 0.496*/
#define Q0n8 unsigned char  		/**< unsigned fractional number using 8 fractional bits, represents 0.0 to 0.996*/
#define Q8n0 unsigned char  		/**< normal unsigned char with 0 fractional bits, represents 0.0 to 255.0*/
#define Q0n16 unsigned int  		/**< unsigned fractional number using 16 fractional bits, represents 0.0 to 0.999*/
#define Q7n8 int 				/**< signed fractional number using 7 integer bits and 8 fractional bits, represents -127.996 to 127.996*/
#define Q1n14 int				/**< signed fractional number using 1 integer bit and 14 fractional bits, represents -1.999 to 1.999*/
#define Q15n0 int				/**< signed number using 15 integer bits and 0 fractional bits, represents -32768 to 32767*/
#define Q8n8 unsigned int		/**< unsigned fractional number using 8 integer bits and 8 fractional bits, represents 0 to 255.996*/
#define Q1n15 unsigned int		/**< unsigned fractional number using 1 integer bit and 15 fractional bits, represents 0 to 1.999*/
#define Q16n0 unsigned int		/**< unsigned number using 16 integer bits and 0 fractional bits, represents 0 to 65536.0*/
#define Q23n8 long				/**< signed fractional number using 23 integer bits and 8 fractional bits, represents -8388607.996 to 8388607.996*/
#define Q15n16 long			/**< signed fractional number using 15 integer bits and 16 fractional bits, represents -32767.999 to 32767.999*/
#define Q31n0 long				/**< signed (normal long int) number using 31 integer bits and 0 fractional bits, represents -2147483648 to 2147483647*/
#define Q32n0 unsigned long		/**< unsigned (normal unsigned long int) number using 32 integer bits and 0 fractional bits, represents 0 to 4294967295*/
#define Q0n32 unsigned long		/**< unsigned (normal unsigned long int) number using 0 integer bits and 32 fractional bits, represents 0 to 0.999999999767169*/
#define Q8n24 unsigned long		/**< signed fractional number using 8 integer bits and 24 fractional bits, represents 0 to 255.999*/
#define Q24n8 unsigned long		/**< unsigned fractional number using 24 integer bits and 8 fractional bits, represents 0 to  16777215*/
#define Q16n16 unsigned long	/**< unsigned fractional number using 16 integer bits and 16 fractional bits, represents 0 to 65535.999*/

// macros to save runtime calculations for representations of 1
#define Q0n7_FIX1 ((Q0n7) 127)			/**< 0.992 in Q0n7 format*/
#define Q7n8_FIX1 ((Q7n8) 256)			/**< 1 in Q7n8 format*/
#define Q8n8_FIX1 ((Q8n8) 256)			/**< 1 in Q8n8 format*/
#define Q23n8_FIX1 ((Q23n8) 256)			/**< 1 in Q23n8 format*/
#define Q1n14_FIX1 ((Q1n14) 16384)		/**< 1 in Q1n14 format*/
#define Q1n15_FIX1 ((Q1n15) 32768)		/**< 1 in Q1n15 format*/
#define Q16n16_FIX1 ((Q16n16) 65536)		/**< 1 in Q16n16 format*/
#define Q0n16_FIX1 ((Q0n16) 65535)		/**< 0.999 in Q0n16 format*/
#define Q15n16_FIX1 ((Q15n16) 65536)		/**< 1 in Q15n16 format*/
#define Q8n24_FIX1 ((Q8n24) 16777216)	/**< 1 in Q8n24 format*/
#define Q0n32_FIX1 ((Q0n32) 4294967295)	/**< 0.999999999767169 in Q0n32 format*/



#define low15bits ((Q1n15) 32767) /**< Useful for keeping the lower 15 bits of a Q1n15 number, using &*/

// Type conversions:
/*
Float to Q

To convert a number from floating point to Qm.n format:

  Multiply the floating point number by 2^n
  Round to the nearest integer

Q to float

To convert a number from Qm.n format to floating point:

  Convert the number to floating point as if it were an integer
  Multiply by 2^-n
*/

#define float_to_Q0n7(a) ((char)((a)*256))		/**<Convert float to Q0n7 fix. @param a is a float*/
#define float_to_Q0n8(a) ((Q0n8)((a)*256))		/**<Convert float to Q0n8 fix. @param a is a float*/
#define float_to_Q7n8(a) ((Q7n8)((a)*256))		/**<Convert float to Q7n8 fix. @param a is a float*/
#define float_to_Q8n8(a) ((Q8n8)((a)*256))		/**<Convert float to Q8n8 fix. @param a is a float*/
#define float_to_Q1n14(a) ((Q1n14)((a)*16384))	/**<Convert float to Q1n14 fix. @param a is a float*/
#define float_to_Q1n15(a) ((Q1n15)((a)*32768))	/**<Convert float to Q1n15 fix. @param a is a float*/
#define float_to_Q8n24(a) ((Q8n24)((a)*16777216))	/**<Convert float to Q8n24 fix. @param a is a float*/
#define float_to_Q23n8(a) ((Q23n8)((a)*256))		/**<Convert float to Q23n8 fix. @param a is a float*/
#define float_to_Q24n8(a) ((Q24n8)((a)*256))		/**<Convert float to Q24n8 fix. @param a is a float*/
#define float_to_Q16n16(a) ((Q16n16)((a)*65536))	/**<Convert float to Q16n16 fix. @param a is a float*/
#define float_to_Q0n16(a) ((Q0n16)((a)*65536))	/**<Convert float to Q0n16 fix. @param a is a float*/
#define float_to_Q15n16(a) ((Q15n16)((a)*65536))	/**<Convert float to Q15n16 fix. @param a is a float*/

#define Q0n7_to_Q1n14(a) (((Q1n14)(a))<<7)		/**<Convert Q0n7 char to Q1n14 fix. @param a is a Q0n7 char */
#define Q0n7_to_Q15n16(a) (((Q15n16)(a))<<8)	/**<Convert Q0n7 signed char to Q15n16 fix. @param a is a Q0n7 signed char */
#define Q0n7_to_float(a) (((float)(a))/256)		/**<Convert Q0n7 fix to float. @param a is a Q0n7 char*/

#define Q0n8_to_Q1n15(a) (((Q1n15)(a))<<7)		/**<Convert Q0n8 unsigned char to Q1n15 fix. @param a is a Q0n8 unsigned char */
#define Q0n8_to_Q8n24(a) (((Q8n24)(a))<<16)	/**<Convert Q0n8 unsigned char to Q8n24 fix. @param a is a Q0n8 unsigned char */
#define Q0n8_to_Q24n8(a) ((Q24n8)(a))			/**<Convert Q0n8 unsigned char to Q24n8 fix. @param a is a Q0n8 unsigned char */
#define Q0n8_to_Q15n16(a) (((Q15n16)(a))<<8)	/**<Convert Q0n8 unsigned char to Q15n16 fix. @param a is a Q0n8 unsigned char */
#define Q0n8_to_Q16n16(a) (((Q16n16)(a))<<8)	/**<Convert Q0n8 unsigned char to Q16n16 fix. @param a is a Q0n8 unsigned char */
#define Q0n8_to_float(a) (((float)(a))/256)			/**<Convert Q0n8 fix to float. @param a is a Q0n8 unsigned char*/

#define Q7n0_to_Q7n8(a) (((Q7n8)(a))<<8)			/**<Convert char to Q7n8 fix. @param a is a char*/

#define Q8n0_to_Q8n8(a) (((Q8n8)(a))<<8)		/**<Convert unsigned char to Q8n8 fix. @param a is a Q0n8 unsigned char*/
#define Q8n0_to_Q16n16(a) (((Q16n16)(a))<<16)	/**<Convert Q8n0 unsigned char to Q16n16 fix. @param a is a Q8n0 unsigned char */

#define Q7n8_to_Q7n0(a) ((char)((a)>>8))		/**<Convert Q7n8 fix to char. @param a is a Q7n8 int*/
#define Q7n8_to_float(a) (((float)(a))/256)			/**<Convert Q7n8 fix to float. @param a is a Q7n8 int*/

#define Q8n8_to_Q8n0(a) ((unsigned char)((a)>>8))	/**<Convert Q8n8 fix to unsigned char. @param a is a Q8n8 unsigned int*/
#define Q8n8_to_float(a) (((float)(a))/256)			/**<Convert Q8n8 fix to float. @param a is a Q8n8 unsigned int*/

#define Q1n14_to_Q0n7(a) ((Q0n7)((a)>>7))			/**<Convert Q1n14 fixed to Q0n7 char. @param a is a Q1n14 int*/
#define Q1n14_to_float(a) (((float)(a))/16384)			/**<Convert fix to float. @param a is an int*/

#define Q1n15_to_Q0n8(a) ((Q0n8)((a)>>7))			/**<Convert Q1n15 fixed to Q0n8 unsigned char. Only for  positive values! @param a is a Q1n15 unsigned int*/
#define Q1n15_to_float(a) (((float)(a))/32768)			/**<Convert fix to float. @param a is a Q1n15 unsigned int*/

#define Q0n16_to_float(a) (((float)(a))/65536)			/**<Convert fix to float. @param a is a Q0n16 unsigned int*/

#define Q16n0_to_Q15n16(a) (((Q15n16)(a))<<16)		/**<Convert Q16n0 unsigned int to Q15n16 fix. @param a is a Q16n0 unsigned int */
#define Q16n0_to_Q23n8(a) (((Q23n8)(a))<<8)		/**<Convert Q16n0 unsigned int to Q23n8 fixed point signed long. @param a is a Q16n0 unsigned int*/
#define Q16n0_to_Q24n8(a) (((Q24n8)(a))<<8)		/**<Convert Q16n0 unsigned int to Q24n8 fixed point unsigned long. @param a is a Q16n0 unsigned int*/
#define Q16n0_to_Q16n16(a) (((Q16n16)(a))<<16)		/**<Convert Q16n0 unsigned int to Q16n16 fixed point unsigned long. @param a is a Q16n0 unsigned int*/
#define Q16n0_to_float(a) (((float)(a))/65536)			/**<Convert Q16n0 unsigned int to float. @param a is a Q16n0 unsigned int*/

#define Q8n24_to_Q0n8(a) ((Q0n8)((a)>>16))			/**<Convert Q8n24 fixed to Q0n8 unsigned char. @param a is a Q8n24 unsigned long*/
#define Q8n24_to_float(a) (((float)(a))/16777216)		/**<Convert fix to float. @param a is a Q8n24 unsigned long*/

#define Q23n8_to_Q31n0(a) ((Q31n0)((a)>>8))		/**<Convert Q23n8 fixed to Q31n0 long. @param a is a Q23n8 long*/
#define Q23n8_to_Q16n0(a) ((Q16n0)((a)>>8))		/**<Convert Q23n8 fixed to Q16n0 unsigned int. Positive values only. @param a is a Q23n8 long*/
#define Q23n8_to_Q15n0(a) ((Q15n0)((a)>>8))		/**<Convert Q23n8 fixed to Q15n0 signed int. @param a is a Q23n8 long*/
#define Q23n8_to_float(a) (((float)(a))/256)			/**<Convert fix to float. @param a is a Q23n8 signed long*/

#define Q24n8_to_Q0n8(a) ((Q0n8)(a))				/**<Convert Q24n8 fixed to Q0n8 unsigned char. @param a is a Q24n8 unsigned long*/
#define Q24n8_to_Q32n0(a) ((Q32n0)((a)>>8))		/**<Convert Q24n8 fixed to Q32n0 unsigned long. @param a is a Q24n8 unsigned long*/
#define Q24n8_to_float(a) (((float)(a))/256)			/**<Convert fix to float. @param a is a Q24n8 unsigned long*/

#define Q15n16_to_Q0n8(a) ((Q0n8)((a)>>8))			/**<Convert Q15n16 fixed to Q0n8 unsigned char.  Only for  positive values!  @param a is a Q15n16 signed long*/
#define Q15n16_to_Q15n0(a) ((Q15n0)((a)>>16))		/**<Convert Q15n16 fixed to Q15n0 signed int. @param a is a Q15n16 signed long*/
#define Q15n16_to_float(a) (((float)(a))/65536)		/**<Convert fix to float. @param a is a Q15n16 signed long*/

#define Q16n16_to_Q0n8(a) ((Q0n8)((a)>>8))			/**<Convert Q16n16 fixed to Q0n8 unsigned char. @param a is a Q16n16 unsigned long*/
#define Q16n16_to_Q16n0(a) ((Q16n0)((a)>>16))		/**<Convert Q16n16 fixed to Q16n0 unsigned int. @param a is a Q16n16 unsigned long*/
#define Q16n16_to_Q24n8(a) ((Q24n8)((a)>>8))		/**<Convert Q16n16 fixed to Q24n8 unsigned long. @param a is a Q16n16 unsigned long*/
#define Q16n16_to_float(a) (((float)(a))/65536)		/**<Convert fix to float. @param a is a Q16n16 unsigned long*/


/** Fast fixed point multiply for Q7n8 fractional numbers.
@param a Q7n8 format multiplicand
@param b Q7n8 format multiplier
*/
#define Q7n8_multfix(a,b)    	  \
({            \
int prod, val1=a, val2=b ;    \
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
	  );        \
  prod;        \
})

// based on:
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
int divfix(int nn, int dd)
begin
  int x, d ;
  signed char count, neg ;
  count = 0;
  neg = 0 ;
  d = dd ;

  // only works with + numbers
  if (d & 0x8000)
  begin
    neg = 1;
    d = -d ;
  end

  // range reduction
  while (d>0x0100)
  begin
    --count ;
    d >>= 1 ;
  end

  while (d<0x0080)
  begin
    ++count ;
    d <<= 1 ;
  end

  // Newton interation
  x = 0x02ea - (d<<1) ;
  x = multfix(x, 0x0200-multfix(d,x));
  //x = multfix(x, 0x0200-multfix(d,x));


  // range expansion
  if (count>0)  x = x<<count ;
  else if (count<0) x = x>>(-count) ;

  // fix sign
  if (neg==1) x=-x;

  //form ratio
  x = multfix(x,nn) ;

  return x ;
end

//========================================================
int sqrtfix(int aa)
begin

  int a;
  char nextbit, ahigh;
  int root, p ;
  a = aa;
  ahigh = a>>8 ;
  //
  // range sort to get integer part and to
  // check for weird bits near the top of the range
  if (ahigh >= 0x40)   //bigger than 64?
  begin
    if (a > 0x7e8f)  //>=126.562 = 11.25^2
    begin
    root = 0x0b40;  // 11
    nextbit = 0x10 ;
    end
    else if (ahigh >= 0x79)  //>=121
    begin
    root = 0x0b00;  // 11
    nextbit = 0x40 ;
    end
    else if (ahigh >= 0x64)  //>=100
    begin
    root = 0x0a00;  // 10
    nextbit = 0x80 ;
    end
    else if (ahigh >= 0x51)  //>=81
    begin
    root = 0x0900;  // 9
    nextbit = 0x80 ;
    end
    else //64
    begin
    root = 0x0800;  //8
    nextbit = 0x80 ;
    end
  end
  else if  (ahigh >= 0x10)  //16  //smaller than 64 and bigger then 16
  begin
    if (ahigh >= 0x31)  //49
    begin
    root = 0x0700;  //7
    nextbit = 0x80 ;
    end
    else if (ahigh >= 0x24)  //36
    begin
    root = 0x0600;  //6
    nextbit = 0x80 ;
    end
    else if (ahigh >= 0x19)  //25
    begin
    root = 0x0500;  //5
    nextbit = 0x80 ;
    end
    else //16
    begin
    root = 0x0400;  //4
    nextbit = 0x80 ;
    end
  end
  else   //smaller than 16
  begin
   if (ahigh >= 0x09)  //9
    begin
    root = 0x0300;  //3
    nextbit = 0x80 ;
    end
    else if (ahigh >= 0x04)  //4
    begin
    root = 0x0200;  //2
    nextbit = 0x80 ;
    end
    else if (ahigh >= 0x01)  //1
    begin
    root = 0x0100;  //1
    nextbit = 0x80 ;
    end
    else   //less than one
    begin
    root = 0;
    nextbit = 0x80 ;
    end
  end
  // now get the low order bits
  while (nextbit)
  begin
		root = nextbit + root;
		p =  multfix(root,root);
    if (p >= a) root = root - nextbit ;
  	nextbit = nextbit>>1 ;
  end
  return root ;
end
*/
/*
// from octosynth, Joe Marshall 2011:

  // multiply 2 16 bit numbers together and shift 8 without precision loss
  // requires assembler really
  volatile unsigned char zeroReg=0;
  volatile unsigned int multipliedCounter=oscillators[c].phaseStep;
  asm volatile
  (
  // high bytes mult together = high  byte
  "ldi %A[outVal],0" "\n\t"
  "mul %B[phaseStep],%B[pitchBend]" "\n\t"
  "mov %B[outVal],r0" "\n\t"
  // ignore overflow into r1 (should never overflow)
  // low byte * high byte -> both bytes
  "mul %A[phaseStep],%B[pitchBend]" "\n\t"
  "add %A[outVal],r0" "\n\t"
  // carry into high byte
  "adc %B[outVal],r1" "\n\t"
  // high byte* low byte -> both bytes
  "mul %B[phaseStep],%A[pitchBend]" "\n\t"
  "add %A[outVal],r0" "\n\t"
  // carry into high byte
  "adc %B[outVal],r1" "\n\t"
  // low byte * low byte -> round
  "mul %A[phaseStep],%A[pitchBend]" "\n\t"
  // the adc below is to round up based on high bit of low*low:
  "adc %A[outVal],r1" "\n\t"
  "adc %B[outVal],%[ZERO]" "\n\t"
  "clr r1" "\n\t"
  :[outVal] "=&d" (multipliedCounter)
  :[phaseStep] "d" (oscillators[c].phaseStep),[pitchBend] "d"( pitchBendMultiplier),[ZERO] "d" (zeroReg)
  :"r1","r0"
  );
  oscillators[c].phaseStep=multipliedCounter;

  */


// dangerous overflow-prone  int power function
int ipow(int base, int exp);



// Base 2 power, using fixed-point exponent
Q16n16 Q16n16_pow2(Q8n8 exponent);


byte byteMod(byte n, byte d);
byte byteDiv(byte n, byte d);
byte byteRnd(byte min, byte max);

/** @}
*/

#endif /* FIXEDMATH_H_ */