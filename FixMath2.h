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
    // if (as_frac) internal_value = value;
    if (as_raw) fromRaw(value);
    else internal_value = (internal_type(value) << NF);
  }
  
  template<typename T>
  void fromRaw(T raw) { internal_value = raw; }

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
 

  //////// MULTIPLICATION OVERLOADS
  
  // Multiplication overload between fixed type, returns the compound type
  template<byte _NI, byte _NF>
  UFixMath2<NI+_NI,NF+_NF> operator* (const UFixMath2<_NI,_NF>& op) const
  {
    typedef typename IntegerType< ((NI+_NI+NF+_NF-1)>>3)+1>::unsigned_type return_type ;
    return_type tt = return_type(internal_value)*op.asRaw();
    return UFixMath2<NI+_NI,NF+_NF>(tt,true);
  }

  // Multiplication with any other type: directly to the internal_value
  template<typename T>
  UFixMath2<NI,NF> operator* (const T op) const
  {
    return UFixMath2<NI,NF>(internal_value*op,true);
  }

  /*    template<typename T>
	UFixMath2<NI,NF> operator* (const T op, const UFixMath2<NI, NF>& uf)
	{
	return UFixMath2<NI,NF>(uf.asRaw()*op,true);
	}
  */
    
  /* version 1, prior promotion/demotion to return type, safe */
  /*   byte sub = NF>>1;
       internal_type tt = (internal_value>>sub) * (UFixMath2<NI,NF>(op).getInt() >> (sub));
       return  UFixMath2<NI,NF>(tt,true);*/

  /* version 2, optimisation (?) of the former: one shift instead of two (at cast and here)
     Keeps the bigger type for the operand, shift it by the appropriate number, cast it to the final type (which might be the same) and multiply by the internal_value, shifted of course */
  /*  byte sub = NF>>1;
      internal_type tt = (internal_value>>sub) * internal_type((SHIFTR(typename IntegerType<((MAX(NI+NF,_NI+_NF))>>3)>::unsigned_type(op.getInt()), (_NF-sub))));
      return  UFixMath2<NI,NF>(tt,true);*/
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
UFixMath2<NI, NF> operator*(float op, const UFixMath2<NI, NF>& uf) {return uf*op;}

template <byte NI, byte NF>
UFixMath2<NI, NF> operator*(double op, const UFixMath2<NI, NF>& uf) {return uf*op;}
/*
  template<typename T,byte NI, byte NF>
  UFixMath2<NI,NF> operator* (const T op, const UFixMath2<NI, NF>& uf)
  {
  return UFixMath2<NI,NF>(uf.asRaw()*op,true);
  }*/




#endif
