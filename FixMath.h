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
    signed (SFixMath) or unsigned (UFixMath).

    A fixed point number has its range defined by the number of bits encoding the integer part (NI 
    in the following) and its precision by the number of bits encoding the fractional part (NF). For UFixMath types, the integral part can hold values in [0,2^NI-1], for SFixMath types, the integral part can hold values in [-2^NI,2^NI-1].

    Like standard C(++) types, the fixed point numbers defined here are following some rules:
    - any fixed type can be converted to another *as long as the value can be represented in the destination type*. Casting to a bigger type in term of NI and NF is safe, but reducing NI can lead to an overflow if the new type cannot hold the integer value and reducing NF leads to a loss of precision.
    - all operations between fixed point number is safe (it won't overflow) and preserve the precision. In particular:
    - only addition, subtraction and multiplication are implemented
    - any operation between a signed and an unsigned leads to a signed number
    - resulting numbers will be casted to a type big enough to store the expected values. It follows that it is worth starting with types that are as small as possible to hold the initial value.
    - all operations between a fixed point number and a native type (int, float, uint) are *not* safe. If the resulting value cannot be represented in the fixed point type it will overflow. Only addition, subtraction, multiplication and right/left shift are implemented.
    - safe right/left shifts, which return the correct value in the correct type are implemented as .sR<shift>() and .sL<shift>() respectively, shift being the shifting amount. These shifts are basically on

    More specifically on the returned types of the operations between fixed point math types:
    - Additions:
    - UFixMath<NI,NF> + UFixMath<_NI,_NF> returns UFixMath<MAX(NI,_NI)+1,MAX(NF,_NF)>
    - SFixMath<NI,NF> + SFixMath<_NI,_NF> returns SFixMath<MAX(NI,_NI)+1,MAX(NF,_NF)>
    - UFixMath<NI,NF> + SFixMath<_NI,_NF> returns SFixMath<MAX(NI,_NI)+1,MAX(NF,_NF)>
    - UFixMath<NI,NF> + anything_else returns UFixMath<NI,NF>
    - SFixMath<NI,NF> + anything_else returns SFixMath<NI,NF>
    - Subtractions:
    - UFixMath<NI,NF> - UFixMath<_NI,_NF> returns SFixMath<MAX(NI,_NI),MAX(NF,_NF)>
    - SFixMath<NI,NF> - SFixMath<_NI,_NF> returns SFixMath<MAX(NI,_NI)+1,MAX(NF,_NF)>
    - SFixMath<NI,NF> - UFixMath<_NI,_NF> returns SFixMath<MAX(NI,_NI)+1,MAX(NF,_NF)>
    - UFixMath<NI,NF> - anything_else returns UFixMath<NI,NF>
    - SFixMath<NI,NF> - anything_else returns SFixMath<NI,NF>
    - (-)SFixMath<NI,NF> return SFixMath<NI,NF>
    - (-)UFixMath<NI,NF> return SFixMath<NI,NF>
    - Multiplications:
    - UFixMath<NI,NF> * UFixMath<_NI,_NF> returns UFixMath<NI+_NI,NF+_NF>
    - UFixMath<NI,NF> * SFixMath<_NI,_NF> returns SFixMath<NI+_NI,NF+_NF>
    - SFixMath<NI,NF> * SFixMath<_NI,_NF> returns SFixMath<NI+_NI,NF+_NF>
    - UFixMath<NI,NF> * anything_else returns UFixMath<NI,NF>
    - SFixMath<NI,NF> * anything_else returns SFixMath<NI,NF>
    - Shifts:
    - UFixMath<NI,NF> .sR<NS> returns UFixMath<NI-NS,NF+NS>
    - UFixMath<NI,NF> .sL<NS> returns UFixMath<NI+NS,NF-NS>
    - same for SFixMath.
      
   
*/




#ifndef FIXMATH2_H_
#define FIXMATH2_H_

#include<Arduino.h>
#include "IntegerType.h"

#define MOZZI_SHIFTR(x,bits) (bits > 0 ? (x >> (bits)) : (x << (-bits))) // shift right for positive shift numbers, and left for negative ones.
#define MAX(N1,N2) ((N1) > (N2) ? (N1) : (N2))
//#define UBITSTOBYTES(N) (((N-1)>>3)+1)
//#define SBITSTOBYTES(N) (((N)>>3)+1)
//#define ONESBITMASK(N) ((1ULL<<(N)) - 1)
#define FULLRANGE(N) ((1ULL<<(N)) - 1)
#define NEEDEDNI(NI, _NI, RANGE, _RANGE) ((RANGE+_RANGE)>FULLRANGE(MAX(NI, _NI)) ? (MAX(NI, _NI)+1) : (MAX(NI, _NI)))

// Experiments
/*#define NBITSREAL(X,N) (abs(X) < (1<<N) ? N : NBITSREAL2(X,N+1))
  #define NBITSREAL2(X,N) (abs(X) < (1<<N) ? N : NBITSREAL(X,N+1))
  #define UFixAuto(X) (UFixMath<NBITSREAL(X,0),0>(X))
  #define UFixAuto(X) (UFixMath<NBITSREAL(X,0),0>(X))*/

/*
  template<typename T>
  constexpr byte nBitsReal(T x, byte n=0) {
  if (abs(x) < ((T)1 << n)) {
  return n;
  } else {
  return nBitsReal(x, n + 1);
  }
  }

  #define UFixAuto(X) (UFixMath<nBitsReal(X),0>(X))
*/

 

namespace MozziPrivate {
  constexpr byte sBitsToBytes(byte N) { return (((N)>>3)+1);}
  constexpr byte uBitsToBytes(byte N) { return (((N-1)>>3)+1);}
}

// Forward declaration
template<byte NI, byte NF, uint64_t RANGE=FULLRANGE(NI)>
class SFixMath;



/** Instanciate an unsigned fixed point math number.
    @param NI The number of bits encoding the integer part. The integral part can range into [0, 2^NI -1]
    @param NF The number of bits encoding the fractional part
*/
template<byte NI, byte NF, uint64_t RANGE=FULLRANGE(NI)> // NI and NF being the number of bits for the integral and the fractionnal parts respectively.
class UFixMath
{
  static_assert(NI+NF<=64, "The total width of a UFixMath cannot exceed 64bits");
  typedef typename IntegerType<MozziPrivate::uBitsToBytes(NI+NF)>::unsigned_type internal_type ; // smallest size that fits our internal integer
  typedef typename IntegerType<MozziPrivate::uBitsToBytes(NI+NF+1)>::unsigned_type next_greater_type ; // smallest size that fits 1<<NF for sure (NI could be equal to 0). It can be the same than internal_type in some cases.
  
public:
  /** Constructor
   */
  UFixMath() {;}

  /** Constructor from a positive floating point value.
      @param fl Floating point value
      @return An unsigned fixed point number
  */
  UFixMath(float fl)  {internal_value = /*static_cast<internal_type>*/(fl * (next_greater_type(1) << NF));}

  /** Constructor from a floating point value.
      @param fl Floating point value
      @return An unsigned fixed point number
  */
  UFixMath(double fl)  {internal_value = static_cast<internal_type> (fl * (next_greater_type(1) << NF)); }
  
  /* Constructor from integer type (as_frac = false) or from fractionnal value (as_frac=true) can be used to emulate the behavior of for instance Q8n0_to_Q8n8 */

  /** Constructor from an integer value which can be interpreted as both a resulting fixed point 
      math number with a fractional part equals to 0, or as a number with decimal, ie as the underlying type behind the fixed point math number.
      @param value Integer value
      @param as_raw=false with false value will be interpreted as an integer, with true it will be interpreted as a number with decimals.
      @return An unsigned fixed point number
  */
  template<typename T>
  UFixMath(T value,bool as_raw=false)
  {
    if (as_raw) internal_value = value;
    else internal_value = (internal_type(value) << NF);
  }


  /** Set the internal value of the fixed point math number.
      @param raw The new internal value.
      @return An UFixMathx
  */  
  template<typename T>
  static UFixMath<NI,NF> fromRaw(T raw){return UFixMath<NI,NF>(raw,true);}





  /** Constructor from another UFixMath. 
      @param uf An unsigned fixed type number which value can be represented in this type.
      @return A unsigned fixed type number
  */
  template<byte _NI, byte _NF, uint64_t _RANGE>
  UFixMath(const UFixMath<_NI,_NF, _RANGE>& uf) {
    //internal_value = MOZZI_SHIFTR((typename IntegerType<((MAX(NI+NF-1,_NI+_NF-1))>>3)+1>::unsigned_type) uf.asRaw(),(_NF-NF));
    internal_value = MOZZI_SHIFTR((typename IntegerType<MozziPrivate::uBitsToBytes(MAX(NI+NF,_NI+_NF))>::unsigned_type) uf.asRaw(),(_NF-NF));
  }

  /** Constructor from another SFixMath. 
      @param uf An signed fixed type number which value can be represented in this type: sign is thus discarded.
      @return A unsigned fixed type number
  */
  template<byte _NI, byte _NF>
  UFixMath(const SFixMath<_NI,_NF>& uf) {
    internal_value = MOZZI_SHIFTR((typename IntegerType<MozziPrivate::uBitsToBytes(MAX(NI+NF,_NI+_NF))>::unsigned_type) uf.asRaw(),(_NF-NF));
  }


  //////// ADDITION OVERLOADS
  
  /** Addition with another UFixMath. Safe.
      @param op The UFixMath to be added.
      @return The result of the addition as a UFixMath.
  */
   template<byte _NI, byte _NF, uint64_t _RANGE>
  UFixMath<NEEDEDNI(NI,_NI,RANGE,_RANGE), MAX(NF,_NF), RANGE+_RANGE> operator+ (const UFixMath<_NI,_NF,_RANGE>& op) const
  {
    constexpr byte new_NI = NEEDEDNI(NI,_NI,RANGE,_RANGE);
    constexpr byte new_NF = MAX(NF, _NF);
    typedef typename IntegerType<MozziPrivate::uBitsToBytes(new_NI+new_NF)>::unsigned_type return_type;
    UFixMath<new_NI,new_NF> left(*this);
    UFixMath<new_NI,new_NF> right(op);

    return_type tt = return_type(left.asRaw()) + right.asRaw();
    return UFixMath<new_NI,new_NF,RANGE+_RANGE>(tt,true);
    }

  /** Addition with another type. Unsafe
      @param op The number to be added.
      @return The result of the addition as a UFixMath.
  */
  template<typename T>
  UFixMath<NI,NF> operator+ (const T op) const
  {
    return UFixMath<NI,NF>(internal_value+((internal_type)op<<NF),true);
  }


  //////// SUBSTRACTION OVERLOADS

  /** Subtraction with another UFixMath. Safe.
      @param op The UFixMath to be subtracted.
      @return The result of the subtraction as a SFixMath.
  */
  template<byte _NI, byte _NF, uint64_t _RANGE> // We do not have the +1 after MAX(NI, _NI) because the substraction between two UFix should fit in the biggest of the two.
  SFixMath<MAX(NI,_NI),MAX(NF,_NF), MAX(RANGE, _RANGE)> operator- (const UFixMath<_NI,_NF, _RANGE>& op) const
  {
    constexpr byte new_NI = MAX(NI, _NI);
    constexpr byte new_NF = MAX(NF, _NF);
    typedef typename IntegerType<MozziPrivate::sBitsToBytes(new_NI+new_NF)>::signed_type return_type;
    SFixMath<new_NI,new_NF> left(*this);
    SFixMath<new_NI,new_NF> right(op);

    return_type tt = return_type(left.asRaw()) - right.asRaw();
    return SFixMath<new_NI,new_NF,MAX(RANGE,_RANGE)>(tt,true);
  }

  
  /** Subtraction with another type. Unsafe
      @param op The number to be subtracted.
      @return The result of the subtraction as a UFixMath.
  */
  template<typename T>
  UFixMath<NI,NF> operator- (const T op) const
  {
    return UFixMath<NI,NF>(internal_value-((internal_type)op<<NF),true);
  }


  /** Opposite of the number.
      @return The opposite numberas a SFixMath.
  */
  SFixMath<NI,NF> operator-() const
  {
    return SFixMath<NI,NF>( -(typename IntegerType<MozziPrivate::sBitsToBytes(NI+NF)>::signed_type)(internal_value),true);
  }

  //////// MULTIPLICATION OVERLOADS
  
  /** Multiplication with another UFixMath. Safe.
      @param op The UFixMath to be multiplied.
      @return The result of the multiplication as a UFixMath.
  */
  template<byte _NI, byte _NF>
  UFixMath<NI+_NI,NF+_NF> operator* (const UFixMath<_NI,_NF>& op) const
  {
    //typedef typename IntegerType< ((NI+_NI+NF+_NF-1)>>3)+1>::unsigned_type return_type ;
    typedef typename IntegerType<MozziPrivate::uBitsToBytes(NI+_NI+NF+_NF)>::unsigned_type return_type ;
    return_type tt = return_type(internal_value)*op.asRaw();
    return UFixMath<NI+_NI,NF+_NF>(tt,true);
  }

  /** Multiplication with another type. Unsafe.
      @param op The number to be multiplied.
      @return The result of the multiplication as a UFixMath.
  */
  template<typename T>
  UFixMath<NI,NF> operator* (const T op) const
  {
    return UFixMath<NI,NF>(internal_value*op,true);
  }


  //////// INVERSE

  /** Compute the inverse of a UFixMath<NI,NF>, as a UFixMath<NF,NI> (not a typo).
      This inverse is able to represent accurately the inverse of the smallest and biggest of the initial number, but might lead to a lot of duplicates in between.
      Good if precision is not a premium but type conservation and speeds are. 
      This is still slower than a multiplication, hence the suggested workflow is to compute the inverse when time is not critical, for instance in updateControl(), and multiply it afterward, for instance in updateAudio(), if you need a division.
      @return The inverse of the number.
  */
  UFixMath<NF,NI> invFast() const
  {
    static_assert(NI+NF<=63, "The fast inverse cannot be computed for when NI+NF>63. Reduce the number of bits.");
    //return UFixMath<NF,NI>((ONESBITMASK(NI+NF)/internal_value),true);
    return UFixMath<NF,NI>((onesbitmask()/internal_value),true);
  }


  /** Compute the inverse of a UFixMath<NI,NF>, as a UFixMath<NF,_NF>.
      _NF is the number of precision bits for the output. Can be any number but especially useful for case between invFast() (_NF=NI) and invAccurate() (_NF=2*NI+NF)
      @return The inverse of the number.
  */
  template<byte _NF>
  UFixMath<NF,_NF> inv() const
  {
    return UFixMath<_NF,NF>(internal_value,true).invFast();  
  }


  
  /** Compute the inverse of a UFixMath<NI,NF>, as a UFixMath<NF,NI*2+NF>.
      This inverse is more accurate than invFast, and usually leads to non common values on the whole input range. This comes at the cost of a way bigger resulting type.
      This is still slower than a multiplication, hence the suggested workflow is to compute the inverse when time is not critical, for instance in updateControl(), and multiply it afterward, for instance in updateAudio(), if you need a division.
      @return The inverse of the number.
  */
  UFixMath<NF,NI*2+NF> invAccurate() const
  {/*
     static_assert(2*NI+2*NF<=63, "The accurate inverse cannot be computed for when 2*NI+2*NF>63. Reduce the number of bits.");
     return UFixMath<NI*2+NF,NF>(internal_value,true).invFast(); */
    //return UFixMath<NI*2+NF,NF>(internal_value,true).inv<NI*2+NF>();
    return inv<NI*2+NF>();
  }


  
  


  //////// SHIFTS OVERLOADS

  /** Right shift. This won't overflow but will not leverage on the bits freed by the shift.
      Better to use .sR<shift>() if possible instead.
      @param op The shift number
      @return The result of the shift as a UFixMath.
  */
  UFixMath<NI,NF> operator>> (const byte op) const
  {
    return UFixMath<NI,NF>(internal_value>>op,true);
  }

  /** Left shift. This can overflow if you shift to a value that cannot be represented.
      Better to use .sL<shift>() if possible instead.
      @param op The shift number
      @return The result of the shift as a UFixMath.
  */
  UFixMath<NI,NF> operator<< (const byte op) const
  {
    return UFixMath<NI,NF>(internal_value<<op,true);
  }

  /** Safe and optimal right shift. The returned type will be adjusted accordingly
      @param op The shift number
      @return The result of the shift as a UFixMath of smaller size.
  */
  template<byte op>
  UFixMath<NI-op,NF+op> sR()
  {
    return UFixMath<NI-op,NF+op>(internal_value,true);
  }

  /** Safe and optimal left shift. The returned type will be adjusted accordingly
      @param op The shift number
      @return The result of the shift as a UFixMath of bigger size.
  */
  template<byte op>
  UFixMath<NI+op,NF-op> sL()
  {
    return UFixMath<NI+op,NF-op>(internal_value,true);
  }

  
  //////// COMPARISON OVERLOADS
   
  template<byte _NI, byte _NF>
  bool operator> (const UFixMath<_NI,_NF>& op) const
  {
    constexpr byte new_NI = MAX(NI, _NI);
    constexpr byte new_NF = MAX(NF, _NF);
    UFixMath<new_NI,new_NF> left(*this);
    UFixMath<new_NI,new_NF> right(op);
    return left.asRaw()>right.asRaw();
  }

  template<byte _NI, byte _NF>
  bool operator< (const UFixMath<_NI,_NF>& op) const
  {
    return op > *this;
  }

  template<byte _NI, byte _NF>
  bool operator== (const UFixMath<_NI,_NF>& op) const
  {
    constexpr byte new_NI = MAX(NI, _NI);
    constexpr byte new_NF = MAX(NF, _NF);
    UFixMath<new_NI,new_NF> left(*this);
    UFixMath<new_NI,new_NF> right(op);
    return left.asRaw()==right.asRaw();
  }

  template<byte _NI, byte _NF>
  bool operator!= (const UFixMath<_NI,_NF>& op) const
  {
    constexpr byte new_NI = MAX(NI, _NI);
    constexpr byte new_NF = MAX(NF, _NF);
    UFixMath<new_NI,new_NF> left(*this);
    UFixMath<new_NI,new_NF> right(op);
    return left.asRaw()!=right.asRaw();
  }


  // Division. Might actually more misleading than helping. NON Working version below.
  /*
    template<byte NI1, byte NF1, byte NI2, byte NF2>
    UFixMath<NI1-NI2, NF1-NF2> operator/(const UFixMath<NI1, NF1>& op1, const UFixMath<NI2, NF2>& op2)
    {
    typedef typename IntegerType< ((NI1-NI2+NF1-NF2+1)>>3)+1>::unsigned_type return_type ;
    return_type tt = (return_type(op1.getInt())<<(NF1-NF2))/op2.getInt();
    return UFixMath<NI1-NI2, NF1-NF2>(tt,true);
    }
  */


  /** Returns the value as floating point number.
      @return The floating point value.
  */
  float asFloat() { return (static_cast<float>(internal_value)) / (next_greater_type(1)<<NF); }
  
  /** Returns the internal integer value
      @return the internal value
  */
  internal_type asRaw() const { return internal_value; }
  
  /** The number of bits used to encode the integral part.
      @return The number of bits used to encode the integral part.
  */
  byte getNI(){return NI;}

  /** The number of bits used to encode the fractional part.
      @return The number of bits used to encode the fractional part.
  */
  byte getNF(){return NF;}
  
private:
  internal_type internal_value;
  static constexpr internal_type onesbitmask() { return (internal_type) ((1ULL<< (NI+NF)) - 1); }
  //static constexpr typename IntegerType<MozziPrivate::uBitsToBytes(2*NI+2*NF)>::unsigned_type onesbitmaskfull() { return (typename IntegerType<MozziPrivate::uBitsToBytes(2*NI+2*NF)>::unsigned_type) ((1ULL<< (NI*2+NF*2)) - 1); }
  
};



/// Reverse overloadings, making a template here leads to an ambiguity, forcing us to specify them one by one??

// Multiplication
template <byte NI, byte NF>
inline UFixMath<NI, NF> operator*(uint8_t op, const UFixMath<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
inline UFixMath<NI, NF> operator*(uint16_t op, const UFixMath<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
inline UFixMath<NI, NF> operator*(uint32_t op, const UFixMath<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
inline UFixMath<NI, NF> operator*(uint64_t op, const UFixMath<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
inline UFixMath<NI, NF> operator*(int8_t op, const UFixMath<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
inline UFixMath<NI, NF> operator*(int16_t op, const UFixMath<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
inline UFixMath<NI, NF> operator*(int32_t op, const UFixMath<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
inline UFixMath<NI, NF> operator*(int64_t op, const UFixMath<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
inline UFixMath<NI, NF> operator*(float op, const UFixMath<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
inline UFixMath<NI, NF> operator*(double op, const UFixMath<NI, NF>& uf) {return uf*op;}

// Addition
template <byte NI, byte NF>
inline UFixMath<NI, NF> operator+(uint8_t op, const UFixMath<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
inline UFixMath<NI, NF> operator+(uint16_t op, const UFixMath<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
inline UFixMath<NI, NF> operator+(uint32_t op, const UFixMath<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
inline UFixMath<NI, NF> operator+(uint64_t op, const UFixMath<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
inline UFixMath<NI, NF> operator+(int8_t op, const UFixMath<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
inline UFixMath<NI, NF> operator+(int16_t op, const UFixMath<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
inline UFixMath<NI, NF> operator+(int32_t op, const UFixMath<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
inline UFixMath<NI, NF> operator+(int64_t op, const UFixMath<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
inline UFixMath<NI, NF> operator+(float op, const UFixMath<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
inline UFixMath<NI, NF> operator+(double op, const UFixMath<NI, NF>& uf) {return uf+op;}

// Substraction
template <byte NI, byte NF>
inline SFixMath<NI, NF> operator-(uint8_t op, const UFixMath<NI, NF>& uf) {return -uf+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator-(uint16_t op, const UFixMath<NI, NF>& uf) {return -uf+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator-(uint32_t op, const UFixMath<NI, NF>& uf) {return -uf+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator-(uint64_t op, const UFixMath<NI, NF>& uf) {return -uf+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator-(int8_t op, const UFixMath<NI, NF>& uf) {return -uf+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator-(int16_t op, const UFixMath<NI, NF>& uf) {return -uf+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator-(int32_t op, const UFixMath<NI, NF>& uf) {return -uf+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator-(int64_t op, const UFixMath<NI, NF>& uf) {return -uf+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator-(float op, const UFixMath<NI, NF>& uf) {return -uf+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator-(double op, const UFixMath<NI, NF>& uf) {return -uf+op;}

////// Helper functions to build SFix from a normal type automatically


/** Create a *pure* fractional unsigned fixed number (UFixMath) from an unsigned integer.
    The number of fractional bits (NF) is chosen automatically depending on the input 
    type. Hence toUFraction(255) and toUFraction(uint8_t(255)) *do not* lead to the 
    same thing: on an AVR, the former will lead to NF=16 - which is overkill and 
    incorrect if you expect toUFraction(255) = 1 -
    whereas the latter will lead to NF=8. Mozzi's objects (Oscil and the like)
    returns correct types, hence you can use this function to convert the return
    value of a Mozzi's function/class member into a pure fractional number.
    @param val The value to be converted into a pure fractional number.
    @return A UFixMath<0,NF> with NF chosen according to the input type
*/
template<typename T>
inline UFixMath<0, sizeof(T)*8> toUFraction(T val) {
  return UFixMath<0, sizeof(T)*8>::fromRaw(val); 
}

/** Create a *pure* integer unsigned fixed number (UFixMath) from an unsigned integer.
    The number of fractional bits (NI) is chosen automatically depending on the input 
    type. Hence toUInt(255) and toSInt(uint8_t(255)) *do not* lead to the 
    same thing: on an AVR, the former will lead to NI=16 - which is overkill -
    whereas the latter will lead to NI=8. Mozzi's objects (Oscil and the like)
    returns correct types, hence you can use this function to convert the return
    value of a Mozzi's function/class member into a pure fractional number.
    @param val The value to be converted into a pure unsigned integer fixed math number.
    @return A UFixMath<NI,0> with NI chosen according to the input type
*/
template<typename T>
inline UFixMath<sizeof(T)*8-1,0> toUInt(T val) {
  return UFixMath<sizeof(T)*8-1,0>::fromRaw(val); 
}



/** Instanciate an signed fixed point math number.
    @param NI The number of bits encoding the integer part. The integral part can range into [-2^NI, 2^NI -1]
    @param NF The number of bits encoding the fractional part
    @note The total number of the underlying int will be NI+NF+1 in order to accomodate the sign. It follows that, if you want something that reproduces the behavior of a int8_t, it should be declared as SFixMath<7,0>.
*/
template<byte NI, byte NF, uint64_t RANGE> // NI and NF being the number of bits for the integral and the fractionnal parts respectively.
class SFixMath
{
  static_assert(NI+NF<64, "The total width of a SFixMath cannot exceed 63bits");
  typedef typename IntegerType<MozziPrivate::sBitsToBytes(NI+NF)>::signed_type internal_type ; // smallest size that fits our internal integer
  typedef typename IntegerType<MozziPrivate::sBitsToBytes(NI+NF+1)>::signed_type next_greater_type ; // smallest size that fits 1<<NF for sure (NI could be equal to 0). It can be the same than internal_type in some cases.
  
public:
  /** Constructor
   */
  SFixMath() {;}
  
  /** Constructor from a floating point value.
      @param fl Floating point value
      @return An signed fixed point number
  */
  SFixMath(float fl)  {internal_value = /*static_cast<internal_type>*/(fl * (next_greater_type(1) << NF));}

  /** Constructor from a floating point value.
      @param fl Floating point value
      @return An signed fixed point number
  */
  SFixMath(double fl)  {internal_value = static_cast<internal_type> (fl * (next_greater_type(1) << NF)); }


  /** Constructor from an integer value which can be interpreted as both a resulting fixed point 
      math number with a fractional part equals to 0, or as a number with decimal, ie as the underlying type behind the fixed point math number.
      @param value Integer value
      @param as_raw=false with false value will be interpreted as an integer, with true it will be interpreted as a number with decimals.
      @return An signed fixed point number
  */
  template<typename T>
  SFixMath(T value,bool as_raw=false)
  {
    if (as_raw) internal_value = value;
    else internal_value = (internal_type(value) << NF);
  }

  /** Set the internal value of the fixed point math number.
      @param raw The new internal value.
      @return A SFixMath.
  */  
  template<typename T>
  static SFixMath<NI,NF> fromRaw(T raw){return SFixMath<NI,NF>(raw,true);}


  /** Constructor from another SFixMath. 
      @param uf A signed fixed type number which value can be represented in this type.
      @return A signed fixed type number
  */
  template<byte _NI, byte _NF>
  SFixMath(const SFixMath<_NI,_NF>& uf) {
    //internal_value = MOZZI_SHIFTR((typename IntegerType<((MAX(NI+NF,_NI+_NF))>>3)+1>::unsigned_type) uf.asRaw(),(_NF-NF));
    internal_value = MOZZI_SHIFTR((typename IntegerType<MozziPrivate::sBitsToBytes(MAX(NI+NF,_NI+_NF))>::unsigned_type) uf.asRaw(),(_NF-NF));
    
  }

  /** Constructor from another UFixMath. 
      @param uf A unsigned fixed type number which value can be represented in this type.
      @return A signed fixed type number
  */
  template<byte _NI, byte _NF>
  SFixMath(const UFixMath<_NI,_NF>& uf) {
    internal_value = MOZZI_SHIFTR((typename IntegerType<MozziPrivate::uBitsToBytes(MAX(NI+NF,_NI+_NF))>::unsigned_type) uf.asRaw(),(_NF-NF));
  }

  //////// ADDITION OVERLOADS

  /** Addition with another SFixMath. Safe.
      @param op The SFixMath to be added.
      @return The result of the addition as a SFixMath.
  */
  template<byte _NI, byte _NF>
  SFixMath<MAX(NI,_NI)+1,MAX(NF,_NF)> operator+ (const SFixMath<_NI,_NF>& op) const
  {
    constexpr byte new_NI = MAX(NI, _NI) + 1;
    constexpr byte new_NF = MAX(NF, _NF);
    typedef typename IntegerType<MozziPrivate::sBitsToBytes(new_NI+new_NF)>::unsigned_type return_type;
    SFixMath<new_NI,new_NF> left(*this);
    SFixMath<new_NI,new_NF> right(op);

    return_type tt = return_type(left.asRaw()) + right.asRaw();
    return SFixMath<new_NI,new_NF>(tt,true);
  }

  /** Addition with another type. Unsafe
      @param op The number to be added.
      @return The result of the addition as a UFixMath.
  */
  template<typename T>
  SFixMath<NI,NF> operator+ (const T op) const
  {
    return SFixMath<NI,NF>(internal_value+(op<<NF),true);
  }


  //////// SUBSTRACTION OVERLOADS

  /** Subtraction with another SFixMath. Safe.
      @param op The SFixMath to be subtracted.
      @return The result of the subtraction as a SFixMath.
  */ 
  template<byte _NI, byte _NF>
  SFixMath<MAX(NI,_NI)+1,MAX(NF,_NF)> operator- (const SFixMath<_NI,_NF>& op) const
  {
    constexpr byte new_NI = MAX(NI, _NI) + 1;
    constexpr byte new_NF = MAX(NF, _NF);
    typedef typename IntegerType<MozziPrivate::sBitsToBytes(new_NI+new_NF)>::unsigned_type return_type;
    SFixMath<new_NI,new_NF> left(*this);
    SFixMath<new_NI,new_NF> right(op);

    return_type tt = return_type(left.asRaw()) - right.asRaw();
    return SFixMath<new_NI,new_NF>(tt,true);
  }

  
  /** Subtraction with another type. Unsafe
      @param op The number to be subtracted.
      @return The result of the subtraction as a SFixMath.
  */
  template<typename T>
  SFixMath<NI,NF> operator- (const T op) const
  {
    return SFixMath<NI,NF>(internal_value-(op<<NF),true);
  }


  /** Opposite of the number.
      @return The opposite numberas a SFixMath.
  */
  SFixMath<NI,NF> operator-() const
  {
    return SFixMath<NI,NF>(-internal_value,true);
  }
  
  //////// MULTIPLICATION OVERLOADS
  
  /** Multiplication with another SFixMath. Safe.
      @param op The SFixMath to be multiplied.
      @return The result of the multiplication as a SFixMath.
  */
  template<byte _NI, byte _NF>
  SFixMath<NI+_NI,NF+_NF> operator* (const SFixMath<_NI,_NF>& op) const
  {
    typedef typename IntegerType<MozziPrivate::sBitsToBytes(NI+_NI+NF+_NF)>::signed_type return_type ;
    return_type tt = return_type(internal_value)*op.asRaw();
    return SFixMath<NI+_NI,NF+_NF>(tt,true);
  }

  /** Multiplication with another type. Unsafe.
      @param op The number to be multiplied.
      @return The result of the multiplication as a UFixMath.
  */
  template<typename T>
  SFixMath<NI,NF> operator* (const T op) const
  {
    return SFixMath<NI,NF>(internal_value*op,true);
  }


  //////// INVERSE

  /** Compute the inverse of a SFixMath<NI,NF>, as a SFixMath<NF,NI> (not a typo).
      This inverse is able to represent accurately the inverse of the smallest and biggest of the initial number, but might lead to a lot of duplicates in between.
      Good if precision is not a premium but type conservation and speeds are. 
      This is still slower than a multiplication, hence the suggested workflow is to compute the inverse when time is not critical, for instance in updateControl(), and multiply it afterward, for instance in updateAudio(), if you need a division.
      @return The inverse of the number.
  */

  SFixMath<NF,NI> invFast() const
  {
    static_assert(NI+NF<=62, "The fast inverse cannot be computed for when NI+NF>63. Reduce the number of bits.");
    //return SFixMath<NF,NI>((ONESBITMASK(NI+NF)/internal_value),true);
    return SFixMath<NF,NI>((onesbitmask()/internal_value),true);
  }

  /** Compute the inverse of a SFixMath<NI,NF>, as a SFixMath<NF,_NF>.
      _NF is the number of precision bits for the output. Can be any number but especially useful for case between invFast() (_NF=NI) and invAccurate() (_NF=2*NI+NF)
      @return The inverse of the number.
  */
  template<byte _NF>
  SFixMath<NF,_NF> inv() const
  {
    return SFixMath<_NF,NF>(internal_value,true).invFast();  
  }

  /** Compute the inverse of a SFixMath<NI,NF>, as a SFixMath<NF,NI*2+NF>.
      This inverse is more accurate than invFast, and usually leads to non common values on the whole input range. This comes at the cost of a way bigger resulting type.
      This is still slower than a multiplication, hence the suggested workflow is to compute the inverse when time is not critical, for instance in updateControl(), and multiply it afterward, for instance in updateAudio(), if you need a division.
      @return The inverse of the number.
  */
  SFixMath<NF,NI*2+NF> invAccurate() const
  {
    return inv<NI*2+NF>();
  }


  //////// SHIFTS OVERLOADS

  /** Right shift. This won't overflow but will not leverage on the bits freed by the shift.
      Better to use .sR<shift>() if possible instead.
      @param op The shift number
      @return The result of the shift as a SFixMath.
  */
  SFixMath<NI,NF> operator>> (const byte op) const
  {
    return SFixMath<NI,NF>(internal_value>>op,true);
  }

  /** Left shift. This can overflow if you shift to a value that cannot be represented.
      Better to use .sL<shift>() if possible instead.
      @param op The shift number
      @return The result of the shift as a UFixMath.
  */
  SFixMath<NI,NF> operator<< (const byte op) const
  {
    return SFixMath<NI,NF>(internal_value<<op,true);
  }

  /** Safe and optimal right shift. The returned type will be adjusted accordingly
      @param op The shift number
      @return The result of the shift as a UFixMath of smaller size.
  */
  template<byte op>
  SFixMath<NI-op,NF+op> sR()
  {
    return SFixMath<NI-op,NF+op>(internal_value,true);
  }

  /** Safe and optimal left shift. The returned type will be adjusted accordingly
      @param op The shift number
      @return The result of the shift as a UFixMath of bigger size.
  */
  template<byte op>
  SFixMath<NI+op,NF-op> sL()
  {
    return SFixMath<NI+op,NF-op>(internal_value,true);
  }


  //////// COMPARISON OVERLOADS
   
  template<byte _NI, byte _NF>
  bool operator> (const SFixMath<_NI,_NF>& op) const
  {
    constexpr byte new_NI = MAX(NI, _NI);
    constexpr byte new_NF = MAX(NF, _NF);
    SFixMath<new_NI,new_NF> left(*this);
    SFixMath<new_NI,new_NF> right(op);
    return left.asRaw()>right.asRaw();
  }

  template<byte _NI, byte _NF>
  bool operator< (const SFixMath<_NI,_NF>& op) const
  {
    return op > *this;
  }

  template<byte _NI, byte _NF>
  bool operator== (const SFixMath<_NI,_NF>& op) const
  {
    constexpr byte new_NI = MAX(NI, _NI);
    constexpr byte new_NF = MAX(NF, _NF);
    SFixMath<new_NI,new_NF> left(*this);
    SFixMath<new_NI,new_NF> right(op);
    return left.asRaw()==right.asRaw();
  }

  template<byte _NI, byte _NF>
  bool operator!= (const SFixMath<_NI,_NF>& op) const
  {
    constexpr byte new_NI = MAX(NI, _NI);
    constexpr byte new_NF = MAX(NF, _NF);
    SFixMath<new_NI,new_NF> left(*this);
    SFixMath<new_NI,new_NF> right(op);
    return left.asRaw()!=right.asRaw();
  }
  

  /** Returns the value as floating point number.
      @return The floating point value.
  */
  float asFloat() { return (static_cast<float>(internal_value)) / (next_greater_type(1)<<NF); }
  
  /** Returns the internal integer value
      @return the internal value
  */
  internal_type asRaw() const { return internal_value; }

  /** The number of bits used to encode the integral part.
      @return The number of bits used to encode the integral part.
  */
  byte getNI(){return NI;}

  /** The number of bits used to encode the fractional part.
      @return The number of bits used to encode the fractional part.
  */
  byte getNF(){return NF;}
  

private:
  internal_type internal_value;
  static constexpr internal_type onesbitmask() { return (internal_type) ((1ULL<< (NI+NF)) - 1); }
  //static constexpr typename IntegerType<MozziPrivate::sBitsToBytes(2*NI+2*NF)>::signed_type onesbitmaskfull() { return (typename IntegerType<MozziPrivate::sBitsToBytes(2*NI+2*NF)>::signed_type) ((1ULL<< (NI*2+NF*2)) - 1); }

};

/// Reverse overloadings, 

// Multiplication
template <byte NI, byte NF>
inline SFixMath<NI, NF> operator*(uint8_t op, const SFixMath<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator*(uint16_t op, const SFixMath<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator*(uint32_t op, const SFixMath<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator*(uint64_t op, const SFixMath<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator*(int8_t op, const SFixMath<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator*(int16_t op, const SFixMath<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator*(int32_t op, const SFixMath<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator*(int64_t op, const SFixMath<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator*(float op, const SFixMath<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator*(double op, const SFixMath<NI, NF>& uf) {return uf*op;}

// Addition
template <byte NI, byte NF>
inline SFixMath<NI, NF> operator+(uint8_t op, const SFixMath<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator+(uint16_t op, const SFixMath<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator+(uint32_t op, const SFixMath<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator+(uint64_t op, const SFixMath<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator+(int8_t op, const SFixMath<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator+(int16_t op, const SFixMath<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator+(int32_t op, const SFixMath<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator+(int64_t op, const SFixMath<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator+(float op, const SFixMath<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator+(double op, const SFixMath<NI, NF>& uf) {return uf+op;}

// Substraction
template <byte NI, byte NF>
inline SFixMath<NI, NF> operator-(uint8_t op, const SFixMath<NI, NF>& uf) {return (-uf)+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator-(uint16_t op, const SFixMath<NI, NF>& uf) {return (-uf)+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator-(uint32_t op, const SFixMath<NI, NF>& uf) {return (-uf)+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator-(uint64_t op, const SFixMath<NI, NF>& uf) {return (-uf)+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator-(int8_t op, const SFixMath<NI, NF>& uf) {return (-uf)+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator-(int16_t op, const SFixMath<NI, NF>& uf) {return (-uf)+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator-(int32_t op, const SFixMath<NI, NF>& uf) {return (-uf)+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator-(int64_t op, const SFixMath<NI, NF>& uf) {return (-uf)+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator-(float op, const SFixMath<NI, NF>& uf) {return (-uf)+op;}

template <byte NI, byte NF>
inline SFixMath<NI, NF> operator-(double op, const SFixMath<NI, NF>& uf) {return (-uf)+op;}









/** Multiplication between a SFixMath and a UFixMath. Safe.
    @param op1 A SFixMath
    @param op2 A UFixMath
    @return The result of the multiplication of op1 and op2. As a SFixMath
*/
template<byte NI, byte NF, byte _NI, byte _NF>
inline SFixMath<NI+_NI,NF+_NF> operator* (const SFixMath<NI,NF>& op1, const UFixMath<_NI,_NF>& op2 )
{
  typedef typename IntegerType< MozziPrivate::sBitsToBytes(NI+_NI+NF+_NF)>::signed_type return_type ;
  return_type tt = return_type(op1.asRaw())*op2.asRaw();
  return SFixMath<NI+_NI,NF+_NF>(tt,true);
}

/** Multiplication between a UFixMath and a SFixMath. Safe.
    @param op1 A UFixMath
    @param op2 A SFixMath
    @return The result of the multiplication of op1 and op2. As a SFixMath
*/
template<byte NI, byte NF, byte _NI, byte _NF>
inline SFixMath<NI+_NI,NF+_NF> operator* (const UFixMath<NI,NF>& op1, const SFixMath<_NI,_NF>& op2 )
{
  return op2*op1;
}


// Addition between SFixMath and UFixMath (promotion to next SFixMath)


/** Addition between a SFixMath and a UFixMath. Safe.
    @param op1 A SFixMath
    @param op2 A UFixMath
    @return The result of the addition of op1 and op2. As a SFixMath
*/
template<byte NI, byte NF, byte _NI, byte _NF>
inline SFixMath<MAX(NI,_NI)+1,MAX(NF,_NF)> operator+ (const SFixMath<NI,NF>& op1, const UFixMath<_NI,_NF>& op2 )
{
  constexpr byte new_NI = MAX(NI, _NI) + 1;
  constexpr byte new_NF = MAX(NF, _NF);
    
  typedef typename IntegerType<MozziPrivate::sBitsToBytes(new_NI+new_NF)>::signed_type return_type;
  SFixMath<new_NI,new_NF> left(op1);
  SFixMath<new_NI,new_NF> right(op2);
  return_type tt = return_type(left.asRaw()) + right.asRaw();
  return SFixMath<new_NI,new_NF>(tt,true);
}

/** Addition between a UFixMath and a SFixMath. Safe.
    @param op1 A UFixMath
    @param op2 A SFixMath
    @return The result of the addition of op1 and op2. As a SFixMath
*/
template<byte NI, byte NF, byte _NI, byte _NF>
inline SFixMath<MAX(NI,_NI)+1,MAX(NF,_NF)> operator+ (const UFixMath<NI,NF>& op1, const SFixMath<_NI,_NF>& op2 )
{
  return op2+op1;
}

// Substraction between SFixMath and UFixMath (promotion to next SFixMath)

/** Subtraction between a SFixMath and a UFixMath. Safe.
    @param op1 A SFixMath
    @param op2 A UFixMath
    @return The result of the subtraction of op1 by op2. As a SFixMath
*/
template<byte NI, byte NF, byte _NI, byte _NF>
inline SFixMath<MAX(NI,_NI)+1,MAX(NF,_NF)> operator- (const SFixMath<NI,NF>& op1, const UFixMath<_NI,_NF>& op2 )
{
  constexpr byte new_NI = MAX(NI, _NI) + 1;
  constexpr byte new_NF = MAX(NF, _NF);
    
  typedef typename IntegerType<MozziPrivate::sBitsToBytes(new_NI+new_NF)>::signed_type return_type;
  SFixMath<new_NI,new_NF> left(op1);
  SFixMath<new_NI,new_NF> right(op2);
  return_type tt = return_type(left.asRaw()) - right.asRaw();
  return SFixMath<new_NI,new_NF>(tt,true);
}

/** Subtraction between a UFixMath and a SFixMath. Safe.
    @param op1 A UFixMath
    @param op2 A SFixMath
    @return The result of the subtraction of op1 by op2. As a SFixMath
*/
template<byte NI, byte NF, byte _NI, byte _NF>
inline SFixMath<MAX(NI,_NI)+1,MAX(NF,_NF)> operator- (const UFixMath<NI,NF>& op1, const SFixMath<_NI,_NF>& op2 )
{
  return -op2+op1;
}

// Comparaison between SFixMath and UFixmath

template<byte NI, byte NF, byte _NI, byte _NF>
inline bool operator> (const SFixMath<NI,NF>& op1, const UFixMath<_NI,_NF>& op2 )
{
  constexpr byte new_NI = MAX(NI, _NI);
  constexpr byte new_NF = MAX(NF, _NF);    
  SFixMath<new_NI,new_NF> left(op1);
  SFixMath<new_NI,new_NF> right(op2);
  return left.asRaw() > right.asRaw();
}

template<byte NI, byte NF, byte _NI, byte _NF>
inline bool operator> (const UFixMath<NI,NF>& op1, const SFixMath<_NI,_NF>& op2 )
{
  constexpr byte new_NI = MAX(NI, _NI);
  constexpr byte new_NF = MAX(NF, _NF);    
  SFixMath<new_NI,new_NF> left(op1);
  SFixMath<new_NI,new_NF> right(op2);
  return left.asRaw() > right.asRaw();
}

template<byte NI, byte NF, byte _NI, byte _NF>
inline bool operator< (const UFixMath<NI,NF>& op1, const SFixMath<_NI,_NF>& op2 )
{
  return op2 > op1;
}

template<byte NI, byte NF, byte _NI, byte _NF>
inline bool operator< (const SFixMath<NI,NF>& op1, const UFixMath<_NI,_NF>& op2 )
{
  return op2 > op1;
}



template<byte NI, byte NF, byte _NI, byte _NF>
inline bool operator== (const SFixMath<NI,NF>& op1, const UFixMath<_NI,_NF>& op2 )
{
  constexpr byte new_NI = MAX(NI, _NI);
  constexpr byte new_NF = MAX(NF, _NF);    
  SFixMath<new_NI,new_NF> left(op1);
  SFixMath<new_NI,new_NF> right(op2);
  return left.asRaw() == right.asRaw();
}

template<byte NI, byte NF, byte _NI, byte _NF>
inline bool operator== (const UFixMath<NI,NF>& op1, const SFixMath<_NI,_NF>& op2 )
{
  return op2 == op1;
}

template<byte NI, byte NF, byte _NI, byte _NF>
inline bool operator!= (const SFixMath<NI,NF>& op1, const UFixMath<_NI,_NF>& op2 )
{
  constexpr byte new_NI = MAX(NI, _NI);
  constexpr byte new_NF = MAX(NF, _NF);    
  SFixMath<new_NI,new_NF> left(op1);
  SFixMath<new_NI,new_NF> right(op2);
  return left.asRaw() != right.asRaw();
}

template<byte NI, byte NF, byte _NI, byte _NF>
inline bool operator!= (const UFixMath<NI,NF>& op1, const SFixMath<_NI,_NF>& op2 )
{
  return op2 != op1;
}

////// Helper functions to build SFix from a normal type automatically


/** Create a *pure* fractional signed fixed number (SFixMath) from a integer.
    The number of fractional bits (NF) is chosen automatically depending on the input 
    type. Hence toSFraction(127) and toSFraction(int8_t(127)) *do not* lead to the 
    same thing: on an AVR, the former will lead to NF=15 - which is overkill and 
    incorrect if you expect toSFraction(127) = 1 -
    whereas the latter will lead to NF=7. Mozzi's objects (Oscil and the like)
    returns correct types, hence you can use this function to convert the return
    value of a Mozzi's function/class member into a pure fractional number.
    @param val The value to be converted into a pure fractional number.
    @return A SFixMath<0,NF> with NF chosen according to the input type
*/
template<typename T>
inline SFixMath<0, sizeof(T)*8-1> toSFraction(T val) {
  return SFixMath<0, sizeof(T)*8-1>::fromRaw(val); 
}

/** Create a *pure* integer signed fixed number (SFixMath) from a integer.
    The number of fractional bits (NI) is chosen automatically depending on the input 
    type. Hence toSInt(127) and toSInt(int8_t(127)) *do not* lead to the 
    same thing: on an AVR, the former will lead to NI=15 - which is overkill -
    whereas the latter will lead to NI=7. Mozzi's objects (Oscil and the like)
    returns correct types, hence you can use this function to convert the return
    value of a Mozzi's function/class member into a pure fractional number.
    @param val The value to be converted into a pure integer fixed math number.
    @return A SFixMath<NI,0> with NI chosen according to the input type
*/
template<typename T>
inline SFixMath<sizeof(T)*8-1,0> toSInt(T val) {
  return SFixMath<sizeof(T)*8-1,0>::fromRaw(val); 
}




#undef MAX
#undef FULLRANGE
//#undef UBITSTOBYTES
//#undef SBITSTOBYTES
//#undef ONESBITMASK


#endif
