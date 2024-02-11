/*
 * FixMath.h
 *
 * Copyright 2023, Thomas Combriat and the Mozzi team
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons
 * Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */


/** This file implements two fixed point number classes. These numbers can have a fractional
    part but are actually standard integers under the hood which makes calculations with them
    efficient on platforms which do not have a FPU like most micro-controllers. These numbers can be
    signed (SFix) or unsigned (UFix).

    A fixed point number has its range defined by the number of bits encoding the integer part (NI 
    in the following) and its precision by the number of bits encoding the fractional part (NF). For UFix types, the integral part can hold values in [0,2^NI-1], for SFix types, the integral part can hold values in [-2^NI,2^NI-1]. The number of bits encoding the fractional can be considered as the precision of the number: given NF, the number of possible values in the  [0,1[ range will 2^NF. Hence, given NF, the resolution will be 1/(2^NF).

    Under the hood, these types will keep track of the maximum possible value they might hold (this is the RANGE template parameter), and, if only *SAFE* operations (see below) are used, will automatically adjust there NI and NF to accomodate the result of a operation. It will also try not to promote there internal type when possible, assuming that you use the complete range of a given type.

    The operations possible with these types can be divided into two categories:
    - the operations between FixMath types are all safe (aka won't overflow) and are the only one included by default
    - the operations between a FixMath and a native C type (int, float) are NOT safe and are not included by default. In order to activate them, you need to `#define MOZZI_FIXMATH_UNSAFE` before including FixMath.h.


    Like standard C(++) types, the fixed point numbers defined here are following some rules:
    - any fixed type can be converted to another *as long as the value can be represented in the destination type*. Casting to a bigger type in term of NI and NF is safe, but reducing NI can lead to an overflow if the new type cannot hold the integer value and reducing NF leads to a loss of precision.
    - Fixed types can be constructed from and converted to standard C types.
    - all operations between fixed point number is safe (it won't overflow) and preserve the precision. In particular:
    - only addition, subtraction and multiplication are implemented (this is a design choice, see below)
    - any operation between a signed and an unsigned leads to a signed number
    - resulting numbers will be casted to a type big enough to store the expected values. It follows that it is worth starting with types that are as small as possible to hold the initial value.
    - all operations between a fixed point number and a native type (int, float, uint) are *not* safe. If the resulting value cannot be represented in the fixed point type it will overflow. Only addition, subtraction, multiplication and right/left shift are implemented. These are only accessible activating the `MOZZI_FIXMATH_UNSAFE` set.
    - safe right/left shifts, which return the correct value in the correct type are implemented as .sR<shift>() and .sL<shift>() respectively, shift being the shifting amount.

    More specifically on the returned types of the operations between fixed point math types:
    - Additions:
      - UFix<NI,NF> + UFix<_NI,_NF> returns UFix<MAX(NI,_NI)+1,MAX(NF,_NF)> at worse
      - SFix<NI,NF> + SFix<_NI,_NF> returns SFix<MAX(NI,_NI)+1,MAX(NF,_NF)> at worse
      - UFix<NI,NF> + SFix<_NI,_NF> returns SFix<MAX(NI,_NI)+1,MAX(NF,_NF)> at worse
      - UFix<NI,NF> + anything_else (signed or not) returns UFix<NI,NF> (only available with `MOZZI_FIXMATH_UNSAFE`)
      - SFix<NI,NF> + anything_else (signed or not) returns SFix<NI,NF> (only available with `MOZZI_FIXMATH_UNSAFE`)
    - Subtractions:
      - UFix<NI,NF> - UFix<_NI,_NF> returns SFix<MAX(NI,_NI),MAX(NF,_NF)> at worse
      - SFix<NI,NF> - SFix<_NI,_NF> returns SFix<MAX(NI,_NI)+1,MAX(NF,_NF)> at worse
      - SFix<NI,NF> - UFix<_NI,_NF> returns SFix<MAX(NI,_NI)+1,MAX(NF,_NF)> at worse
      - UFix<NI,NF> - anything_else (signed or not) returns UFix<NI,NF> (only available with `MOZZI_FIXMATH_UNSAFE`)
      - SFix<NI,NF> - anything_else (signed or not) returns SFix<NI,NF> (only available with `MOZZI_FIXMATH_UNSAFE`)
      - (-)SFix<NI,NF> return SFix<NI,NF>
      - (-)UFix<NI,NF> return SFix<NI,NF>
    - Multiplications:
      - UFix<NI,NF> * UFix<_NI,_NF> returns UFix<NI+_NI,NF+_NF> at worse
      - UFix<NI,NF> * SFix<_NI,_NF> returns SFix<NI+_NI,NF+_NF> at worse
      - SFix<NI,NF> * SFix<_NI,_NF> returns SFix<NI+_NI,NF+_NF> at worse
      - UFix<NI,NF> * anything_else (signed or not) returns UFix<NI,NF> (only available with `MOZZI_FIXMATH_UNSAFE`)
      - SFix<NI,NF> * anything_else (signed or not) returns SFix<NI,NF> (only available with `MOZZI_FIXMATH_UNSAFE`)
    - Shifts:
      - UFix<NI,NF> .sR<NS> returns UFix<NI-NS,NF+NS>
      - UFix<NI,NF> .sL<NS> returns UFix<NI+NS,NF-NS>
      - same for SFix.
    - Inverse:
      - UFix<NI,NF>.invFast() returns the inverse of the number as UFix<NF,NI>
      - SFix<NI,NF>.invFast() returns the inverse of the number as SFix<NF,NI>
      - UFix<NI,NF>.invAccurate() returns the inverse of the number as UFix<NF,2*NI+NF>
      - SFix<NI,NF>.invAccurate() returns the inverse of the number as SFix<NF,2*NI+NF>
      - UFix<NI,NF>.inv<_NF>() returns the inverse of the number as UFix<NF,_NF>
      - SFix<NI,NF>.inv<_NF>() returns the inverse of the number as SFix<NF,_NF>
    - Conversion (should be preferred over casting, when possible):
      - UFix<NI,NF>.asSFix() returns SFix<NI,NF>
      - SFix<NI,NF>.asUFix() returns UFix<NI,NF>
      - UFix<NI,NF>.asFloat() returns the value as a float.
      - SFix<NI,NF>.asFloat() returns the value as a float.
      - UFix<NI,NF>.asRaw() returns the internal value.
      - SFix<NI,NF>.asRaw() returns the internal value.
      - T.toUFraction() returns UFix<0,NF> with NF the number of bits of T (uint8_t leads to NF=8bits).
      - T.toSFraction() returns SFix<0,NF> with NF the number of bits of T (int8_t leads to NF=7bits).
      - T.toUInt() returns UFix<NI,0> with NI the number of bits of T (uint8_t leads to NI=8bits).
      - T.toSInt() returns SFix<NI,> with NI the number of bits of T (int8_t leads to NI=7bits).   
    
    Note on division:
      The division is not implemented. This is a deliberate choice made for two reasons:
        - in contrast with all the other fundamental operations, it is not possible to guarantee that precision will be kept (other operations returns *exact* results whenever the operands were also exactly represented. Note that this is actually not the case when using normal floating point numbers. The inverse functions can be used to fake a division, by multiplying by the inverse of a number.
	- division are usually very slow operations on MCU, hence there usage is discouraged. The ideal way of doing it is to compute the inverse whenever needed and only when needed. In the context of Mozzi for instance, a good way to do it would be to compute needed inverses in updateControl(), and use them in updateAudio().
   
*/




#ifndef FIXMATH2_H_
#define FIXMATH2_H_

#if FOR_DOXYGEN_ONLY
#define MOZZI_FIXMATH_UNSAFE
#endif


#include<Arduino.h>
#include "IntegerType.h"
 

namespace MozziPrivate {
  template<typename T> constexpr T shiftR(T x, int8_t bits) {return (bits > 0 ? (x >> (bits)) : (x << (-bits)));} // shift right with positive values, left with negative
  constexpr int8_t sBitsToBytes(int8_t N) { return (((N)>>3)+1);}  // conversion between Bits and Bytes for signed
  constexpr int8_t uBitsToBytes(int8_t N) { return (((N-1)>>3)+1);}
  template<typename T>  constexpr T mozziMax(T N1, T N2) { return (N1) > (N2) ? (N1) : (N2);}
  template<typename T>  constexpr T mozziMin(T N1, T N2) { return (N1) > (N2) ? (N2) : (N1);}
  constexpr uint64_t sFullRange(int8_t N) { return uint64_t(1)<<N;} // maximum absolute value that can be hold in a signed of size N
  constexpr uint64_t uFullRange(int8_t N) { return ((uint64_t(1)<<(N-1))-1) + (uint64_t(1)<<(N-1));}
  constexpr uint64_t rangeAdd(byte NF, byte _NF, uint64_t RANGE, uint64_t _RANGE) { return ((NF > _NF) ? (RANGE + (_RANGE<<(NF-_NF))) : (_RANGE + (RANGE<<(_NF-NF))));}  // returns the RANGE following an addition
  constexpr int8_t neededNIExtra(int8_t NI, int8_t NF, uint64_t RANGE) { return (RANGE > (uFullRange(NI+NF)) ? (NI+1) : (RANGE > (uFullRange(NI+NF-1)) ? (NI) : (NI-1)));} // check if RANGE can be hold in the unsigned type defined by NI and NF
  constexpr int8_t neededSNIExtra(int8_t NI, int8_t NF, uint64_t RANGE) { return (RANGE > (sFullRange(NI+NF)) ? (NI+1) : (RANGE > (sFullRange(NI+NF-1)) ? (NI) : (NI-1)));}  // same for signed
  constexpr uint64_t rangeShift(int8_t N, int8_t SH, uint64_t RANGE) { return ((SH < N) ? (RANGE) : (shiftR(RANGE,(N-SH))));}  // make sure that NI or NF does not turn negative when safe shifts are used.
}

// Forward declaration
template<int8_t NI, int8_t NF, uint64_t RANGE=MozziPrivate::sFullRange(NI+NF)>
class SFix;



/** Instanciate an unsigned fixed point math number.
    @param NI The number of bits encoding the integer part. The integral part can range into [0, 2^NI -1]
    @param NF The number of bits encoding the fractional part
    @param RANGE A purely internal parameter that keeps track of the range used by the type. Safer to *not* define it.
*/
template<int8_t NI, int8_t NF, uint64_t RANGE=MozziPrivate::uFullRange(NI+NF)> // NI and NF being the number of bits for the integral and the fractionnal parts respectively.
class UFix
{
  static_assert(NI+NF<=64, "The total width of a UFix cannot exceed 64bits");
  typedef typename IntegerType<MozziPrivate::uBitsToBytes(NI+NF)>::unsigned_type internal_type ; // smallest size that fits our internal integer
  typedef typename IntegerType<MozziPrivate::uBitsToBytes(NI+NF+1)>::unsigned_type next_greater_type ; // smallest size that fits 1<<NF for sure (NI could be equal to 0). It can be the same than internal_type in some cases.
  
public:
  /** Constructor
   */
  UFix() {;}

  /** Constructor from a positive floating point value.
      @param fl Floating point value
      @return An unsigned fixed point number
  */
  UFix(float fl)  {internal_value = /*static_cast<internal_type>*/(fl * (next_greater_type(1) << NF));}

  /** Constructor from a floating point value.
      @param fl Floating point value
      @return An unsigned fixed point number
  */
  UFix(double fl)  {internal_value = static_cast<internal_type> (fl * (next_greater_type(1) << NF)); }
  
  /* Constructor from integer type (as_frac = false) or from fractionnal value (as_frac=true) can be used to emulate the behavior of for instance Q8n0_to_Q8n8 */

  /** Constructor from an integer value which can be interpreted as both a resulting fixed point 
      math number with a fractional part equals to 0, or as a number with decimal, ie as the underlying type behind the fixed point math number.
      @param value Integer value
      @param as_raw=false with false value will be interpreted as an integer, with true it will be interpreted as a number with decimals.
      @return An unsigned fixed point number
  */
  template<typename T>
  UFix(T value,bool as_raw=false)  {
    
    if (as_raw) internal_value = value;
    else internal_value = (internal_type(value) << NF);      
  }


  /** Set the internal value of the fixed point math number.
      @param raw The new internal value.
      @return An UFixx
  */  
  template<typename T>
  static UFix<NI,NF> fromRaw(T raw){return UFix<NI,NF>(raw,true);}





  /** Constructor from another UFix. 
      @param uf An unsigned fixed type number which value can be represented in this type.
      @return A unsigned fixed type number
  */
  template<int8_t _NI, int8_t _NF, uint64_t _RANGE>
  UFix(const UFix<_NI,_NF, _RANGE>& uf) {
    internal_value = MozziPrivate::shiftR((typename IntegerType<MozziPrivate::uBitsToBytes(MozziPrivate::mozziMax(NI+NF,_NI+_NF))>::unsigned_type) uf.asRaw(),(_NF-NF));
  }



  /** Constructor from a SFix. 
      @param uf An signed fixed type number which value can be represented in this type: sign is thus discarded.
      @return A unsigned fixed type number
  */
  template<int8_t _NI, int8_t _NF, uint64_t _RANGE>
  UFix(const SFix<_NI,_NF, _RANGE>& uf) {
    internal_value = MozziPrivate::shiftR((typename IntegerType<MozziPrivate::uBitsToBytes(MozziPrivate::mozziMax(NI+NF,_NI+_NF))>::unsigned_type) uf.asRaw(),(_NF-NF));
  }


  //////// ADDITION OVERLOADS
  
  /** Addition with another UFix. Safe.
      @param op The UFix to be added.
      @return The result of the addition as a UFix.
  */
   template<int8_t _NI, int8_t _NF, uint64_t _RANGE>
   UFix<MozziPrivate::neededNIExtra(MozziPrivate::mozziMax(NI,_NI),MozziPrivate::mozziMax(NF,_NF),MozziPrivate::rangeAdd(NF,_NF,RANGE,_RANGE)),MozziPrivate::mozziMax(NF, _NF), MozziPrivate::rangeAdd(NF,_NF,RANGE,_RANGE)> operator+ (const UFix<_NI,_NF,_RANGE>& op) const
  {
    constexpr uint64_t new_RANGE = MozziPrivate::rangeAdd(NF,_NF,RANGE,_RANGE);
    constexpr int8_t new_NI = MozziPrivate::neededNIExtra(MozziPrivate::mozziMax(NI,_NI),MozziPrivate::mozziMax(NF,_NF),new_RANGE);
    constexpr int8_t new_NF = MozziPrivate::mozziMax(NF, _NF);
    typedef typename IntegerType<MozziPrivate::uBitsToBytes(new_NI+new_NF)>::unsigned_type return_type;
    UFix<new_NI,new_NF> left(*this);
    UFix<new_NI,new_NF> right(op);

    return_type tt = return_type(left.asRaw()) + right.asRaw();
    return UFix<new_NI,new_NF,new_RANGE>(tt,true);
    }

    /** Addition with a SFix. Safe.
      @param op The UFix to be added.
      @return The result of the addition as a SFix.
  */
   template<int8_t _NI, int8_t _NF, uint64_t _RANGE>
   SFix<MozziPrivate::neededSNIExtra(MozziPrivate::mozziMax(NI,_NI),MozziPrivate::mozziMax(NF,_NF),MozziPrivate::rangeAdd(NF,_NF,RANGE,_RANGE)),MozziPrivate::mozziMax(NF, _NF), MozziPrivate::rangeAdd(NF,_NF,RANGE,_RANGE)> operator+ (const SFix<_NI,_NF,_RANGE>& op) const
  {
    constexpr uint64_t new_RANGE = MozziPrivate::rangeAdd(NF,_NF,RANGE,_RANGE);
    constexpr int8_t new_NI = MozziPrivate::neededSNIExtra(MozziPrivate::mozziMax(NI,_NI),MozziPrivate::mozziMax(NF,_NF),new_RANGE);
    constexpr int8_t new_NF = MozziPrivate::mozziMax(NF, _NF);
    typedef typename IntegerType<MozziPrivate::uBitsToBytes(new_NI+new_NF)>::signed_type return_type;
    SFix<new_NI,new_NF> left(*this);
    SFix<new_NI,new_NF> right(op);

    return_type tt = return_type(left.asRaw()) + right.asRaw();
    return SFix<new_NI,new_NF,new_RANGE>(tt,true);
    }
  
#ifdef MOZZI_FIXMATH_UNSAFE
  /** Addition with another type.
      @note Unsafe. Only available with `MOZZI_FIXMATH_UNSAFE`
      @param op The number to be added.
      @return The result of the addition as a UFix.
  */
  template<typename T>
  UFix<NI,NF> operator+ (const T op) const
  {
    return UFix<NI,NF>(internal_value+((internal_type)op<<NF),true);
  }
#endif

  //////// SUBSTRACTION OVERLOADS

  /** Subtraction with another UFix. Safe.
      @param op The UFix to be subtracted.
      @return The result of the subtraction as a SFix.
  */
  template<int8_t _NI, int8_t _NF, uint64_t _RANGE> // We do not have the +1 after MozziPrivate::mozziMax(NI, _NI) because the substraction between two UFix should fit in the biggest of the two.
  SFix<MozziPrivate::mozziMax(NI,_NI),MozziPrivate::mozziMax(NF,_NF), MozziPrivate::mozziMax(MozziPrivate::shiftR(RANGE,MozziPrivate::mozziMax(NF,_NF)-NF), MozziPrivate::shiftR(_RANGE,MozziPrivate::mozziMax(NF,_NF)-_NF))> operator- (const UFix<_NI,_NF, _RANGE>& op) const
  {
    constexpr int8_t new_NI = MozziPrivate::mozziMax(NI, _NI);
    constexpr int8_t new_NF = MozziPrivate::mozziMax(NF, _NF);
    typedef typename IntegerType<MozziPrivate::sBitsToBytes(new_NI+new_NF)>::signed_type return_type;
    SFix<new_NI,new_NF> left(*this);
    SFix<new_NI,new_NF> right(op);

    return_type tt = return_type(left.asRaw()) - right.asRaw();
    return SFix<new_NI,new_NF,MozziPrivate::mozziMax(MozziPrivate::shiftR(RANGE,MozziPrivate::mozziMax(NF,_NF)-NF), MozziPrivate::shiftR(_RANGE,MozziPrivate::mozziMax(NF,_NF)-_NF))>(tt,true);
  }

  #ifdef MOZZI_FIXMATH_UNSAFE
  /** Subtraction with another type.
      @note Unsafe. Only available with `MOZZI_FIXMATH_UNSAFE`
      @param op The number to be subtracted.
      @return The result of the subtraction as a UFix.
  */
  template<typename T>
  UFix<NI,NF> operator- (const T op) const
  {
    return UFix<NI,NF>(internal_value-((internal_type)op<<NF),true);
  }
#endif

  /** Opposite of the number.
      @return The opposite number as a SFix.
  */
  SFix<NI,NF,RANGE> operator-() const
  {
    return SFix<NI,NF,RANGE>( -(typename IntegerType<MozziPrivate::sBitsToBytes(NI+NF)>::signed_type)(internal_value),true);
  }

  //////// MULTIPLICATION OVERLOADS
  
  /** Multiplication with another UFix. Safe.
      @param op The UFix to be multiplied.
      @return The result of the multiplication as a UFix.
  */
  template<int8_t _NI, int8_t _NF, uint64_t _RANGE>
  UFix<MozziPrivate::neededNIExtra(NI+_NI, NF+_NF, RANGE*_RANGE),NF+_NF, RANGE*_RANGE> operator* (const UFix<_NI,_NF,_RANGE>& op) const 
  {
    constexpr int8_t NEW_NI = MozziPrivate::neededNIExtra(NI+_NI, NF+_NF, RANGE*_RANGE);
    typedef typename IntegerType<MozziPrivate::uBitsToBytes(NEW_NI+NF+_NF)>::unsigned_type return_type ;
    return_type tt = return_type(internal_value)*op.asRaw();
    return UFix<NEW_NI,(NF+_NF),RANGE*_RANGE>(tt,true);
  }

    /** Multiplication with a SFix. Safe.
      @param op The SFix to be multiplied.
      @return The result of the multiplication as a SFix.
  */
  template<int8_t _NI, int8_t _NF, uint64_t _RANGE>
  SFix<MozziPrivate::neededSNIExtra(NI+_NI, NF+_NF, RANGE*_RANGE),NF+_NF, RANGE*_RANGE> operator* (const SFix<_NI,_NF,_RANGE>& op) const 
  {
    constexpr int8_t NEW_NI = MozziPrivate::neededSNIExtra(NI+_NI, NF+_NF, RANGE*_RANGE);
    typedef typename IntegerType<MozziPrivate::sBitsToBytes(NEW_NI+NF+_NF)>::signed_type return_type ;
    return_type tt = return_type(internal_value)*op.asRaw();
    return SFix<NEW_NI,(NF+_NF),RANGE*_RANGE>(tt,true);
  }

  #ifdef MOZZI_FIXMATH_UNSAFE
  /** Multiplication with another type.
      @note Unsafe. Only available with `MOZZI_FIXMATH_UNSAFE`
      @param op The number to be multiplied.
      @return The result of the multiplication as a UFix of identical NI and NF
  */
  template<typename T>
  UFix<NI,NF> operator* (const T op) const
  {
    return UFix<NI,NF>(internal_value*op,true);
  }
#endif

  //////// INVERSE

  /** Compute the inverse of a UFix<NI,NF>, as a UFix<NF,NI> (not a typo).
      This inverse is able to represent accurately the inverse of the smallest and biggest of the initial number, but might lead to a lot of duplicates in between.
      Good if precision is not a premium but type conservation and speeds are. 
      This is still slower than a multiplication, hence the suggested workflow is to compute the inverse when time is not critical, for instance in updateControl(), and multiply it afterward, for instance in updateAudio(), if you need a division.
      @return The inverse of the number.
  */
  UFix<NF,NI> invFast() const
  {
    static_assert(NI+NF<=63, "The fast inverse cannot be computed for when NI+NF>63. Reduce the number of bits.");
    return UFix<NF,NI>((onesbitmask()/internal_value),true);
  }


  /** Compute the inverse of a UFix<NI,NF>, as a UFix<NF,_NF>.
      _NF is the number of precision bits for the output. Can be any number but especially useful for case between invFast() (_NF=NI) and invAccurate() (_NF=2*NI+NF)
      @return The inverse of the number.
  */
  template<int8_t _NF>
  UFix<NF,_NF> inv() const
  {
    return UFix<_NF,NF>(internal_value,true).invFast();  
  }


  
  /** Compute the inverse of a UFix<NI,NF>, as a UFix<NF,NI*2+NF>.
      This inverse is more accurate than invFast, and usually leads to non common values on the whole input range. This comes at the cost of a way bigger resulting type.
      This is still slower than a multiplication, hence the suggested workflow is to compute the inverse when time is not critical, for instance in updateControl(), and multiply it afterward, for instance in updateAudio(), if you need a division.
      @return The inverse of the number.
  */
  UFix<NF,MozziPrivate::mozziMin(NI*2+NF,63-NF)> invAccurate() const // The MozziPrivate::mozziMin is just to remove compiler error when a big FixMath is instanciated but no accurate inverse is actually computed (this would be catch by the static_assert)
  {
    static_assert(2*NI+2*NF<=63, "The accurate inverse cannot be computed for when 2*NI+2*NF>63. Reduce the number of bits.");
    return inv<NI*2+NF>();
  }


  
  


  //////// SHIFTS OVERLOADS

  /** Right shift. This won't overflow but will not leverage on the bits freed by the shift.
      Better to use .sR<shift>() if possible instead.
      @param op The shift number
      @return The result of the shift as a UFix.
  */
  UFix<NI,NF> operator>> (const int8_t op) const
  {
    return UFix<NI,NF>(internal_value>>op,true);
  }


  #ifdef MOZZI_FIXMATH_UNSAFE
  /** Left shift. This can overflow if you shift to a value that cannot be represented.
      Better to use .sL<shift>() if possible instead.
      @note Unsafe. Only available with `MOZZI_FIXMATH_UNSAFE`
      @param op The shift number
      @return The result of the shift as a UFix.
  */
  UFix<NI,NF> operator<< (const int8_t op) const
  {
    return UFix<NI,NF>(internal_value<<op,true);
  }
#endif
  
  /** Safe and optimal right shift. The returned type will be adjusted accordingly
      @param op The shift number
      @return The result of the shift as a UFix of smaller size.
  */
  template<int8_t op>
  UFix<MozziPrivate::mozziMax(NI-op,0),NF+op, MozziPrivate::rangeShift(NI,op,RANGE)> sR()
  {
    return UFix<MozziPrivate::mozziMax(NI-op,0),NF+op,MozziPrivate::rangeShift(NI,op,RANGE)>(internal_value,true);
  }

  /** Safe and optimal left shift. The returned type will be adjusted accordingly
      @param op The shift number
      @return The result of the shift as a UFix of bigger size.
  */
  template<int8_t op>
  UFix<NI+op,MozziPrivate::mozziMax(NF-op,0),MozziPrivate::rangeShift(NF,op,RANGE)> sL()
  {
    return UFix<NI+op,MozziPrivate::mozziMax(NF-op,0)>(internal_value,true);
  }

  
  //////// COMPARISON OVERLOADS
  /** Comparison with another UFix.
      @param op A UFix
      @return true if this is bigger than op, false otherwise
  */
  template<int8_t _NI, int8_t _NF>
  bool operator> (const UFix<_NI,_NF>& op) const
  {
    constexpr int8_t new_NI = MozziPrivate::mozziMax(NI, _NI);
    constexpr int8_t new_NF = MozziPrivate::mozziMax(NF, _NF);
    UFix<new_NI,new_NF> left(*this);
    UFix<new_NI,new_NF> right(op);
    return left.asRaw()>right.asRaw();
  }

  /** Comparison with another UFix.
      @param op A UFix
      @return true if this is smaller than op, false otherwise
  */
  template<int8_t _NI, int8_t _NF>
  bool operator< (const UFix<_NI,_NF>& op) const
  {
    return op > *this;
  }


  /** Comparison with another UFix.
      @param op A UFix
      @return true if this equal to op, false otherwise
  */
  template<int8_t _NI, int8_t _NF>
  bool operator== (const UFix<_NI,_NF>& op) const
  {
    constexpr int8_t new_NI = MozziPrivate::mozziMax(NI, _NI);
    constexpr int8_t new_NF = MozziPrivate::mozziMax(NF, _NF);
    UFix<new_NI,new_NF> left(*this);
    UFix<new_NI,new_NF> right(op);
    return left.asRaw()==right.asRaw();
  }

  /** Comparison with another UFix.
      @param op A UFix
      @return true if this not equal to op, false otherwise
  */
  template<int8_t _NI, int8_t _NF>
  bool operator!= (const UFix<_NI,_NF>& op) const
  {
    constexpr int8_t new_NI = MozziPrivate::mozziMax(NI, _NI);
    constexpr int8_t new_NF = MozziPrivate::mozziMax(NF, _NF);
    UFix<new_NI,new_NF> left(*this);
    UFix<new_NI,new_NF> right(op);
    return left.asRaw()!=right.asRaw();
  }
  
  /** Returns the number as a SFix of same range and precision. This is more optimized than a cast.
      @return a SFix 
  */
  SFix<NI,NF,RANGE> asSFix() const
  {
    return SFix<NI,NF,RANGE>(internal_value,true);
  }

  /** Returns the value as floating point number.
      @return The floating point value.
  */
  float asFloat() const { return (static_cast<float>(internal_value)) / (next_greater_type(1)<<NF); }
  
  /** Returns the internal integer value
      @return the internal value
  */
  internal_type asRaw() const { return internal_value; }
  
  /** The number of bits used to encode the integral part.
      @return The number of bits used to encode the integral part.
  */
  int8_t getNI() const {return NI;}

  /** The number of bits used to encode the fractional part.
      @return The number of bits used to encode the fractional part.
  */
  int8_t getNF() const {return NF;}
    
private:
  internal_type internal_value;
  static constexpr internal_type onesbitmask() { return (internal_type) ((1ULL<< (NI+NF)) - 1); }
};


#ifdef MOZZI_FIXMATH_UNSAFE
// Multiplication
template <int8_t NI, int8_t NF>
inline UFix<NI, NF> operator*(uint8_t op, const UFix<NI, NF>& uf) {return uf*op;}

template <int8_t NI, int8_t NF>
inline UFix<NI, NF> operator*(uint16_t op, const UFix<NI, NF>& uf) {return uf*op;}

template <int8_t NI, int8_t NF>
inline UFix<NI, NF> operator*(uint32_t op, const UFix<NI, NF>& uf) {return uf*op;}

template <int8_t NI, int8_t NF>
inline UFix<NI, NF> operator*(uint64_t op, const UFix<NI, NF>& uf) {return uf*op;}

template <int8_t NI, int8_t NF>
inline UFix<NI, NF> operator*(int8_t op, const UFix<NI, NF>& uf) {return uf*op;}

template <int8_t NI, int8_t NF>
inline UFix<NI, NF> operator*(int16_t op, const UFix<NI, NF>& uf) {return uf*op;}

template <int8_t NI, int8_t NF>
inline UFix<NI, NF> operator*(int32_t op, const UFix<NI, NF>& uf) {return uf*op;}

template <int8_t NI, int8_t NF>
inline UFix<NI, NF> operator*(int64_t op, const UFix<NI, NF>& uf) {return uf*op;}

template <int8_t NI, int8_t NF>
inline UFix<NI, NF> operator*(float op, const UFix<NI, NF>& uf) {return uf*op;}

template <int8_t NI, int8_t NF>
inline UFix<NI, NF> operator*(double op, const UFix<NI, NF>& uf) {return uf*op;}

// Addition
template <int8_t NI, int8_t NF>
inline UFix<NI, NF> operator+(uint8_t op, const UFix<NI, NF>& uf) {return uf+op;}

template <int8_t NI, int8_t NF>
inline UFix<NI, NF> operator+(uint16_t op, const UFix<NI, NF>& uf) {return uf+op;}

template <int8_t NI, int8_t NF>
inline UFix<NI, NF> operator+(uint32_t op, const UFix<NI, NF>& uf) {return uf+op;}

template <int8_t NI, int8_t NF>
inline UFix<NI, NF> operator+(uint64_t op, const UFix<NI, NF>& uf) {return uf+op;}

template <int8_t NI, int8_t NF>
inline UFix<NI, NF> operator+(int8_t op, const UFix<NI, NF>& uf) {return uf+op;}

template <int8_t NI, int8_t NF>
inline UFix<NI, NF> operator+(int16_t op, const UFix<NI, NF>& uf) {return uf+op;}

template <int8_t NI, int8_t NF>
inline UFix<NI, NF> operator+(int32_t op, const UFix<NI, NF>& uf) {return uf+op;}

template <int8_t NI, int8_t NF>
inline UFix<NI, NF> operator+(int64_t op, const UFix<NI, NF>& uf) {return uf+op;}

template <int8_t NI, int8_t NF>
inline UFix<NI, NF> operator+(float op, const UFix<NI, NF>& uf) {return uf+op;}

template <int8_t NI, int8_t NF>
inline UFix<NI, NF> operator+(double op, const UFix<NI, NF>& uf) {return uf+op;}

// Substraction
template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator-(uint8_t op, const UFix<NI, NF>& uf) {return -uf+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator-(uint16_t op, const UFix<NI, NF>& uf) {return -uf+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator-(uint32_t op, const UFix<NI, NF>& uf) {return -uf+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator-(uint64_t op, const UFix<NI, NF>& uf) {return -uf+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator-(int8_t op, const UFix<NI, NF>& uf) {return -uf+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator-(int16_t op, const UFix<NI, NF>& uf) {return -uf+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator-(int32_t op, const UFix<NI, NF>& uf) {return -uf+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator-(int64_t op, const UFix<NI, NF>& uf) {return -uf+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator-(float op, const UFix<NI, NF>& uf) {return -uf+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator-(double op, const UFix<NI, NF>& uf) {return -uf+op;}
#endif
////// Helper functions to build SFix from a normal type automatically


/** Create a *pure* fractional unsigned fixed number (UFix) from an unsigned integer.
    The number of fractional bits (NF) is chosen automatically depending on the input 
    type. Hence toUFraction(255) and toUFraction(uint8_t(255)) *do not* lead to the 
    same thing: on an AVR, the former will lead to NF=16 - which is overkill and 
    incorrect if you expect toUFraction(255) = 1 -
    whereas the latter will lead to NF=8. Mozzi's objects (Oscil and the like)
    returns correct types, hence you can use this function to convert the return
    value of a Mozzi's function/class member into a pure fractional number.
    @param val The value to be converted into a pure fractional number.
    @return A UFix<0,NF> with NF chosen according to the input type
*/
template<typename T>
inline UFix<0, sizeof(T)*8> toUFraction(T val) {
  return UFix<0, sizeof(T)*8>::fromRaw(val); 
}

/** Create a *pure* integer unsigned fixed number (UFix) from an unsigned integer.
    The number of fractional bits (NI) is chosen automatically depending on the input 
    type. Hence toUInt(255) and toUInt(uint8_t(255)) *do not* lead to the 
    same thing: on an AVR, the former will lead to NI=16 - which is overkill -
    whereas the latter will lead to NI=8. Mozzi's objects (Oscil and the like)
    returns correct types, hence you can use this function to convert the return
    value of a Mozzi's function/class member into a pure fractional number.
    @param val The value to be converted into a pure unsigned integer fixed math number.
    @return A UFix<NI,0> with NI chosen according to the input type
*/
template<typename T>
inline UFix<sizeof(T)*8-1,0> toUInt(T val) {
  return UFix<sizeof(T)*8-1,0>::fromRaw(val); 
}



/** Instanciate an signed fixed point math number.
    @param NI The number of bits encoding the integer part. The integral part can range into [-2^NI, 2^NI -1]
    @param NF The number of bits encoding the fractional part
    @param RANGE A purely internal parameter that keeps track of the range used by the type. Safer to *not* define it. 
    @note The total number of the underlying int will be NI+NF+1 in order to accomodate the sign. It follows that, if you want something that reproduces the behavior of a int8_t, it should be declared as SFix<7,0>.
*/
template<int8_t NI, int8_t NF, uint64_t RANGE> // NI and NF being the number of bits for the integral and the fractionnal parts respectively.
class SFix
{
  static_assert(NI+NF<64, "The total width of a SFix cannot exceed 63bits");
  typedef typename IntegerType<MozziPrivate::sBitsToBytes(NI+NF)>::signed_type internal_type ; // smallest size that fits our internal integer
  typedef typename IntegerType<MozziPrivate::sBitsToBytes(NI+NF+1)>::signed_type next_greater_type ; // smallest size that fits 1<<NF for sure (NI could be equal to 0). It can be the same than internal_type in some cases.
  
public:
  /** Constructor
   */
  SFix() {;}
  
  /** Constructor from a floating point value.
      @param fl Floating point value
      @return An signed fixed point number
  */
  SFix(float fl)  {internal_value = /*static_cast<internal_type>*/(fl * (next_greater_type(1) << NF));}

  /** Constructor from a floating point value.
      @param fl Floating point value
      @return An signed fixed point number
  */
  SFix(double fl)  {internal_value = static_cast<internal_type> (fl * (next_greater_type(1) << NF)); }


  /** Constructor from an integer value which can be interpreted as both a resulting fixed point 
      math number with a fractional part equals to 0, or as a number with decimal, ie as the underlying type behind the fixed point math number.
      @param value Integer value
      @param as_raw=false with false value will be interpreted as an integer, with true it will be interpreted as a number with decimals.
      @return An signed fixed point number
  */
  template<typename T>
  SFix(T value,bool as_raw=false)
  {
    if (as_raw) internal_value = value;
    else internal_value = (internal_type(value) << NF);
  }

  /** Set the internal value of the fixed point math number.
      @param raw The new internal value.
      @return A SFix.
  */  
  template<typename T>
  static SFix<NI,NF> fromRaw(T raw){return SFix<NI,NF>(raw,true);}


  /** Constructor from another SFix. 
      @param uf A signed fixed type number which value can be represented in this type.
      @return A signed fixed type number
  */
  template<int8_t _NI, int8_t _NF, uint64_t _RANGE>
  SFix(const SFix<_NI,_NF, _RANGE>& uf) {
    internal_value = MozziPrivate::shiftR((typename IntegerType<MozziPrivate::sBitsToBytes(MozziPrivate::mozziMax(NI+NF,_NI+_NF))>::signed_type) uf.asRaw(),(_NF-NF));
    
  }

  /** Constructor from an UFix. 
      @param uf A unsigned fixed type number which value can be represented in this type.
      @return A signed fixed type number
  */
  template<int8_t _NI, int8_t _NF, uint64_t _RANGE>
  SFix(const UFix<_NI,_NF, _RANGE>& uf) {
    internal_value = MozziPrivate::shiftR((typename IntegerType<MozziPrivate::uBitsToBytes(MozziPrivate::mozziMax(NI+NF,_NI+_NF))>::unsigned_type) uf.asRaw(),(_NF-NF));
  }

  //////// ADDITION OVERLOADS

  /** Addition with another SFix. Safe.
      @param op The SFix to be added.
      @return The result of the addition as a SFix.
  */
  template<int8_t _NI, int8_t _NF, uint64_t _RANGE>
  SFix<MozziPrivate::neededSNIExtra(MozziPrivate::mozziMax(NI,_NI),MozziPrivate::mozziMax(NF,_NF),MozziPrivate::rangeAdd(NF,_NF,RANGE,_RANGE)),MozziPrivate::mozziMax(NF, _NF), MozziPrivate::rangeAdd(NF,_NF,RANGE,_RANGE)> operator+ (const SFix<_NI,_NF,_RANGE>& op) const
  {
    constexpr uint64_t new_RANGE = MozziPrivate::rangeAdd(NF,_NF,RANGE,_RANGE);
    constexpr int8_t new_NI = MozziPrivate::neededSNIExtra(MozziPrivate::mozziMax(NI,_NI),MozziPrivate::mozziMax(NF,_NF),new_RANGE);
    constexpr int8_t new_NF = MozziPrivate::mozziMax(NF, _NF);
    typedef typename IntegerType<MozziPrivate::sBitsToBytes(new_NI+new_NF)>::signed_type return_type;
    SFix<new_NI,new_NF> left(*this);
    SFix<new_NI,new_NF> right(op);

    return_type tt = return_type(left.asRaw()) + right.asRaw();
    return SFix<new_NI, new_NF, new_RANGE>(tt,true);
  }
  
#ifdef MOZZI_FIXMATH_UNSAFE
  /** Addition with another type.
      @note Unsafe. Only available with `MOZZI_FIXMATH_UNSAFE`
      @param op The number to be added.
      @return The result of the addition as a UFix.
  */
  template<typename T>
  SFix<NI,NF> operator+ (const T op) const
  {
    return SFix<NI,NF>(internal_value+(op<<NF),true);
  }
#endif

  //////// SUBSTRACTION OVERLOADS

  /** Subtraction with another SFix. Safe.
      @param op The SFix to be subtracted.
      @return The result of the subtraction as a SFix.
  */ 
  template<int8_t _NI, int8_t _NF, uint64_t _RANGE>
  SFix<MozziPrivate::neededSNIExtra(MozziPrivate::mozziMax(NI,_NI),MozziPrivate::mozziMax(NF,_NF),MozziPrivate::rangeAdd(NF,_NF,RANGE,_RANGE)),MozziPrivate::mozziMax(NF, _NF), MozziPrivate::rangeAdd(NF,_NF,RANGE,_RANGE)> operator- (const SFix<_NI,_NF, _RANGE>& op) const
  {
    constexpr uint64_t new_RANGE = MozziPrivate::rangeAdd(NF,_NF,RANGE,_RANGE);
    constexpr int8_t new_NI = MozziPrivate::neededSNIExtra(MozziPrivate::mozziMax(NI,_NI),MozziPrivate::mozziMax(NF,_NF),new_RANGE);
    constexpr int8_t new_NF = MozziPrivate::mozziMax(NF, _NF);
    typedef typename IntegerType<MozziPrivate::sBitsToBytes(new_NI+new_NF)>::signed_type return_type;
    SFix<new_NI,new_NF> left(*this);
    SFix<new_NI,new_NF> right(op);
    return_type tt = return_type(left.asRaw()) - return_type(right.asRaw());
    return SFix<new_NI, new_NF, new_RANGE>(tt,true);
  }

    /** Subtraction with a UFix. Safe.
      @param op The SFix to be subtracted.
      @return The result of the subtraction as a SFix.
  */ 
  template<int8_t _NI, int8_t _NF, uint64_t _RANGE>
  SFix<MozziPrivate::neededSNIExtra(MozziPrivate::mozziMax(NI,_NI),MozziPrivate::mozziMax(NF,_NF),MozziPrivate::rangeAdd(NF,_NF,RANGE,_RANGE)),MozziPrivate::mozziMax(NF, _NF), MozziPrivate::rangeAdd(NF,_NF,RANGE,_RANGE)> operator- (const UFix<_NI,_NF, _RANGE>& op) const
  {
    constexpr uint64_t new_RANGE = MozziPrivate::rangeAdd(NF,_NF,RANGE,_RANGE);
    constexpr int8_t new_NI = MozziPrivate::neededSNIExtra(MozziPrivate::mozziMax(NI,_NI),MozziPrivate::mozziMax(NF,_NF),new_RANGE);
    constexpr int8_t new_NF = MozziPrivate::mozziMax(NF, _NF);
    typedef typename IntegerType<MozziPrivate::sBitsToBytes(new_NI+new_NF)>::signed_type return_type;
    SFix<new_NI,new_NF> left(*this);
    SFix<new_NI,new_NF> right(op);
    return_type tt = return_type(left.asRaw()) - return_type(right.asRaw());
    return SFix<new_NI, new_NF, new_RANGE>(tt,true);
  }

  #ifdef MOZZI_FIXMATH_UNSAFE
  /** Subtraction with another type.
      @note Unsafe. Only available with `MOZZI_FIXMATH_UNSAFE`
      @param op The number to be subtracted.
      @return The result of the subtraction as a SFix.
  */
   template<typename T>
  SFix<NI,NF> operator- (const T op) const
  {
    return SFix<NI,NF>(internal_value-(op<<NF),true);
    }
#endif

  /** Opposite of the number.
      @return The opposite numberas a SFix.
  */
  SFix<NI,NF,RANGE> operator-() const
  {
    return SFix<NI,NF,RANGE>(-internal_value,true);
  }
  
  //////// MULTIPLICATION OVERLOADS
  
  /** Multiplication with another SFix. Safe.
      @param op The SFix to be multiplied.
      @return The result of the multiplication as a SFix.
  */
  template<int8_t _NI, int8_t _NF, uint64_t _RANGE>
  SFix<MozziPrivate::neededSNIExtra(NI+_NI, NF+_NF, RANGE*_RANGE),NF+_NF, RANGE*_RANGE> operator* (const SFix<_NI,_NF,_RANGE>& op) const
  {
    constexpr int8_t NEW_NI = MozziPrivate::neededSNIExtra(NI+_NI, NF+_NF, RANGE*_RANGE);
    typedef typename IntegerType<MozziPrivate::sBitsToBytes(NI+_NI+NF+_NF)>::signed_type return_type ;
    return_type tt = return_type(internal_value)*op.asRaw();
    return SFix<NI+_NI,NF+_NF>(tt,true);
  }

  #ifdef MOZZI_FIXMATH_UNSAFE
  /** Multiplication with another type.
      @note Unsafe. Only available with `MOZZI_FIXMATH_UNSAFE`
      @param op The number to be multiplied.
      @return The result of the multiplication as a UFix.
  */
  template<typename T>
  SFix<NI,NF> operator* (const T op) const
  {
    return SFix<NI,NF>(internal_value*op,true);
  }
#endif


  //////// INVERSE

  /** Compute the inverse of a SFix<NI,NF>, as a SFix<NF,NI> (not a typo).
      This inverse is able to represent accurately the inverse of the smallest and biggest of the initial number, but might lead to a lot of duplicates in between.
      Good if precision is not a premium but type conservation and speeds are. 
      This is still slower than a multiplication, hence the suggested workflow is to compute the inverse when time is not critical, for instance in updateControl(), and multiply it afterward, for instance in updateAudio(), if you need a division.
      @return The inverse of the number.
  */

  SFix<NF,NI> invFast() const
  {
    static_assert(NI+NF<=62, "The fast inverse cannot be computed for when NI+NF>63. Reduce the number of bits.");
    return SFix<NF,NI>((onesbitmask()/internal_value),true);
  }

  /** Compute the inverse of a SFix<NI,NF>, as a SFix<NF,_NF>.
      _NF is the number of precision bits for the output. Can be any number but especially useful for case between invFast() (_NF=NI) and invAccurate() (_NF=2*NI+NF)
      @return The inverse of the number.
  */
  template<int8_t _NF>
  SFix<NF,_NF> inv() const
  {
    return SFix<_NF,NF>(internal_value,true).invFast();  
  }

  /** Compute the inverse of a SFix<NI,NF>, as a SFix<NF,NI*2+NF>.
      This inverse is more accurate than invFast, and usually leads to non common values on the whole input range. This comes at the cost of a way bigger resulting type.
      This is still slower than a multiplication, hence the suggested workflow is to compute the inverse when time is not critical, for instance in updateControl(), and multiply it afterward, for instance in updateAudio(), if you need a division.
      @return The inverse of the number.
  */
  SFix<NF,MozziPrivate::mozziMin(NI*2+NF,62-NF)> invAccurate() const
  {
    return inv<NI*2+NF>();
    }


  //////// SHIFTS OVERLOADS

  /** Right shift. This won't overflow but will not leverage on the bits freed by the shift.
      Better to use .sR<shift>() if possible instead.
      @param op The shift number
      @return The result of the shift as a SFix.
  */
  SFix<NI,NF> operator>> (const int8_t op) const
  {
    return SFix<NI,NF>(internal_value>>op,true);
  }

  #ifdef MOZZI_FIXMATH_UNSAFE
  /** Left shift. This can overflow if you shift to a value that cannot be represented.
      Better to use .sL<shift>() if possible instead.
      @note Unsafe. Only available with `MOZZI_FIXMATH_UNSAFE`
      @param op The shift number
      @return The result of the shift as a UFix.
  */
  SFix<NI,NF> operator<< (const int8_t op) const
  {
    return SFix<NI,NF>(internal_value<<op,true);
  }
  #endif

  /** Safe and optimal right shift. The returned type will be adjusted accordingly
      @param op The shift number
      @return The result of the shift as a UFix of smaller size.
  */
  template<int8_t op>
  SFix<MozziPrivate::mozziMax(NI-op,0), NF+op, MozziPrivate::rangeShift(NI,op,RANGE)> sR()
  {
    return SFix<MozziPrivate::mozziMax(NI-op,0),NF+op,MozziPrivate::rangeShift(NI,op,RANGE)>(internal_value,true);
  }

  /** Safe and optimal left shift. The returned type will be adjusted accordingly
      @param op The shift number
      @return The result of the shift as a UFix of bigger size.
  */
template<int8_t op>
  SFix<NI+op,MozziPrivate::mozziMax(NF-op,0),MozziPrivate::rangeShift(NF,op,RANGE)> sL()
  {
    return SFix<NI+op,MozziPrivate::mozziMax(NF-op,0)>(internal_value,true);
  }


  //////// COMPARISON OVERLOADS

  /** Comparison with another SFix.
      @param op A UFix
      @return true if this is bigger than op, false otherwise
  */
  template<int8_t _NI, int8_t _NF>
  bool operator> (const SFix<_NI,_NF>& op) const
  {
    constexpr int8_t new_NI = MozziPrivate::mozziMax(NI, _NI);
    constexpr int8_t new_NF = MozziPrivate::mozziMax(NF, _NF);
    SFix<new_NI,new_NF> left(*this);
    SFix<new_NI,new_NF> right(op);
    return left.asRaw()>right.asRaw();
  }

  /** Comparison with another SFix.
      @param op A UFix
      @return true if this is smaller than op, false otherwise
  */
  template<int8_t _NI, int8_t _NF>
  bool operator< (const SFix<_NI,_NF>& op) const
  {
    return op > *this;
  }


  /** Comparison with another SFix.
      @param op A UFix
      @return true if this is equal to op, false otherwise
  */
  template<int8_t _NI, int8_t _NF>
  bool operator== (const SFix<_NI,_NF>& op) const
  {
    constexpr int8_t new_NI = MozziPrivate::mozziMax(NI, _NI);
    constexpr int8_t new_NF = MozziPrivate::mozziMax(NF, _NF);
    SFix<new_NI,new_NF> left(*this);
    SFix<new_NI,new_NF> right(op);
    return left.asRaw()==right.asRaw();
  }

  /** Comparison with another SFix.
      @param op A UFix
      @return true if this is not equal to op, false otherwise
  */
  template<int8_t _NI, int8_t _NF>
  bool operator!= (const SFix<_NI,_NF>& op) const
  {
    constexpr int8_t new_NI = MozziPrivate::mozziMax(NI, _NI);
    constexpr int8_t new_NF = MozziPrivate::mozziMax(NF, _NF);
    SFix<new_NI,new_NF> left(*this);
    SFix<new_NI,new_NF> right(op);
    return left.asRaw()!=right.asRaw();
  }


  /** Returns the number as a UFix of same (positive) range and precision. The initial value has to be positive to return something correct. This is more optimized than a cast.
      @return a UFix 
  */
  UFix<NI,NF,RANGE> asUFix() const
  {
    return UFix<NI,NF,RANGE>(internal_value,true);
  }
  

  /** Returns the value as floating point number.
      @return The floating point value.
  */
  float asFloat() const {return (static_cast<float>(internal_value)) / (next_greater_type(1)<<NF); }
  
  /** Returns the internal integer value
      @return the internal value
  */
  internal_type asRaw() const {return internal_value; }

  /** The number of bits used to encode the integral part.
      @return The number of bits used to encode the integral part.
  */
  int8_t getNI() const {return NI;}

  /** The number of bits used to encode the fractional part.
      @return The number of bits used to encode the fractional part.
  */
  int8_t getNF() const {return NF;}
  

private:
  internal_type internal_value;
  static constexpr internal_type onesbitmask() { return (internal_type) ((1ULL<< (NI+NF)) - 1); }
};


#ifdef MOZZI_FIXMATH_UNSAFE
/// Reverse overloadings, 

// Multiplication
template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator*(uint8_t op, const SFix<NI, NF>& uf) {return uf*op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator*(uint16_t op, const SFix<NI, NF>& uf) {return uf*op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator*(uint32_t op, const SFix<NI, NF>& uf) {return uf*op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator*(uint64_t op, const SFix<NI, NF>& uf) {return uf*op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator*(int8_t op, const SFix<NI, NF>& uf) {return uf*op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator*(int16_t op, const SFix<NI, NF>& uf) {return uf*op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator*(int32_t op, const SFix<NI, NF>& uf) {return uf*op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator*(int64_t op, const SFix<NI, NF>& uf) {return uf*op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator*(float op, const SFix<NI, NF>& uf) {return uf*op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator*(double op, const SFix<NI, NF>& uf) {return uf*op;}

// Addition
template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator+(uint8_t op, const SFix<NI, NF>& uf) {return uf+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator+(uint16_t op, const SFix<NI, NF>& uf) {return uf+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator+(uint32_t op, const SFix<NI, NF>& uf) {return uf+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator+(uint64_t op, const SFix<NI, NF>& uf) {return uf+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator+(int8_t op, const SFix<NI, NF>& uf) {return uf+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator+(int16_t op, const SFix<NI, NF>& uf) {return uf+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator+(int32_t op, const SFix<NI, NF>& uf) {return uf+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator+(int64_t op, const SFix<NI, NF>& uf) {return uf+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator+(float op, const SFix<NI, NF>& uf) {return uf+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator+(double op, const SFix<NI, NF>& uf) {return uf+op;}

// Substraction
template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator-(uint8_t op, const SFix<NI, NF>& uf) {return (-uf)+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator-(uint16_t op, const SFix<NI, NF>& uf) {return (-uf)+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator-(uint32_t op, const SFix<NI, NF>& uf) {return (-uf)+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator-(uint64_t op, const SFix<NI, NF>& uf) {return (-uf)+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator-(int8_t op, const SFix<NI, NF>& uf) {return (-uf)+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator-(int16_t op, const SFix<NI, NF>& uf) {return (-uf)+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator-(int32_t op, const SFix<NI, NF>& uf) {return (-uf)+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator-(int64_t op, const SFix<NI, NF>& uf) {return (-uf)+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator-(float op, const SFix<NI, NF>& uf) {return (-uf)+op;}

template <int8_t NI, int8_t NF>
inline SFix<NI, NF> operator-(double op, const SFix<NI, NF>& uf) {return (-uf)+op;}

#endif






/** Addition between a SFix and a UFix. Safe.
    @param op1 A SFix
    @param op2 A UFix
    @return The result of the addition of op1 and op2. As a SFix
*/
template<int8_t NI, int8_t NF, uint64_t RANGE, int8_t _NI, int8_t _NF, uint64_t _RANGE>
inline SFix<MozziPrivate::neededSNIExtra(MozziPrivate::mozziMax(NI,_NI),MozziPrivate::mozziMax(NF,_NF),MozziPrivate::rangeAdd(NF,_NF,RANGE,_RANGE)),MozziPrivate::mozziMax(NF, _NF), MozziPrivate::rangeAdd(NF,_NF,RANGE,_RANGE)> operator+ (const SFix<NI,NF,RANGE>& op1, const UFix<_NI,_NF,_RANGE>& op2 )
{
  return op2+op1;
  }


/** Subtraction between a UFix and a SFix. Safe.
    @param op1 A UFix
    @param op2 A SFix
    @return The result of the subtraction of op1 by op2. As a SFix
*/
template<int8_t NI, int8_t NF, uint64_t RANGE, int8_t _NI, int8_t _NF, uint64_t _RANGE>
inline SFix<MozziPrivate::neededSNIExtra(MozziPrivate::mozziMax(NI,_NI),MozziPrivate::mozziMax(NF,_NF),MozziPrivate::rangeAdd(NF,_NF,RANGE,_RANGE)),MozziPrivate::mozziMax(NF, _NF), MozziPrivate::rangeAdd(NF,_NF,RANGE,_RANGE)> operator- (const UFix<NI,NF, RANGE>& op1, const SFix<_NI,_NF, _RANGE>& op2)
{
  return -op2+op1;
  }


/** Multiplication between a SFix and a UFix. Safe.
    @param op1 A SFix
    @param op2 A UFix
    @return The result of the multiplication of op1 and op2. As a SFix
*/
template<int8_t NI, int8_t NF, uint64_t RANGE, int8_t _NI, int8_t _NF, uint64_t _RANGE>
inline SFix<MozziPrivate::neededSNIExtra(NI+_NI, NF+_NF, RANGE*_RANGE),NF+_NF, RANGE*_RANGE> operator* (const SFix<_NI,_NF,_RANGE>& op1, const UFix<NI,NF,RANGE>& op2)
{
  return op2*op1;
}

// Comparison between SFix and UFixmath

/** Comparison between a SFix and an UFix.
    @param op1 a SFix
    @param op2 A UFix
    @return true if op1 is bigger than op2, false otherwise
*/
template<int8_t NI, int8_t NF, int8_t _NI, int8_t _NF>
inline bool operator> (const SFix<NI,NF>& op1, const UFix<_NI,_NF>& op2 )
{
  constexpr int8_t new_NI = MozziPrivate::mozziMax(NI, _NI);
  constexpr int8_t new_NF = MozziPrivate::mozziMax(NF, _NF);    
  SFix<new_NI,new_NF> left(op1);
  SFix<new_NI,new_NF> right(op2);
  return left.asRaw() > right.asRaw();
}

/** Comparison between a UFix and an SFix.
    @param op1 a UFix
    @param op2 A SFix
    @return true if op1 is bigger than op2, false otherwise
*/
template<int8_t NI, int8_t NF, int8_t _NI, int8_t _NF>
inline bool operator> (const UFix<NI,NF>& op1, const SFix<_NI,_NF>& op2 )
{
  constexpr int8_t new_NI = MozziPrivate::mozziMax(NI, _NI);
  constexpr int8_t new_NF = MozziPrivate::mozziMax(NF, _NF);    
  SFix<new_NI,new_NF> left(op1);
  SFix<new_NI,new_NF> right(op2);
  return left.asRaw() > right.asRaw();
}

/** Comparison between a UFix and an SFix.
    @param op1 a UFix
    @param op2 A SFix
    @return true if op1 is smaller than op2, false otherwise
*/
template<int8_t NI, int8_t NF, int8_t _NI, int8_t _NF>
inline bool operator< (const UFix<NI,NF>& op1, const SFix<_NI,_NF>& op2 )
{
  return op2 > op1;
}


/** Comparison between a SFix and an UFix.
    @param op1 a SFix
    @param op2 A UFix
    @return true if op1 is smaller than op2, false otherwise
*/
template<int8_t NI, int8_t NF, int8_t _NI, int8_t _NF>
inline bool operator< (const SFix<NI,NF>& op1, const UFix<_NI,_NF>& op2 )
{
  return op2 > op1;
}


/** Comparison between a SFix and an UFix.
    @param op1 a SFix
    @param op2 A UFix
    @return true if op1 is equal to op2, false otherwise
*/
template<int8_t NI, int8_t NF, int8_t _NI, int8_t _NF>
inline bool operator== (const SFix<NI,NF>& op1, const UFix<_NI,_NF>& op2 )
{
  constexpr int8_t new_NI = MozziPrivate::mozziMax(NI, _NI);
  constexpr int8_t new_NF = MozziPrivate::mozziMax(NF, _NF);    
  SFix<new_NI,new_NF> left(op1);
  SFix<new_NI,new_NF> right(op2);
  return left.asRaw() == right.asRaw();
}


/** Comparison between a UFix and an SFix.
    @param op1 a UFix
    @param op2 A SFix
    @return true if op1 is equal to op2, false otherwise
*/
template<int8_t NI, int8_t NF, int8_t _NI, int8_t _NF>
inline bool operator== (const UFix<NI,NF>& op1, const SFix<_NI,_NF>& op2 )
{
  return op2 == op1;
}

/** Comparison between a SFix and an UFix.
    @param op1 a SFix
    @param op2 A UFix
    @return true if op1 is not equal to op2, false otherwise
*/
template<int8_t NI, int8_t NF, int8_t _NI, int8_t _NF>
inline bool operator!= (const SFix<NI,NF>& op1, const UFix<_NI,_NF>& op2 )
{
  constexpr int8_t new_NI = MozziPrivate::mozziMax(NI, _NI);
  constexpr int8_t new_NF = MozziPrivate::mozziMax(NF, _NF);    
  SFix<new_NI,new_NF> left(op1);
  SFix<new_NI,new_NF> right(op2);
  return left.asRaw() != right.asRaw();
}


/** Comparison between a UFix and an SFix.
    @param op1 a UFix
    @param op2 A SFix
    @return true if op1 is not equal to op2, false otherwise
*/
template<int8_t NI, int8_t NF, int8_t _NI, int8_t _NF>
inline bool operator!= (const UFix<NI,NF>& op1, const SFix<_NI,_NF>& op2 )
{
  return op2 != op1;
}

////// Helper functions to build SFix from a normal type automatically


/** Create a *pure* fractional signed fixed number (SFix) from a integer.
    The number of fractional bits (NF) is chosen automatically depending on the input 
    type. Hence toSFraction(127) and toSFraction(int8_t(127)) *do not* lead to the 
    same thing: on an AVR, the former will lead to NF=15 - which is overkill and 
    incorrect if you expect toSFraction(127) = 1 -
    whereas the latter will lead to NF=7. Mozzi's objects (Oscil and the like)
    returns correct types, hence you can use this function to convert the return
    value of a Mozzi's function/class member into a pure fractional number.
    @param val The value to be converted into a pure fractional number.
    @return A SFix<0,NF> with NF chosen according to the input type
*/
template<typename T>
inline SFix<0, sizeof(T)*8-1> toSFraction(T val) {
  return SFix<0, sizeof(T)*8-1>::fromRaw(val); 
}

/** Create a *pure* integer signed fixed number (SFix) from a integer.
    The number of fractional bits (NI) is chosen automatically depending on the input 
    type. Hence toSInt(127) and toSInt(int8_t(127)) *do not* lead to the 
    same thing: on an AVR, the former will lead to NI=15 - which is overkill -
    whereas the latter will lead to NI=7. Mozzi's objects (Oscil and the like)
    returns correct types, hence you can use this function to convert the return
    value of a Mozzi's function/class member into a pure fractional number.
    @param val The value to be converted into a pure integer fixed math number.
    @return A SFix<NI,0> with NI chosen according to the input type
*/
template<typename T>
inline SFix<sizeof(T)*8-1,0> toSInt(T val) {
  return SFix<sizeof(T)*8-1,0>::fromRaw(val); 
}






#endif
