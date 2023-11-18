/*
 * FixMath2.h
 *
 * Copyright 2012 Tim Barrass
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons
 * Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */


#ifndef FIXMATH2_H_
#define FIXMATH2_H_

#include "IntegerType.h"

#define SHIFTR(x,bits) (bits > 0 ? (x >> (bits)) : (x << (-bits))) // shift right for positive shift numbers, and left for negative ones.
#define MAX(N1,N2) (N1 > N2 ? N1 : N2)

template<byte NI, byte NF> // NI and NF being the number of bits for the integral and the fractionnal parts respectively.
class UFixMath2
{
  static_assert(NI+NF<=64, "The total width of a UFixMath2 cannot exceed 64bits");
  typedef typename IntegerType<((NI+NF-1)>>3)+1>::unsigned_type internal_type ; // smallest size that fits our internal integer
  typedef typename IntegerType<((NI+NF)>>3)+1>::unsigned_type next_greater_type ; // smallest size that fits 1<<NF for sure (NI could be equal to 0). It can be the same than internal_type in some cases.
  
public:
  /** Constructor
   */
  UFixMath2() {;}
  
  /* Constructor from float */
  UFixMath2(float fl)  {internal_value = /*static_cast<internal_type>*/(fl * (next_greater_type(1) << NF));}

  /* Constructor from double */
  UFixMath2(double fl)  {internal_value = static_cast<internal_type> (fl * (next_greater_type(1) << NF)); }
  
  /* Constructor from integer type (as_frac = false) or from fractionnal value (as_frac=true) can be used to emulate the behavior of for instance Q8n0_to_Q8n8 */
  template<typename T>
  UFixMath2(T value,bool as_raw=false)
  {
    if (as_raw) fromRaw(value);
    else internal_value = (internal_type(value) << NF);
  }
  
  template<typename T>
  void fromRaw(T raw) { internal_value = raw; }

  /* template<typename T>
     static UFixMath2<NI, NF>fromRaw(T raw) {
     UFixMath2<NI, NF> ret;
     ret.internal_value = raw;
     return ret;
     }*/

  /* Constructors from signed types
   */

  /* Constructor from different integer and fractionnal part, to remove? */
  /*  UFixMath2(typename IntegerType<((NI)>>3)>::unsigned_type integral_part, typename IntegerType<((NF)>>3)>::unsigned_type fractionnal_part)
      {
      internal_value = (integral_part << NI) + fractionnal_part;
      }*/
  // probably a more confusing than anything constructor! TO REMOVE


  /* Constructor from another fixed type */
  template<byte _NI, byte _NF>
  UFixMath2(const UFixMath2<_NI,_NF>& uf) {
    internal_value = SHIFTR((typename IntegerType<((MAX(NI+NF-1,_NI+_NF-1))>>3)+1>::unsigned_type) uf.asRaw(),(_NF-NF));
  }


  //////// ADDITION OVERLOADS
  // Between UFix
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



  //////// SUBSTRACTION OVERLOADS
  // Between UFix
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

  //////// MULTIPLICATION OVERLOADS
  
  // Multiplication overload between Ufixed type, returns the compound type, safe
  template<byte _NI, byte _NF>
  UFixMath2<NI+_NI,NF+_NF> operator* (const UFixMath2<_NI,_NF>& op) const
  {
    typedef typename IntegerType< ((NI+_NI+NF+_NF-1)>>3)+1>::unsigned_type return_type ;
    return_type tt = return_type(internal_value)*op.asRaw();
    return UFixMath2<NI+_NI,NF+_NF>(tt,true);
  }

  // Multiplication with any other type: directly to the internal_value, potential overflow
  template<typename T>
  UFixMath2<NI,NF> operator* (const T op) const
  {
    return UFixMath2<NI,NF>(internal_value*op,true);
  }


  // Right shift operator
  UFixMath2<NI-1,NF> operator>> (const byte op) const
  {
    return UFixMath2<NI-1,NF>(internal_value>>op,true);
  }

  // Left shift operator
  UFixMath2<NI+1,NF> operator<< (const byte op) const
  {
    return UFixMath2<NI+1,NF>(internal_value<<op,true);
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
  
  float asFloat() { return (static_cast<float>(internal_value)) / (next_greater_type(1)<<NF); }
  internal_type asRaw() const { return internal_value; }
  

  /* byte getNI(){return NI;}
     byte getNF(){return NF;}
  */
private:
  internal_type internal_value;
  
};


/// Reverse overloadings, making a template here leads to an ambiguity, forcing us to specify them one by one??


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





///////////// SIGNED TYPE

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
  
  /* Constructor from float */
  SFixMath2(float fl)  {internal_value = /*static_cast<internal_type>*/(fl * (next_greater_type(1) << NF));}

  /* Constructor from double */
  SFixMath2(double fl)  {internal_value = static_cast<internal_type> (fl * (next_greater_type(1) << NF)); }


  /* Constructor from other types (int)*/
  template<typename T>
  SFixMath2(T value,bool as_raw=false)
  {
    // if (as_frac) internal_value = value;
    if (as_raw) fromRaw(value);
    else internal_value = (internal_type(value) << NF);
  }
  
  template<typename T>
  void fromRaw(T raw) { internal_value = raw; }



  
  //////// MULTIPLICATION OVERLOADS
  
  // Multiplication overload between fixed type, returns the compound type
  template<byte _NI, byte _NF>
  SFixMath2<NI+_NI,NF+_NF> operator* (const SFixMath2<_NI,_NF>& op) const
  {
    typedef typename IntegerType< ((NI+_NI+NF+_NF-1)>>3)+1>::signed_type return_type ;
    return_type tt = return_type(internal_value)*op.asRaw();
    return SFixMath2<NI+_NI,NF+_NF>(tt,true);
  }

  // Multiplication with any other type: directly to the internal_value
  template<typename T>
  SFixMath2<NI,NF> operator* (const T op) const
  {
    return SFixMath2<NI,NF>(internal_value*op,true);
  }


  // Right shift operator
  SFixMath2<NI-1,NF> operator>> (const byte op) const
  {
    return SFixMath2<NI-1,NF>(internal_value>>op,true);
  }

  // Left shift operator
  SFixMath2<NI+1,NF> operator<< (const byte op) const
  {
    return SFixMath2<NI+1,NF>(internal_value<<op,true);
  }

  



  float asFloat() { return (static_cast<float>(internal_value)) / (next_greater_type(1)<<NF); }
  internal_type asRaw() const { return internal_value; }

private:
  internal_type internal_value;

};

/// Reverse overloadings, 


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








// Multiplications between SFixMath2 and UFixMath2

template<byte NI, byte NF, byte _NI, byte _NF>
SFixMath2<NI+_NI+1,NF+_NF> operator* (const SFixMath2<NI,NF>& op1, const UFixMath2<_NI,_NF>& op2 )
{
  typedef typename IntegerType< ((NI+_NI+1+NF+_NF-1)>>3)+1>::signed_type return_type ;
  return_type tt = return_type(op1.asRaw())*op2.asRaw();
  return SFixMath2<NI+_NI+1,NF+_NF>(tt,true);
}

template<byte NI, byte NF, byte _NI, byte _NF>
SFixMath2<NI+_NI+1,NF+_NF> operator* (const UFixMath2<NI,NF>& op1, const SFixMath2<_NI,_NF>& op2 )
{
  typedef typename IntegerType< ((NI+_NI+1+NF+_NF-1)>>3)+1>::signed_type return_type ;
  return_type tt = return_type(op1.asRaw())*op2.asRaw();
  return SFixMath2<NI+_NI+1,NF+_NF>(tt,true);
}




#endif
