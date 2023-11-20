/*
 * FixMath2.h
 *
 * Copyright 2023, Thomas Combriat and the Mozzi consortsium
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
signed (SFixMath2) or unsigned (UFixMath2).

A fixed point number has its range defined by the number of bits encoding the integer part (NI 
in the following) and its precision by the number of bits encoding the fractional part (NF).

Like standard C(++) types, the fixed point numbers defined here are following some rules:
 - any fixed type can be converted to another *as long as the value can be represented in the destination type*. Casting to a bigger type in term of NI and NF is safe, but reducing NI can lead to an overflow if the new type cannot hold the integer value and reducing NF leads to a loss of precision.
 - all operations between fixed point number is safe (it won't overflow) and preserve the precision. In particular:
   - only addition, subtraction and multiplication are implemented
   - any operation between a signed and an unsigned leads to a signed number
   - resulting numbers will be casted to a type big enough to store the expected values. It follows that it is worth starting with types that are as small as possible to hold the initial value.
 - all operations between a fixed point number and a native type (int, float, uint) are *not* safe. If the resulting value cannot be represented in the fixed point type it will overflow. Only addition, subtraction, multiplication and right/left shift are implemented.
 - safe right/left shifts, which return the correct value in the correct type are implemented as .sR<shift>() and .sL<shift>() respectively, shift being the shifting amount.
*/




#ifndef FIXMATH2_H_
#define FIXMATH2_H_

#include "IntegerType.h"

#define SHIFTR(x,bits) (bits > 0 ? (x >> (bits)) : (x << (-bits))) // shift right for positive shift numbers, and left for negative ones.
#define MAX(N1,N2) (N1 > N2 ? N1 : N2)

// Experiments
/*#define NBITSREAL(X,N) (abs(X) < (1<<N) ? N : NBITSREAL2(X,N+1))
  #define NBITSREAL2(X,N) (abs(X) < (1<<N) ? N : NBITSREAL(X,N+1))
  #define UFixAuto(X) (UFixMath2<NBITSREAL(X,0),0>(X))
  #define UFixAuto(X) (UFixMath2<NBITSREAL(X,0),0>(X))*/

/*
  template<typename T>
  constexpr byte nBitsReal(T x, byte n=0) {
  if (abs(x) < ((T)1 << n)) {
  return n;
  } else {
  return nBitsReal(x, n + 1);
  }
  }

 #define UFixAuto(X) (UFixMath2<nBitsReal(X),0>(X))
*/

 




// Forward declaration
template<byte NI, byte NF>
class SFixMath2;



/** Instanciate an unsigned fixed point math number.
@param NI The number of bits encoding the integer part
@param NF The number of bits encoding the fractional part
*/
template<byte NI, byte NF=0> // NI and NF being the number of bits for the integral and the fractionnal parts respectively.
class UFixMath2
{
  static_assert(NI+NF<=64, "The total width of a UFixMath2 cannot exceed 64bits");
  typedef typename IntegerType<((NI+NF-1)>>3)+1>::unsigned_type internal_type ; // smallest size that fits our internal integer
  typedef typename IntegerType<((NI+NF)>>3)+1>::unsigned_type next_greater_type ; // smallest size that fits 1<<NF for sure (NI could be equal to 0). It can be the same than internal_type in some cases.
  
public:
  /** Constructor
   */
  UFixMath2() {;}

  /** Constructor from a positive floating point value.
@param fl Floating point value
@return An unsigned fixed point number
  */
  UFixMath2(float fl)  {internal_value = /*static_cast<internal_type>*/(fl * (next_greater_type(1) << NF));}

  /** Constructor from a floating point value.
@param fl Floating point value
@return An unsigned fixed point number
  */
  UFixMath2(double fl)  {internal_value = static_cast<internal_type> (fl * (next_greater_type(1) << NF)); }
  
  /* Constructor from integer type (as_frac = false) or from fractionnal value (as_frac=true) can be used to emulate the behavior of for instance Q8n0_to_Q8n8 */

    /** Constructor from an integer value which can be interpreted as both a resulting fixed point 
math number with a fractional part equals to 0, or as a number with decimal, ie as the underlying type behind the fixed point math number.
@param value Integer value
@param as_raw=false with false value will be interpreted as an integer, with true it will be interpreted as a number with decimals.
@return An unsigned fixed point number
  */
  template<typename T>
  UFixMath2(T value,bool as_raw=false)
  {
    if (as_raw) fromRaw(value);
    else internal_value = (internal_type(value) << NF);
  }


  /** Set the internal value of the fixed point math number.
@param raw The new internal value.
  */  
  template<typename T>
  void fromRaw(T raw) { internal_value = raw; }





  /** Constructor from another UFixMath2. 
@param uf An unsigned fixed type number which value can be represented in this type.
@return A unsigned fixed type number
  */
  template<byte _NI, byte _NF>
  UFixMath2(const UFixMath2<_NI,_NF>& uf) {
    internal_value = SHIFTR((typename IntegerType<((MAX(NI+NF-1,_NI+_NF-1))>>3)+1>::unsigned_type) uf.asRaw(),(_NF-NF));
  }

  /** Constructor from another SFixMath2. 
@param uf An signed fixed type number which value can be represented in this type.
@return A unsigned fixed type number
  */
  template<byte _NI, byte _NF>
  UFixMath2(const SFixMath2<_NI,_NF>& uf) {
    internal_value = SHIFTR((typename IntegerType<((MAX(NI+NF-1,_NI+_NF-1))>>3)+1>::unsigned_type) uf.asRaw(),(_NF-NF));
  }


  //////// ADDITION OVERLOADS

  /** Addition with another UFixMath2. Safe.
@param op The UFixMath2 to be added.
@return The result of the addition as a UFixMath2.
  */
  template<byte _NI, byte _NF>
  UFixMath2<MAX(NI,_NI)+1,MAX(NF,_NF)> operator+ (const UFixMath2<_NI,_NF>& op) const
  {
    constexpr byte new_NI = MAX(NI, _NI) + 1;
    constexpr byte new_NF = MAX(NF, _NF);
    typedef typename IntegerType< ((new_NI+new_NF-1)>>3)+1>::unsigned_type return_type;
    UFixMath2<new_NI,new_NF> left(*this);
    UFixMath2<new_NI,new_NF> right(op);

    return_type tt = return_type(left.asRaw()) + right.asRaw();
    return UFixMath2<new_NI,new_NF>(tt,true);
  }

  /** Addition with another type. Unsafe
@param op The number to be added.
@return The result of the addition as a UFixMath2.
  */
  template<typename T>
  UFixMath2<NI,NF> operator+ (const T op) const
  {
    return UFixMath2<NI,NF>(internal_value+((internal_type)op<<NF),true);
  }


  //////// SUBSTRACTION OVERLOADS

      /** Subtraction with another UFixMath2. Safe.
@param op The UFixMath2 to be subtracted.
@return The result of the subtraction as a UFixMath2.
  */
  template<byte _NI, byte _NF> // We do not have the +1 after MAX(NI, _NI) because the substraction between two UFix should fit in the bigger of the two.
  UFixMath2<MAX(NI,_NI),MAX(NF,_NF)> operator- (const UFixMath2<_NI,_NF>& op) const
  {
    constexpr byte new_NI = MAX(NI, _NI);
    constexpr byte new_NF = MAX(NF, _NF);
    typedef typename IntegerType< ((new_NI+new_NF-1)>>3)+1>::unsigned_type return_type;
    UFixMath2<new_NI,new_NF> left(*this);
    UFixMath2<new_NI,new_NF> right(op);

    return_type tt = return_type(left.asRaw()) - right.asRaw();
    return UFixMath2<new_NI,new_NF>(tt,true);
  }

  
    /** Subtraction with another type. Unsafe
@param op The number to be subtracted.
@return The result of the subtraction as a UFixMath2.
  */
  template<typename T>
  UFixMath2<NI,NF> operator- (const T op) const
  {
    return UFixMath2<NI,NF>(internal_value-((internal_type)op<<NF),true);
  }

    /** Opposite of the number.
@return The opposite numberas a SFixMath2.
  */
  SFixMath2<NI+1,NF> operator-() const
  {
    return SFixMath2<NI+1,NF>( -(typename IntegerType<((NI+NF-1+1)>>3)+1>::signed_type)(internal_value),true);
  }

  //////// MULTIPLICATION OVERLOADS
  
  /** Multiplication with another UFixMath2. Safe.
@param op The UFixMath2 to be multiplied.
@return The result of the multiplication as a UFixMath2.
  */
  template<byte _NI, byte _NF>
  UFixMath2<NI+_NI,NF+_NF> operator* (const UFixMath2<_NI,_NF>& op) const
  {
    typedef typename IntegerType< ((NI+_NI+NF+_NF-1)>>3)+1>::unsigned_type return_type ;
    return_type tt = return_type(internal_value)*op.asRaw();
    return UFixMath2<NI+_NI,NF+_NF>(tt,true);
  }

  /** Multiplication with another type. Unsafe.
@param op The number to be multiplied.
@return The result of the multiplication as a UFixMath2.
  */
  template<typename T>
  UFixMath2<NI,NF> operator* (const T op) const
  {
    return UFixMath2<NI,NF>(internal_value*op,true);
  }


  /** Right shift. This won't overflow but will not leverage on the bits freed by the shift.
Better to use .sR<shift>() if possible instead.
@param op The shift number
@return The result of the shift as a UFixMath2.
  */
  UFixMath2<NI,NF> operator>> (const byte op) const
  {
    return UFixMath2<NI,NF>(internal_value>>op,true);
  }

  /** Left shift. This can overflow if you shift to a value that cannot be represented.
Better to use .sL<shift>() if possible instead.
@param op The shift number
@return The result of the shift as a UFixMath2.
  */
  UFixMath2<NI,NF> operator<< (const byte op) const
  {
    return UFixMath2<NI,NF>(internal_value<<op,true);
  }

  /** Safe and optimal right shift. The returned type will be adjusted accordingly
@param op The shift number
@return The result of the shift as a UFixMath2 of smaller size.
  */
  template<byte op>
  UFixMath2<NI-op,NF> sR()
  {
    return UFixMath2<NI-op,NF>(internal_value>>op,true);
  }

    /** Safe and optimal left shift. The returned type will be adjusted accordingly
@param op The shift number
@return The result of the shift as a UFixMath2 of bigger size.
  */
  template<byte op>
  UFixMath2<NI+op,NF> sL()
  {
    return UFixMath2<NI+op,NF>((typename IntegerType<((NI+op+NF-1)>>3)+1>::unsigned_type) internal_value<<op,true);
  }



  // Division. Might actually more misleading than helping. NON Working version below.
  /*
    template<byte NI1, byte NF1, byte NI2, byte NF2>
    UFixMath2<NI1-NI2, NF1-NF2> operator/(const UFixMath2<NI1, NF1>& op1, const UFixMath2<NI2, NF2>& op2)
    {
    typedef typename IntegerType< ((NI1-NI2+NF1-NF2+1)>>3)+1>::unsigned_type return_type ;
    return_type tt = (return_type(op1.getInt())<<(NF1-NF2))/op2.getInt();
    return UFixMath2<NI1-NI2, NF1-NF2>(tt,true);
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
  
};


/// Reverse overloadings, making a template here leads to an ambiguity, forcing us to specify them one by one??

// Multiplication
template <byte NI, byte NF>
UFixMath2<NI, NF> operator*(uint8_t op, const UFixMath2<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
UFixMath2<NI, NF> operator*(uint16_t op, const UFixMath2<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
UFixMath2<NI, NF> operator*(uint32_t op, const UFixMath2<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
UFixMath2<NI, NF> operator*(uint64_t op, const UFixMath2<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
UFixMath2<NI, NF> operator*(int8_t op, const UFixMath2<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
UFixMath2<NI, NF> operator*(int16_t op, const UFixMath2<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
UFixMath2<NI, NF> operator*(int32_t op, const UFixMath2<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
UFixMath2<NI, NF> operator*(int64_t op, const UFixMath2<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
UFixMath2<NI, NF> operator*(float op, const UFixMath2<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
UFixMath2<NI, NF> operator*(double op, const UFixMath2<NI, NF>& uf) {return uf*op;}

// Addition
template <byte NI, byte NF>
UFixMath2<NI, NF> operator+(uint8_t op, const UFixMath2<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
UFixMath2<NI, NF> operator+(uint16_t op, const UFixMath2<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
UFixMath2<NI, NF> operator+(uint32_t op, const UFixMath2<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
UFixMath2<NI, NF> operator+(uint64_t op, const UFixMath2<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
UFixMath2<NI, NF> operator+(int8_t op, const UFixMath2<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
UFixMath2<NI, NF> operator+(int16_t op, const UFixMath2<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
UFixMath2<NI, NF> operator+(int32_t op, const UFixMath2<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
UFixMath2<NI, NF> operator+(int64_t op, const UFixMath2<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
UFixMath2<NI, NF> operator+(float op, const UFixMath2<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
UFixMath2<NI, NF> operator+(double op, const UFixMath2<NI, NF>& uf) {return uf+op;}

// Substraction
template <byte NI, byte NF>
SFixMath2<NI+1, NF> operator-(uint8_t op, const UFixMath2<NI, NF>& uf) {return -uf+op;}

template <byte NI, byte NF>
SFixMath2<NI+1, NF> operator-(uint16_t op, const UFixMath2<NI, NF>& uf) {return -uf+op;}

template <byte NI, byte NF>
SFixMath2<NI+1, NF> operator-(uint32_t op, const UFixMath2<NI, NF>& uf) {return -uf+op;}

template <byte NI, byte NF>
SFixMath2<NI+1, NF> operator-(uint64_t op, const UFixMath2<NI, NF>& uf) {return -uf+op;}

template <byte NI, byte NF>
SFixMath2<NI+1, NF> operator-(int8_t op, const UFixMath2<NI, NF>& uf) {return -uf+op;}

template <byte NI, byte NF>
SFixMath2<NI+1, NF> operator-(int16_t op, const UFixMath2<NI, NF>& uf) {return -uf+op;}

template <byte NI, byte NF>
SFixMath2<NI+1, NF> operator-(int32_t op, const UFixMath2<NI, NF>& uf) {return -uf+op;}

template <byte NI, byte NF>
SFixMath2<NI+1, NF> operator-(int64_t op, const UFixMath2<NI, NF>& uf) {return -uf+op;}

template <byte NI, byte NF>
SFixMath2<NI+1, NF> operator-(float op, const UFixMath2<NI, NF>& uf) {return -uf+op;}

template <byte NI, byte NF>
SFixMath2<NI+1, NF> operator-(double op, const UFixMath2<NI, NF>& uf) {return -uf+op;}




/** Instanciate an signed fixed point math number.
@param NI The number of bits encoding the integer part
@param NF The number of bits encoding the fractional part
*/
template<byte NI, byte NF> // NI and NF being the number of bits for the integral and the fractionnal parts respectively.
class SFixMath2
{
  static_assert(NI+NF<64, "The total width of a SFixMath2 cannot exceed 63bits");
  typedef typename IntegerType<((NI+NF-1)>>3)+1>::signed_type internal_type ; // smallest size that fits our internal integer
  typedef typename IntegerType<((NI+NF)>>3)+1>::signed_type next_greater_type ; // smallest size that fits 1<<NF for sure (NI could be equal to 0). It can be the same than internal_type in some cases.
  
public:
  /** Constructor
   */
  SFixMath2() {;}
  
    /** Constructor from a floating point value.
@param fl Floating point value
@return An signed fixed point number
  */
  SFixMath2(float fl)  {internal_value = /*static_cast<internal_type>*/(fl * (next_greater_type(1) << NF));}

  /** Constructor from a floating point value.
@param fl Floating point value
@return An signed fixed point number
  */
  SFixMath2(double fl)  {internal_value = static_cast<internal_type> (fl * (next_greater_type(1) << NF)); }


    /** Constructor from an integer value which can be interpreted as both a resulting fixed point 
math number with a fractional part equals to 0, or as a number with decimal, ie as the underlying type behind the fixed point math number.
@param value Integer value
@param as_raw=false with false value will be interpreted as an integer, with true it will be interpreted as a number with decimals.
@return An signed fixed point number
  */
  template<typename T>
  SFixMath2(T value,bool as_raw=false)
  {
    // if (as_frac) internal_value = value;
    if (as_raw) fromRaw(value);
    else internal_value = (internal_type(value) << NF);
  }

    /** Set the internal value of the fixed point math number.
@param raw The new internal value.
  */  
  template<typename T>
  void fromRaw(T raw) { internal_value = raw; }


  /** Constructor from another sFixMath2. 
@param uf A signed fixed type number which value can be represented in this type.
@return A signed fixed type number
  */
  template<byte _NI, byte _NF>
  SFixMath2(const SFixMath2<_NI,_NF>& uf) {
    internal_value = SHIFTR((typename IntegerType<((MAX(NI+NF-1,_NI+_NF-1))>>3)+1>::unsigned_type) uf.asRaw(),(_NF-NF));
  }

  /** Constructor from another UFixMath2. 
@param uf A unsigned fixed type number.
@return A signed fixed type number
  */
  template<byte _NI, byte _NF>
  SFixMath2(const UFixMath2<_NI,_NF>& uf) {
    internal_value = SHIFTR((typename IntegerType<((MAX(NI+NF-1,_NI+_NF-1))>>3)+1>::unsigned_type) uf.asRaw(),(_NF-NF));
  }

  //////// ADDITION OVERLOADS

    /** Addition with another SFixMath2. Safe.
@param op The SFixMath2 to be added.
@return The result of the addition as a SFixMath2.
  */
  template<byte _NI, byte _NF>
  SFixMath2<MAX(NI,_NI)+1,MAX(NF,_NF)> operator+ (const SFixMath2<_NI,_NF>& op) const
  {
    constexpr byte new_NI = MAX(NI, _NI) + 1;
    constexpr byte new_NF = MAX(NF, _NF);
    typedef typename IntegerType< ((new_NI+new_NF-1)>>3)+1>::unsigned_type return_type;
    SFixMath2<new_NI,new_NF> left(*this);
    SFixMath2<new_NI,new_NF> right(op);

    return_type tt = return_type(left.asRaw()) + right.asRaw();
    return SFixMath2<new_NI,new_NF>(tt,true);
  }

  /** Addition with another type. Unsafe
@param op The number to be added.
@return The result of the addition as a UFixMath2.
  */
  template<typename T>
  SFixMath2<NI,NF> operator+ (const T op) const
  {
    return SFixMath2<NI,NF>(internal_value+(op<<NF),true);
  }


  //////// SUBSTRACTION OVERLOADS

       /** Subtraction with another SFixMath2. Safe.
@param op The SFixMath2 to be subtracted.
@return The result of the subtraction as a SFixMath2.
  */ 
  template<byte _NI, byte _NF> // We do not have the +1 after MAX(NI, _NI) because the substraction between two SFix should fit in the bigger of the two.
  SFixMath2<MAX(NI,_NI),MAX(NF,_NF)> operator- (const SFixMath2<_NI,_NF>& op) const
  {
    constexpr byte new_NI = MAX(NI, _NI);
    constexpr byte new_NF = MAX(NF, _NF);
    typedef typename IntegerType< ((new_NI+new_NF-1)>>3)+1>::unsigned_type return_type;
    SFixMath2<new_NI,new_NF> left(*this);
    SFixMath2<new_NI,new_NF> right(op);

    return_type tt = return_type(left.asRaw()) - right.asRaw();
    return SFixMath2<new_NI,new_NF>(tt,true);
  }

  
    /** Subtraction with another type. Unsafe
@param op The number to be subtracted.
@return The result of the subtraction as a SFixMath2.
  */
  template<typename T>
  SFixMath2<NI,NF> operator- (const T op) const
  {
    return SFixMath2<NI,NF>(internal_value-(op<<NF),true);
  }


    /** Opposite of the number.
@return The opposite numberas a SFixMath2.
  */
  SFixMath2<NI,NF> operator-() const
  {
    return SFixMath2<NI,NF>(-internal_value,true);
  }
  
  //////// MULTIPLICATION OVERLOADS
  
  /** Multiplication with another SFixMath2. Safe.
@param op The SFixMath2 to be multiplied.
@return The result of the multiplication as a SFixMath2.
  */
  template<byte _NI, byte _NF>
  SFixMath2<NI+_NI,NF+_NF> operator* (const SFixMath2<_NI,_NF>& op) const
  {
    typedef typename IntegerType< ((NI+_NI+NF+_NF-1)>>3)+1>::signed_type return_type ;
    return_type tt = return_type(internal_value)*op.asRaw();
    return SFixMath2<NI+_NI,NF+_NF>(tt,true);
  }

  /** Multiplication with another type. Unsafe.
@param op The number to be multiplied.
@return The result of the multiplication as a UFixMath2.
  */
  template<typename T>
  SFixMath2<NI,NF> operator* (const T op) const
  {
    return SFixMath2<NI,NF>(internal_value*op,true);
  }


  /** Right shift. This won't overflow but will not leverage on the bits freed by the shift.
Better to use .sR<shift>() if possible instead.
@param op The shift number
@return The result of the shift as a SFixMath2.
  */
  SFixMath2<NI,NF> operator>> (const byte op) const
  {
    return SFixMath2<NI,NF>(internal_value>>op,true);
  }

  /** Left shift. This can overflow if you shift to a value that cannot be represented.
Better to use .sL<shift>() if possible instead.
@param op The shift number
@return The result of the shift as a UFixMath2.
  */
  SFixMath2<NI,NF> operator<< (const byte op) const
  {
    return SFixMath2<NI,NF>(internal_value<<op,true);
  }

    /** Safe and optimal right shift. The returned type will be adjusted accordingly
@param op The shift number
@return The result of the shift as a UFixMath2 of smaller size.
  */
  template<byte op>
  SFixMath2<NI-op,NF> sR()
  {
    return SFixMath2<NI-op,NF>(internal_value>>op,true);
  }

      /** Safe and optimal left shift. The returned type will be adjusted accordingly
@param op The shift number
@return The result of the shift as a UFixMath2 of bigger size.
  */
  template<byte op>
  SFixMath2<NI+op,NF> sL()
  {
    return SFixMath2<NI+op,NF>((typename IntegerType<((NI+op+NF-1)>>3)+1>::signed_type) internal_value<<op,true);
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

};

/// Reverse overloadings, 

// Multiplication
template <byte NI, byte NF>
SFixMath2<NI, NF> operator*(uint8_t op, const SFixMath2<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator*(uint16_t op, const SFixMath2<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator*(uint32_t op, const SFixMath2<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator*(uint64_t op, const SFixMath2<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator*(int8_t op, const SFixMath2<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator*(int16_t op, const SFixMath2<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator*(int32_t op, const SFixMath2<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator*(int64_t op, const SFixMath2<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator*(float op, const SFixMath2<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator*(double op, const SFixMath2<NI, NF>& uf) {return uf*op;}

// Addition
template <byte NI, byte NF>
SFixMath2<NI, NF> operator+(uint8_t op, const SFixMath2<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator+(uint16_t op, const SFixMath2<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator+(uint32_t op, const SFixMath2<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator+(uint64_t op, const SFixMath2<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator+(int8_t op, const SFixMath2<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator+(int16_t op, const SFixMath2<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator+(int32_t op, const SFixMath2<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator+(int64_t op, const SFixMath2<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator+(float op, const SFixMath2<NI, NF>& uf) {return uf+op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator+(double op, const SFixMath2<NI, NF>& uf) {return uf+op;}

// Substraction
template <byte NI, byte NF>
SFixMath2<NI, NF> operator-(uint8_t op, const SFixMath2<NI, NF>& uf) {return (-uf)+op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator-(uint16_t op, const SFixMath2<NI, NF>& uf) {return (-uf)+op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator-(uint32_t op, const SFixMath2<NI, NF>& uf) {return (-uf)+op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator-(uint64_t op, const SFixMath2<NI, NF>& uf) {return (-uf)+op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator-(int8_t op, const SFixMath2<NI, NF>& uf) {return (-uf)+op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator-(int16_t op, const SFixMath2<NI, NF>& uf) {return (-uf)+op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator-(int32_t op, const SFixMath2<NI, NF>& uf) {return (-uf)+op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator-(int64_t op, const SFixMath2<NI, NF>& uf) {return (-uf)+op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator-(float op, const SFixMath2<NI, NF>& uf) {return (-uf)+op;}

template <byte NI, byte NF>
SFixMath2<NI, NF> operator-(double op, const SFixMath2<NI, NF>& uf) {return (-uf)+op;}









  /** Multiplication between a SFixMath2 and a UFixMath2. Safe.
@param op1 A SFixMath2
@param op2 A UFixMath2
@return The result of the multiplication of op1 and op2. As a SFixMath2
  */
template<byte NI, byte NF, byte _NI, byte _NF>
SFixMath2<NI+_NI+1,NF+_NF> operator* (const SFixMath2<NI,NF>& op1, const UFixMath2<_NI,_NF>& op2 )
{
  typedef typename IntegerType< ((NI+_NI+1+NF+_NF-1)>>3)+1>::signed_type return_type ;
  return_type tt = return_type(op1.asRaw())*op2.asRaw();
  return SFixMath2<NI+_NI+1,NF+_NF>(tt,true);
}

  /** Multiplication between a UFixMath2 and a SFixMath2. Safe.
@param op1 A UFixMath2
@param op2 A SFixMath2
@return The result of the multiplication of op1 and op2. As a SFixMath2
  */
template<byte NI, byte NF, byte _NI, byte _NF>
SFixMath2<NI+_NI+1,NF+_NF> operator* (const UFixMath2<NI,NF>& op1, const SFixMath2<_NI,_NF>& op2 )
{
  return op2*op1;
}


// Addition between SFixMath2 and UFixMath2 (promotion to next SFixMath2)


  /** Addition between a SFixMath2 and a UFixMath2. Safe.
@param op1 A SFixMath2
@param op2 A UFixMath2
@return The result of the addition of op1 and op2. As a SFixMath2
  */
template<byte NI, byte NF, byte _NI, byte _NF>
SFixMath2<MAX(NI,_NI)+1,MAX(NF,_NF)> operator+ (const SFixMath2<NI,NF>& op1, const UFixMath2<_NI,_NF>& op2 )
{
  constexpr byte new_NI = MAX(NI, _NI) + 1;
  constexpr byte new_NF = MAX(NF, _NF);
    
  typedef typename IntegerType< ((new_NI+new_NF-1)>>3)+1>::signed_type return_type;
  SFixMath2<new_NI,new_NF> left(op1);
  SFixMath2<new_NI,new_NF> right(op2);
  return_type tt = return_type(left.asRaw()) + right.asRaw();
  return SFixMath2<new_NI,new_NF>(tt,true);
}

  /** Addition between a UFixMath2 and a SFixMath2. Safe.
@param op1 A UFixMath2
@param op2 A SFixMath2
@return The result of the addition of op1 and op2. As a SFixMath2
  */
template<byte NI, byte NF, byte _NI, byte _NF>
SFixMath2<MAX(NI,_NI)+1,MAX(NF,_NF)> operator+ (const UFixMath2<NI,NF>& op1, const SFixMath2<_NI,_NF>& op2 )
{
  return op2+op1;
}

// Substraction between SFixMath2 and UFixMath2 (promotion to next SFixMath2)

  /** Subtraction between a SFixMath2 and a UFixMath2. Safe.
@param op1 A SFixMath2
@param op2 A UFixMath2
@return The result of the subtraction of op1 by op2. As a SFixMath2
  */
template<byte NI, byte NF, byte _NI, byte _NF>
SFixMath2<MAX(NI,_NI)+1,MAX(NF,_NF)> operator- (const SFixMath2<NI,NF>& op1, const UFixMath2<_NI,_NF>& op2 )
{
  constexpr byte new_NI = MAX(NI, _NI) + 1;
  constexpr byte new_NF = MAX(NF, _NF);
    
  typedef typename IntegerType< ((new_NI+new_NF-1)>>3)+1>::signed_type return_type;
  SFixMath2<new_NI,new_NF> left(op1);
  SFixMath2<new_NI,new_NF> right(op2);
  return_type tt = return_type(left.asRaw()) - right.asRaw();
  return SFixMath2<new_NI,new_NF>(tt,true);
}

  /** Subtraction between a UFixMath2 and a SFixMath2. Safe.
@param op1 A UFixMath2
@param op2 A SFixMath2
@return The result of the subtraction of op1 by op2. As a SFixMath2
  */
template<byte NI, byte NF, byte _NI, byte _NF>
SFixMath2<MAX(NI,_NI)+1,MAX(NF,_NF)> operator- (const UFixMath2<NI,NF>& op1, const SFixMath2<_NI,_NF>& op2 )
{
  return -op2+op1;
  /* constexpr byte new_NI = MAX(NI, _NI) + 1;
  constexpr byte new_NF = MAX(NF, _NF);
    
  typedef typename IntegerType< ((new_NI+new_NF-1)>>3)+1>::signed_type return_type;
  SFixMath2<new_NI,new_NF> left(op1);
  SFixMath2<new_NI,new_NF> right(op2);
  return_type tt = return_type(left.asRaw()) - right.asRaw();
  return SFixMath2<new_NI,new_NF>(tt,true);*/
}


#endif
