/*
http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Int-To-Type
Template meta-programming extras.
*/

#ifndef META_H_
#define META_H_

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

#endif /* META_H_ */
