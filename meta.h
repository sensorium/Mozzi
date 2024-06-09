/*
 * meta.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2012-2024 Tim Barrass and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
 */

/*
http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Int-To-Type
Template meta-programming extras.
*/

#ifndef META_H_
#define META_H_

/** @defgroup soundtables Look-up-tables and python scripts to generate tables or convert sounds.

Look-up-tables and python scripts to generate tables or convert sounds. Includes ready-to-use wave tables and a few example samples which are in the Mozzi/tables and Mozzi/samples folders. You can convert your own sounds from a program like Audacity to tables for Mozzi with a script called char2mozzi.py, in Mozzi/python. Read the int8_t2mozzi.py file for instructions. Also check out the other scripts in the python folder for templates to use if you want to do your own thing.
*/

/** @defgroup util Utility functions, and debugging

Utility functions. Includes functions for debugging and profiling high frequency code with an oscilloscope when serial is too slow, some miscellaneous functions used internally by Mozzi, python scripts for converting or generating sound tables, and assorted meta-programming utils.
*/

/** @ingroup util
Enables you to instantiate a template based on an integer value.
For example, this is used in StateVariable.h to choose a different next()
function for each kind of filter, LOWPASS, BANDPASS, HIGHPASS or NOTCH, which are simple
integer values 0,1,2,3 provided to the StateVariable template on instantiation.
Fantastic!
It's in C++11, but not yet available in avr-gcc.
See "c++ enable_if"
*/
template <int I>
struct Int2Type
{
  enum {
    value = I      };
};


/*
//from http://en.wikibooks.org/wiki/C%2B%2B_Programming/Templates/Template_Meta-Programming#Compile-time_programming

//First, the general (unspecialized) template says that factorial<n>::value is given by n*factorial<n-1>::value:
template <unsigned n>
struct factorial
{
  enum { value = n * factorial<n-1>::value };
};


//Next, the specialization for zero says that factorial<0>::value evaluates to 1:
template <>
struct factorial<0>
{
  enum { value = 1 };
};


//And now some code that "calls" the factorial template at compile-time:
// Because calculations are done at compile-time, they can be
// used for things such as array sizes, eg.
//  int array[ factorial<7>::value ];

*/
  
#endif /* META_H_ */
